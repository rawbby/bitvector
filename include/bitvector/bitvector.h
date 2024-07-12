#ifndef BIT_VECTOR_H
#define BIT_VECTOR_H

#include "bits.h"
#include "traits.h"
#include "util/math.h"

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>

class CBitVector
{
  friend class BitVector;
  friend class BitsVector;

protected:
  std::vector<std::uint64_t> m_data;
  std::uint64_t m_shift;

public:
  explicit CBitVector(std::size_t size)
    : m_data()
    , m_shift(Mod64(size))
  {
    m_data.resize(CeilDiv64(size));
  }

  CBitVector()
    : m_data()
    , m_shift()
  {
  }

  CBitVector(CBitVector&& bit_vector) noexcept = default;

  CBitVector(const CBitVector& bit_vector) = default;

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

  [[nodiscard]] CBits at(IsIntegral auto i) const
  {
    const auto index = HandleIndex(size(), i);
    return { &m_data[Div64(index)], Mod64(index), 1 };
  }

public:
  // clang-format off
  [[nodiscard]]CBits operator[](IsUnsigned auto i)const{return{RemoveConst(this)->at(i)};}
  [[nodiscard]]CBits access(IsIntegral auto i)const{return{RemoveConst(this)->at(i)};}
  // clang-format on
};

class BitVector : public CBitVector
{
public:
  explicit BitVector(std::size_t size)
    : CBitVector(size)
  {
  }

  BitVector()
    : CBitVector()
  {
  }

  BitVector(BitVector&& bit_vector) noexcept = default;

  BitVector(const BitVector& bit_vector) = default;

  explicit BitVector(CBitVector&& bit_vector)
  {
    m_shift = bit_vector.m_shift;
    m_data = std::move(bit_vector.m_data);
  }

  explicit BitVector(const CBitVector& bit_vector)
  {
    m_shift = bit_vector.m_shift;
    m_data = bit_vector.m_data;
  }

  BitVector& operator=(BitVector&& bit_vector) noexcept
  {
    m_shift = bit_vector.m_shift;
    m_data = std::move(bit_vector.m_data);
    return *this;
  }

  BitVector& operator=(const BitVector& bit_vector)
  {
    m_shift = bit_vector.m_shift;
    m_data = bit_vector.m_data;
    return *this;
  }

  BitVector& operator=(CBitVector&& bit_vector) noexcept
  {
    m_shift = bit_vector.m_shift;
    m_data = std::move(bit_vector.m_data);
    return *this;
  }

  BitVector& operator=(const CBitVector& bit_vector)
  {
    m_shift = bit_vector.m_shift;
    m_data = bit_vector.m_data;
    return *this;
  }

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
  [[nodiscard]] Bits at(IsIntegral auto i)
  {
    const auto index = HandleIndex(size(), i);
    return { &m_data[Div64(index)], Mod64(index), 1 };
  }

  [[nodiscard]] Bits operator[](IsUnsigned auto i)
  {
    return at(i);
  }

  [[nodiscard]] CBitVector&& finalize()
  {
    return std::move(*this);
  }
};

#endif
