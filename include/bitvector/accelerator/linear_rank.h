#ifndef LINEAR_RANK_H
#define LINEAR_RANK_H

#include "../bitvector.h"
#include "../traits.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector> // std::vector

class LinearRank
{
private:
  std::shared_ptr<CBitVector> m_bit_vector;

public:
  explicit LinearRank(std::shared_ptr<CBitVector> bit_vector)
    : m_bit_vector(std::move(bit_vector))
  {
  }

public:
  void build() {}

  [[maybe_unused]] [[nodiscard]] static std::size_t bits()
  {
    return 0;
  }

  [[maybe_unused]] [[nodiscard]] std::size_t rank0(std::size_t index) const
  {
    return index - rank1(index);
  }

  [[maybe_unused]] [[nodiscard]] std::size_t rank1(std::size_t index) const
  {
    std::size_t r = 0;
    for (std::size_t i = 0; i < index; ++i)
      r += m_bit_vector->access(i);
    return r;
  }
};

static_assert(Rank<LinearRank>);

#endif
