#ifndef BIT_VECTOR_ACCESSOR_H
#define BIT_VECTOR_ACCESSOR_H

#include "../bitvector.h"
#include "../traits.h"
#include "../util/math.h"
#include "linear_rank.h"
#include "linear_select.h"

#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include <vector>

template<typename R = LinearRank, typename S = LinearSelect>
  requires(Rank<R> && Select<S>)
class BitVectorAccessor
{
private:
  std::shared_ptr<BitVector> m_bit_vector;
  R m_rank;
  S m_select;

public:
  explicit BitVectorAccessor(std::shared_ptr<BitVector> bit_vector)
    : m_bit_vector(bit_vector)
    , m_rank(bit_vector)
    , m_select(std::move(bit_vector))
  {
  }

public:
  // clang-format off
  [[nodiscard]]std::size_t size()const{return m_bit_vector->size();}
  [[nodiscard]]std::size_t bits()const{return m_rank.bits()+m_select.bits();}
  [[nodiscard]]bool operator[](std::size_t i)const{return m_bit_vector->access(i);}
  [[nodiscard]]bool at(std::size_t i)const{return m_bit_vector->access(i);}
  // clang-format on

public:
  void build()
  {
    m_rank.build();
    m_select.build();
  }

  // clang-format off
  [[maybe_unused]][[nodiscard]]std::size_t select0(std::size_t i)const{return m_select.select0(i);}
  [[maybe_unused]][[nodiscard]]std::size_t select1(std::size_t i)const{return m_select.select1(i);}
  [[maybe_unused]][[nodiscard]]std::size_t rank0(std::size_t i)const{return m_rank.rank0(i);}
  [[maybe_unused]][[nodiscard]]std::size_t rank1(std::size_t i)const{return m_rank.rank1(i);}
  // clang-format on
};

#endif
