#ifndef LINEAR_SELECT_H
#define LINEAR_SELECT_H

#include "../bitvector.h"
#include "../traits.h"

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <utility>
#include <vector> // std::vector

class LinearSelect
{
private:
  std::shared_ptr<BitVector> m_bit_vector;

public:
  explicit LinearSelect(std::shared_ptr<BitVector> bit_vector)
    : m_bit_vector(std::move(bit_vector))
  {
  }

public:
  void build() {}

  [[maybe_unused]] [[nodiscard]] static std::size_t bits()
  {
    return 0;
  }

  [[maybe_unused]] [[nodiscard]] std::size_t select0(std::size_t index) const
  {
    if (!index)
      return 0;
    std::size_t s = 0;
    for (std::size_t b_sum = 0; b_sum < index; ++s)
      b_sum += (1 - static_cast<std::size_t>(m_bit_vector->access(s)));
    return s - 1;
  }

  [[maybe_unused]] [[nodiscard]] std::size_t select1(std::size_t index) const
  {
    if (!index)
      return 0;
    std::size_t s = 0;
    for (std::size_t b_sum = 0; b_sum < index; ++s)
      b_sum += static_cast<std::size_t>(m_bit_vector->access(s));
    return s - 1;
  }
};

static_assert(Select<LinearSelect>);

#endif
