#ifndef BITS_VECTOR_H
#define BITS_VECTOR_H

#include "bitvector.h"
#include "util/sanity.h"

/** only for elements with less or equal to 64 bits */
class BitsVector
{
private:
  BitVector m_data;
  std::size_t m_element_bits;

public:
  BitsVector(std::size_t element_bits, std::size_t size)
    : m_data(size * element_bits)
    , m_element_bits(element_bits)
  {
    DEBUG_ASSERT(element_bits <= 64);
  }

  explicit BitsVector(std::size_t element_bits)
    : m_data()
    , m_element_bits(element_bits)
  {
    DEBUG_ASSERT(element_bits <= 64);
  }

  BitsVector(const BitsVector& bit_vector) = default;

  BitsVector(BitsVector&& bit_vector) noexcept
    : m_data(bit_vector.m_data)
    , m_element_bits(bit_vector.m_element_bits)
  {
  }

  BitsVector& operator=(BitsVector bits_vector) {
    m_element_bits = bits_vector.m_element_bits;
    m_data = std::move(bits_vector.m_data);
    return *this;
  }

public:
  void inline push(auto value)
  {
    auto v = ToU64(value);
    for (std::size_t i = 0; i < m_element_bits; ++i)
      m_data.push(CBit::GetBit(&v, i));
  }

  void resize(std::size_t size)
  {
    m_data.resize(size * m_element_bits);
  }

  void reserve(std::size_t size)
  {
    m_data.reserve(size * m_element_bits);
  }

public:
  [[nodiscard]] std::size_t size() const
  {
    return m_data.size() / m_element_bits;
  }

  [[nodiscard]] std::size_t bits() const
  {
    return m_data.bits();
  }

  [[nodiscard]] Bits at(IsIntegral auto i)
  {
    const auto index = HandleIndex(size(), i) * m_element_bits;
    return { &m_data.m_data[Div64(index)], Mod64(index), m_element_bits };
  }

  // clang-format off
  [[nodiscard]]Bits operator[](IsUnsigned auto i){return at(i);}
  [[nodiscard]]CBits operator[](IsUnsigned auto i)const{return{RemoveConst(this)->at(i)};}
  [[nodiscard]]CBits access(IsIntegral auto i)const{return{RemoveConst(this)->at(i)};}
  // clang-format on
};

#endif
