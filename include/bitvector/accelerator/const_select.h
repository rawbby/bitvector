#ifndef CONST_SELECT_H
#define CONST_SELECT_H

#include "accessor.h"
#include "const_rank.h"
#include "linear_select.h"

#include "../bitvector.h"
#include "../traits.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

class ConstSelect
{
public:
  const struct Sizes
  {
    std::size_t bits;
    std::size_t occurrences_per_l1_chunk;
    std::size_t occurrences_per_l2_chunk;
    std::size_t l1_chunks_element_bits;
    std::size_t l2_chunks_element_bits;
    std::size_t l2_number_of_chunks;
    std::size_t l1_chunk_sparse_lookup_entry_bits;
    std::size_t l2_chunk_sparse_lookup_entry_bits;
    std::size_t l1_chunk_sparse_border;
    std::size_t l2_chunk_sparse_border;
    std::size_t max_sparse_chunks;
    std::size_t sparse_index_bits;

    explicit Sizes(std::size_t n)
    {
      bits = n;
      occurrences_per_l1_chunk = Log2Pow2(n);
      occurrences_per_l2_chunk = Sqrt(Log2(n));
      l1_chunks_element_bits = Log2(n) + 1;
      l1_chunk_sparse_border = Log2Pow4(n);
      l2_chunk_sparse_border = Log2(n);

      // l1_chunk_sparse_border is the maximum offset as relative but for medium and small values
      // this one is bigger than total index ...
      l2_chunks_element_bits = Min(Log2(l1_chunk_sparse_border) + 1, l1_chunks_element_bits);
      l2_number_of_chunks = CeilDiv(occurrences_per_l1_chunk, occurrences_per_l2_chunk);

      l1_chunk_sparse_lookup_entry_bits = Log2(n) + 1;
      l2_chunk_sparse_lookup_entry_bits = Log2(n) + 1;
      max_sparse_chunks = CeilDiv(n, l1_chunk_sparse_border);
      sparse_index_bits = Log2(max_sparse_chunks) + 1;
    }
  } sizes;

private:
  std::shared_ptr<BitVector> bit_vector;

  BitsVector l1_0chunks;
  std::shared_ptr<BitVector> l1_0chunk_sparse;
  BitVectorAccessor<ConstRank, LinearSelect> l1_0chunk_sparse_access;
  BitsVector l1_0chunk_sparse_lookup;

  BitsVector l2_0chunks;
  std::shared_ptr<BitVector> l2_0chunk_sparse;
  BitVectorAccessor<ConstRank, LinearSelect> l2_0chunk_sparse_access;
  BitsVector l2_0chunk_sparse_lookup;

  BitsVector l1_1chunks;
  std::shared_ptr<BitVector> l1_1chunk_sparse;
  BitVectorAccessor<ConstRank, LinearSelect> l1_1chunk_sparse_access;
  BitsVector l1_1chunk_sparse_lookup;

  BitsVector l2_1chunks;
  std::shared_ptr<BitVector> l2_1chunk_sparse;
  BitVectorAccessor<ConstRank, LinearSelect> l2_1chunk_sparse_access;
  BitsVector l2_1chunk_sparse_lookup;

public:
  explicit ConstSelect(std::shared_ptr<BitVector> bit_vector)
    : bit_vector(std::move(bit_vector))
    , sizes(bit_vector->size())

    , l1_0chunks(sizes.l1_chunks_element_bits)
    , l1_0chunk_sparse(std::make_shared<BitVector>())
    , l1_0chunk_sparse_access(l1_0chunk_sparse)
    , l1_0chunk_sparse_lookup(sizes.l1_chunk_sparse_lookup_entry_bits)
    , l1_1chunks(sizes.l1_chunks_element_bits)
    , l1_1chunk_sparse(std::make_shared<BitVector>())
    , l1_1chunk_sparse_access(l1_1chunk_sparse)
    , l1_1chunk_sparse_lookup(sizes.l1_chunk_sparse_lookup_entry_bits)

    , l2_0chunks(sizes.l2_chunks_element_bits)
    , l2_0chunk_sparse(std::make_shared<BitVector>())
    , l2_0chunk_sparse_access(l2_0chunk_sparse)
    , l2_0chunk_sparse_lookup(sizes.l2_chunk_sparse_lookup_entry_bits)
    , l2_1chunks(sizes.l2_chunks_element_bits)
    , l2_1chunk_sparse(std::make_shared<BitVector>())
    , l2_1chunk_sparse_access(l2_1chunk_sparse)
    , l2_1chunk_sparse_lookup(sizes.l2_chunk_sparse_lookup_entry_bits)
  {
  }

public:
  [[maybe_unused]] [[nodiscard]] std::size_t bits() const
  {
    return l1_0chunks.bits() + l1_0chunk_sparse->bits() + l1_0chunk_sparse_access.bits() + l1_0chunk_sparse_lookup.bits() + l2_0chunks.bits() + l2_0chunk_sparse->bits() +
           l2_0chunk_sparse_access.bits() + l2_0chunk_sparse_lookup.bits() + l1_1chunks.bits() + l1_1chunk_sparse->bits() + l1_1chunk_sparse_access.bits() +
           l1_1chunk_sparse_lookup.bits() + l2_1chunks.bits() + l2_1chunk_sparse->bits() + l2_1chunk_sparse_access.bits() + l2_1chunk_sparse_lookup.bits();
  }

