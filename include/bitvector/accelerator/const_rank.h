#ifndef CONST_RANK_H
#define CONST_RANK_H

#include "../bitsvector.h"
#include "../bitvector.h"
#include "../bitvector_view.h"
#include "../traits.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

class ConstRank
{
private:
  struct Sizes
  {
    std::size_t bits;
    std::size_t s_block_bits;
    std::size_t s_block_count;
    std::size_t s_block_elem_size;
    std::size_t block_bits;
    std::size_t block_count_per_sb;
    std::size_t block_count_total;
    std::size_t block_elem_size;

    explicit Sizes(std::size_t n)
    {
      bits = n;

      // by definition
      block_bits = Max(1, Log2Half(n));

      s_block_bits = Log2Pow2(n);                        // theoretical optimum
      s_block_bits = NearestPower2(s_block_bits) - 1;    // round to nearest power of two (for optimal bit_with see *1)
      s_block_bits = Max(block_bits << 1, s_block_bits); // ensure at least two blocks per super block
      s_block_bits = FloorX(s_block_bits, block_bits);   // ensure a multiple of block bits

      // by calculation
      block_count_per_sb = s_block_bits / block_bits; // exact as s_block_bits is a multiple of block_bits
      s_block_count = CeilDiv(n, s_block_bits);       // calculate the super blocks needed
      block_count_total = CeilDiv(n, block_bits);     // calculate the blocks needed

      block_elem_size = BitWidth(s_block_bits); // *1 calculate the bits needed for total offset
      s_block_elem_size = BitWidth(n);          // calculate the bits needed for relative offset
    }
  } sizes;
  std::shared_ptr<BitVector> bit_vector;
  BitsVector s_blocks;
  BitsVector blocks;

public:
  explicit ConstRank(std::shared_ptr<BitVector> bit_vector)
    : bit_vector(std::move(bit_vector))
    , sizes(bit_vector->size())
    , s_blocks(sizes.s_block_elem_size)
    , blocks(sizes.block_elem_size)
  {
    s_blocks.reserve(sizes.s_block_count);
    blocks.reserve(sizes.block_count_total);
  }

public:
  [[maybe_unused]] [[nodiscard]] std::size_t bits() const
  {
    return s_blocks.bits() + blocks.bits();
  }

  void build()
  {
    const auto n = sizes.bits;

    std::uint64_t c = 0;

    for (std::size_t i = 0; i < n; ++i) {
      if (!bit_vector->access(i))
        ++c;

      if (!((i + 1) % sizes.block_bits)) // reached end of block
        blocks.push(c);

      if (!((i + 1) % sizes.s_block_bits)) { // reached end of super block
        s_blocks.push(c);
        if (s_blocks.size() > 1) // if not first block add c from previous block
          s_blocks.at(-1) += s_blocks.at(-2);
        c = 0;
      }
    }
  }

  [[maybe_unused]] [[nodiscard]] std::size_t rank0(std::size_t index) const
  {
    if (!index)
      return 0;

    const auto si = index / sizes.s_block_bits;
    const auto bi_tot = index / sizes.block_bits;
    const auto bi_rel = bi_tot % sizes.block_count_per_sb;
    const auto bo = index % sizes.block_bits;

    const auto sc = si ? s_blocks[si - 1].get() : ToU64(0ull);
    const auto bc = bi_rel ? blocks[bi_tot - 1].get() : ToU64(0ull);

    auto oc = bo;

    for (std::size_t i = index - bo; i < index; ++i)
      oc -= bit_vector->access(i);

    return sc + bc + oc;
  }

  [[maybe_unused]] [[nodiscard]] std::size_t rank1(std::size_t index) const
  {
    return index - rank0(index);
  }
};

static_assert(Rank<ConstRank>);

#endif
