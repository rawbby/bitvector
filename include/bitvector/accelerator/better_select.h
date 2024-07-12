#ifndef CONST_SELECT_H
#define CONST_SELECT_H

#include "accessor.h"
#include "const_rank.h"
#include "linear_select.h"

#include <iostream>

#include "../bitvector.h"
#include "../traits.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

class BetterSelect
{
public:
  const struct Sizes
  {
    std::size_t bits;

    std::size_t l1_occurrences_per_chunk;
    std::size_t l1_chunks_element_bits;
    std::size_t l1_chunk_sparse_lookup_entry_bits;
    std::size_t l1_chunk_sparse_border;
    std::size_t l1_max_sparse_chunks;
    std::size_t l1_sparse_index_bits;

    // std::size_t l2_occurrences_per_chunk;
    // std::size_t l2_chunks_element_bits;
    // std::size_t l2_chunk_sparse_lookup_entry_bits;
    // std::size_t l2_chunk_sparse_border;
    // std::size_t l2_max_sparse_chunks;
    // std::size_t l2_sparse_index_bits;

    explicit Sizes(std::size_t n)
    {
      bits = n;

      l1_occurrences_per_chunk = Log2Pow2(n);
      l1_chunks_element_bits = Log2(n) + 1;
      l1_chunk_sparse_border = Log2Pow4(n);
      l1_chunk_sparse_lookup_entry_bits = BitWidth(n);
      l1_max_sparse_chunks = CeilDiv(n, l1_chunk_sparse_border);
      l1_sparse_index_bits = BitWidth(l1_max_sparse_chunks);

      // l2_occurrences_per_chunk = Log2Pow2(n);
      // l2_chunks_element_bits = Log2(n) + 1;
      // l2_chunk_sparse_border = Log2Pow4(n);
      // l2_chunk_sparse_lookup_entry_bits = BitWidth(n);
      // l2_max_sparse_chunks = CeilDiv(n, l2_chunk_sparse_border);
      // l2_sparse_index_bits = BitWidth(l2_max_sparse_chunks);
    }
  } sizes;

private:
  std::shared_ptr<CBitVector> bit_vector;

  struct L
  {
    BitsVector chunks;
    std::shared_ptr<BitVector> chunk_sparse;
    std::shared_ptr<ConstRank> chunk_sparse_access;
    BitsVector chunk_sparse_lookup;

    explicit L(std::size_t chunk_offset_bits, std::size_t lookup_bits)
      : chunks(chunk_offset_bits)
      , chunk_sparse(std::make_shared<BitVector>())
      , chunk_sparse_access()
      , chunk_sparse_lookup(lookup_bits)
    {
    }
  };

  L l1[2];
  // L l2[2];

public:
  explicit BetterSelect(std::shared_ptr<CBitVector> bit_vector)
    : bit_vector(std::move(bit_vector))
    , sizes(bit_vector->size())
    , l1{ L(sizes.l1_chunks_element_bits, sizes.l1_chunk_sparse_lookup_entry_bits), L(sizes.l1_chunks_element_bits, sizes.l1_chunk_sparse_lookup_entry_bits) }
  //, l2{ L(sizes.l1_chunks_element_bits, sizes.l1_chunk_sparse_lookup_entry_bits), L(sizes.l1_chunks_element_bits, sizes.l1_chunk_sparse_lookup_entry_bits) }
  {
  }

public:
  [[maybe_unused]] [[nodiscard]] std::size_t bits() const
  {
    return l1[0].chunks.bits() + l1[0].chunk_sparse->bits() + l1[0].chunk_sparse_access->bits() + l1[0].chunk_sparse_lookup.bits() + //
           l1[1].chunks.bits() + l1[1].chunk_sparse->bits() + l1[1].chunk_sparse_access->bits() + l1[1].chunk_sparse_lookup.bits();
  }

  void build()
  {
    const auto n = bit_vector->size();

    struct L_
    {
      std::size_t size = 0;
      std::size_t count = 0;
      BitsVector chunk_lookup;
      explicit L_(Sizes sizes)
        : chunk_lookup(sizes.l1_chunk_sparse_lookup_entry_bits)
      {
      }
    } l_[2]{ L_(sizes), L_(sizes) };

    // first chunk has no offset
    l1[0].chunks.push(0);
    l1[1].chunks.push(0);

    for (std::size_t i = 0; i < n; ++i) {
      ++l_[0].size;
      ++l_[1].size;

      const auto b = bit_vector->access(i);

      ++l_[b].count;
      l_[b].chunk_lookup.push(i);

      if (!(l_[b].count % sizes.l1_occurrences_per_chunk)) { // end of current chunk
        l1[b].chunks.push(i + 1);

        if (l_[b].size < sizes.l1_chunk_sparse_border) { // dense
          l1[b].chunk_sparse->push(false);
        } else { // sparse
          l1[b].chunk_sparse->push(true);
          // add lookup
          auto m = l_[b].chunk_lookup.size();
          for (std::size_t j = 0; j < m; ++j) {
            const auto index = l_[b].chunk_lookup[j].get();
            l1[b].chunk_sparse_lookup.push(index);
          }
        }

        l_[b].size = 0;
        l_[b].count = 0;
        l_[b].chunk_lookup = BitsVector(sizes.l1_chunk_sparse_lookup_entry_bits);
      }
    }

    for (int b = 0; b < 2; ++b) {
      if (l_[b].size) {
        if (l_[b].size < sizes.l1_chunk_sparse_border) { // dense
          l1[b].chunk_sparse->push(false);
        } else { // sparse
          l1[b].chunk_sparse->push(true);
          // add lookup
          auto m = l_[b].chunk_lookup.size();
          for (std::size_t j = 0; j < m; ++j) {
            const auto index = l_[b].chunk_lookup[j].get();
            l1[b].chunk_sparse_lookup.push(index);
          }
        }
      }

      l1[b].chunk_sparse_access = std::make_shared<ConstRank>(l1[b].chunk_sparse);
    }
  }

  template<std::size_t b>
  [[maybe_unused]] [[nodiscard]] std::size_t select(std::size_t index) const
  {
    const auto chunk_index = index / sizes.l1_occurrences_per_chunk;
    const auto chunk_rest = index % sizes.l1_occurrences_per_chunk;
    if (l1[b].chunk_sparse->at(chunk_index)) {
      const auto rank = l1[b].chunk_sparse_access->rank1(chunk_index);
      const auto result = l1[b].chunk_sparse_lookup[rank * sizes.l1_occurrences_per_chunk + chunk_rest - 1].get();
      return result;
    }

    const auto skipped_b = chunk_index * sizes.l1_occurrences_per_chunk;

    auto s = static_cast<std::size_t>(l1[b].chunks[chunk_index]);
    for (std::size_t b_sum = skipped_b; b_sum < index; ++s)
      b_sum += (1 - b) ^ static_cast<std::size_t>(bit_vector->access(s));

    return s - 1;
  }

  [[maybe_unused]] [[nodiscard]] std::size_t select0(std::size_t index) const
  {
    return select<0>(index);
  }

  [[maybe_unused]] [[nodiscard]] std::size_t select1(std::size_t index) const
  {
    return select<1>(index);
  }
};

static_assert(Select<BetterSelect>);

#endif
