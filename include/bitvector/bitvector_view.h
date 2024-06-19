#ifndef BIT_VECTOR_VIEW_H
#define BIT_VECTOR_VIEW_H

#include "bitvector.h"
#include "traits.h"

class BitVectorView
{
protected:
  std::shared_ptr<BitVector> m_data;
  std::size_t m_offset;
  std::size_t m_size;

public:
  BitVectorView(std::shared_ptr<BitVector> bit_vector, std::size_t offset, std::size_t size)
    : m_data(std::move(bit_vector))
    , m_offset(offset)
    , m_size(size)
  {
  }

  BitVectorView(BitVector&& bit_vector, std::size_t offset, std::size_t size)
    : m_data(std::make_shared<BitVector>(bit_vector))
    , m_offset(offset)
    , m_size(size)
  {
  }

  explicit BitVectorView(BitVector&& bit_vector)
    : m_data(std::make_shared<BitVector>(bit_vector))
    , m_offset(0)
    , m_size(m_data->size())
  {
  }

  BitVectorView(BitVectorView&& bit_vector_view, std::size_t offset, std::size_t size)
    : m_data(std::move(bit_vector_view.m_data))
    , m_offset(bit_vector_view.m_offset + offset)
    , m_size(size)
  {
  }

  BitVectorView(const BitVectorView& bit_vector_view, std::size_t offset, std::size_t size)
    : m_data(bit_vector_view.m_data)
    , m_offset(bit_vector_view.m_offset + offset)
    , m_size(size)
  {
  }

  BitVectorView(BitVectorView&& bit_vector_view) noexcept
    : m_data(std::move(bit_vector_view.m_data))
    , m_offset(bit_vector_view.m_offset)
    , m_size(bit_vector_view.m_size)
  {
  }

  BitVectorView(const BitVectorView&) = default;

  BitVectorView& operator=(const BitVectorView&) = default;

  BitVectorView& operator=(BitVectorView&&) = default;

public:
  [[nodiscard]] std::size_t size() const
  {
    return m_size;
  }

  [[nodiscard]] std::size_t pop_count() const
  {
    std::size_t bits = 0;

    for (std::size_t i = 0; i < Mod64(64 - Mod64(m_offset)); ++i)
      bits += access(i).get();

    for (std::size_t i = Div64(Ceil64(m_offset)); i < Div64(m_offset + m_size); ++i)
      bits += std::popcount(m_data->m_data[i]);

    for (std::size_t i = Floor64(m_offset + m_size); i < m_offset + m_size; ++i)
      bits += access(i - m_offset).get();

    return bits;
  }

  [[nodiscard]] Bits access(IsIntegral auto i)
  {
    return m_data->at(m_offset + HandleIndex(size(), i));
  }

  // clang-format off
  [[nodiscard]]Bits operator[](IsUnsigned auto i){return access(i);}
  [[nodiscard]]CBits operator[](IsUnsigned auto i)const{return{RemoveConst(this)->access(i)};}
  [[nodiscard]]CBits access(IsIntegral auto i)const{return{RemoveConst(this)->access(i)};}
  // clang-format on
};

#endif