  void build()
  {
    const auto n = bit_vector->size();
    std::size_t zeroes = 0;
    std::size_t ones = 0;
    std::size_t zero_chunk_size = 0;
    std::size_t one_chunk_size = 0;

    // std::size_t chunk = 1;
    l1_0chunks.push(0); // first chunk has no offset
    l1_1chunks.push(0); // first chunk has no offset

    auto zero_lookup_entry = BitsVector(sizes.l1_chunk_sparse_lookup_entry_bits);
    auto one_lookup_entry = BitsVector(sizes.l1_chunk_sparse_lookup_entry_bits);

    for (std::size_t i = 0; i < n; ++i) {
      ++zero_chunk_size;
      ++one_chunk_size;

      if (!bit_vector->access(i)) {
        ++zeroes;
        zero_lookup_entry.push(i);

        if (!(zeroes % sizes.occurrences_per_l1_chunk)) { // end of current chunk
          l1_0chunks.push(i + 1);

          if (zero_chunk_size < sizes.l1_chunk_sparse_border) { // dense
            l1_0chunk_sparse->push(false);
            // subdivide

            l2_0chunks.push(0);
            for (std::size_t j = 1; j < sizes.l2_number_of_chunks; ++j) {

              l2_0chunks.push(zero_lookup_entry[j * sizes.occurrences_per_l2_chunk].get() + 1);

              const auto ti = l2_0chunks.size() - 1;
              const auto pi = l2_0chunks.size() - 2;
              const auto chunk_size = l2_0chunks[ti].get() - l2_0chunks[pi].get();
              if (chunk_size < sizes.l2_chunk_sparse_border) { // dense
                l2_0chunk_sparse->push(false);
              } else { // sparse
                l2_0chunk_sparse->push(true);

                // add lookup
                for (std::size_t k = 0; k < sizes.l2_number_of_chunks; ++k) {
                  for (std::size_t l = 0; l < sizes.occurrences_per_l2_chunk; ++l) {
                    l2_0chunk_sparse_lookup.push(zero_lookup_entry[k * sizes.occurrences_per_l2_chunk + l].get());
                  }
                }
              }
            }

            // std::cout << "l2_0chunks len: " << l2_0chunks.size() << ' ' << zeroes << std::endl;

          } else { // sparse
            l1_0chunk_sparse->push(true);
            // add lookup
            auto m = zero_lookup_entry.size();
            for (std::size_t j = 0; j < m; ++j)
              l1_0chunk_sparse_lookup.push(zero_lookup_entry[j].get());
          }

          zero_lookup_entry = BitsVector(sizes.l1_chunk_sparse_lookup_entry_bits); // TODO push and clear has a bug :(
          zero_chunk_size = 0;
        }
      } else {
        ++ones;

        if (!(ones % sizes.occurrences_per_l1_chunk)) { // end of current chunk
          l1_1chunks.push(i + 1);

          if (one_chunk_size < sizes.l1_chunk_sparse_border) { // dense
            l1_1chunk_sparse->push(false);

          } else { // sparse
            l1_1chunk_sparse->push(true);
            // add lookup
            auto m = one_lookup_entry.size();
            for (std::size_t j = 0; j < m; ++j)
              l1_1chunk_sparse_lookup.push(one_lookup_entry[j].get());
          }

          one_lookup_entry = BitsVector(sizes.l1_chunk_sparse_lookup_entry_bits);
          one_chunk_size = 0;
        }
      }
    }

    // last sparse entries
    if (zero_chunk_size < sizes.l1_chunk_sparse_border) {
      l1_0chunk_sparse->push(false);
    } else {
      l1_0chunk_sparse->push(true);
    }
    if (one_chunk_size < sizes.l1_chunk_sparse_border) {
      l1_1chunk_sparse->push(false);
    } else {
      l1_1chunk_sparse->push(true);
    }

    l1_0chunk_sparse_access.build();
    l1_1chunk_sparse_access.build();
    l2_0chunk_sparse_access.build();
    l2_1chunk_sparse_access.build();
  }

  [[maybe_unused]] [[nodiscard]] std::size_t select0(std::size_t index) const
  {
    if (!index)
      return 0;

    const auto chunk_index = index / sizes.occurrences_per_l1_chunk;
    const auto chunk_rest = index % sizes.occurrences_per_l1_chunk;
    if (l1_0chunk_sparse->access(chunk_index))
      return l1_0chunk_sparse_lookup[l1_0chunk_sparse_access.rank1(chunk_index) * sizes.occurrences_per_l1_chunk + chunk_rest].get();

    const auto skipped_b = chunk_index * sizes.occurrences_per_l1_chunk;

    auto s = static_cast<std::size_t>(l1_0chunks[chunk_index]);
    for (std::size_t b_sum = skipped_b; b_sum < index; ++s)
      b_sum += (1 - static_cast<std::size_t>(bit_vector->access(s)));

    return s - 1;
  }

  [[maybe_unused]] [[nodiscard]] std::size_t select1(std::size_t index) const
  {
    if (!index)
      return 0;

    const auto chunk_index = index / sizes.occurrences_per_l1_chunk;
    const auto chunk_rest = index % sizes.occurrences_per_l1_chunk;
    if (l1_1chunk_sparse->access(chunk_index))
      return l1_1chunk_sparse_lookup[l1_1chunk_sparse_access.rank1(chunk_index) * sizes.occurrences_per_l1_chunk + chunk_rest].get();

    const auto skipped_b = chunk_index * sizes.occurrences_per_l1_chunk;

    auto s = static_cast<std::size_t>(l1_1chunks[chunk_index]);
    for (std::size_t b_sum = skipped_b; b_sum < index; ++s)
      b_sum += static_cast<std::size_t>(bit_vector->access(s));

    return s - 1;
  }
};

static_assert(Select<ConstSelect>);

#endif
