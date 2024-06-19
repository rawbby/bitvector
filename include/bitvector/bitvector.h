#ifndef BIT_VECTOR_H
#define BIT_VECTOR_H

#include "bits.h"
#include "traits.h"
#include "util/math.h"

#include <cstdint> // std::uint64_t
#include <cstdlib> // std::size_t
#include <memory>  // std::vector
#include <utility>
#include <vector> // std::vector

class BitVector
{
protected:
  friend class BitsVector;
  friend class BitVectorView;
  std::vector<std::uint64_t> m_data;
  std::uint64_t m_shift;

public:
  explicit BitVector(std::size_t size)
    : m_data()
    , m_shift(Mod64(size))
  {
    m_data.resize(CeilDiv64(size));
  }

  BitVector()
    : m_data()
    , m_shift()
  {
  }

  BitVector(BitVector&& bit_vector) noexcept
    : m_data(std::move(bit_vector.m_data))
    , m_shift(bit_vector.m_shift)
  {
  }

  BitVector(const BitVector& bit_vector) = default;

  BitVector& operator=(const BitVector& bit_vector) = default;

  BitVector& operator=(BitVector&& bit_vector) = default;

public:
  void inline push(auto b)
  {
    if (!m_shift)
      m_data.emplace_back(0);

    m_data.back() |= ((ToU64(b) & 1) << m_shift);
    m_shift = (m_shift + 1) & 0b111111;
  }

  void resize(std::size_t size)
  {
    m_data.resize(CeilDiv64(size));
  }

  void reserve(std::size_t size)
  {
    m_data.reserve(CeilDiv64(size));
  }

public:
  [[nodiscard]] std::size_t size() const
  {
    if (m_data.empty())
      return m_shift;

    return ((m_data.size() - bool(m_shift)) << 6) + m_shift;
  }

  [[nodiscard]] std::size_t bits() const
  {
    return m_data.size() << 6;
  }

  [[nodiscard]] Bits at(IsIntegral auto i)
  {
    const auto index = HandleIndex(size(), i);
    return { &m_data[Div64(index)], Mod64(index), 1 };
  }

  // clang-format off
  [[nodiscard]]Bits operator[](IsUnsigned auto i){return at(i);}
  [[nodiscard]]CBits operator[](IsUnsigned auto i)const{return{RemoveConst(this)->at(i)};}
  [[nodiscard]]CBits access(IsIntegral auto i)const{return{RemoveConst(this)->at(i)};}
  // clang-format off
};

#endif
