#ifndef PASTA_BIT_VECTOR_H
#define PASTA_BIT_VECTOR_H

#include <bitvector/bitvector.h>

#include <pasta/bit_vector/bit_vector.hpp>
#include <pasta/bit_vector/support/rank_select.hpp>

#include <memory>

class PastaBitVectorAccessor
{
private:
  std::shared_ptr<BitVector> m_original_bit_vector;
  std::shared_ptr<pasta::BitVector> m_bit_vector;
  std::shared_ptr<pasta::RankSelect<>> m_accessor;

public:
  explicit PastaBitVectorAccessor(std::shared_ptr<BitVector> bit_vector)
    : m_original_bit_vector(std::move(bit_vector))
    , m_bit_vector(std::make_shared<pasta::BitVector>())
    , m_accessor()
  {
    m_bit_vector->resize(m_original_bit_vector->size());
    for (std::size_t i = 0; i < m_original_bit_vector->size(); ++i)
      (*m_bit_vector)[i] = (*m_original_bit_vector)[i];
  }

public:
  // clang-format off
  [[nodiscard]]std::size_t size()const{return m_bit_vector->size();}
  [[nodiscard]]std::size_t bits()const{return m_accessor->space_usage() << 3;}
  [[nodiscard]]bool operator[](std::size_t i)const{return (*m_bit_vector)[i];}
  [[nodiscard]]bool at(std::size_t i)const{return (*m_bit_vector)[i];}
  // clang-format on

public:
  void build()
  {
    m_accessor = std::make_shared<pasta::RankSelect<>>(*m_bit_vector);
  }

  // clang-format off
  [[maybe_unused]][[nodiscard]]std::size_t select0(std::size_t i)const{return m_accessor->select0(i);}
  [[maybe_unused]][[nodiscard]]std::size_t select1(std::size_t i)const{return m_accessor->select1(i);}
  [[maybe_unused]][[nodiscard]]std::size_t rank0(std::size_t i)const{return m_accessor->rank0(i);}
  [[maybe_unused]][[nodiscard]]std::size_t rank1(std::size_t i)const{return m_accessor->rank1(i);}
  // clang-format on
};

#endif
