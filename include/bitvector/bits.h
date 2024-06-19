#ifndef BITS_H
#define BITS_H

#include "bit.h"
#include "traits.h"
#include "util/math.h"

#include <cstdint>
#include <cstdlib>
#include <memory>

class CBits
{
protected:
  const std::uint64_t* m_data;
  std::size_t m_offset;
  std::size_t m_size;

public:
  CBits(const std::uint64_t* data, std::size_t offset, std::size_t size)
    : m_data(data)
    , m_offset(offset)
    , m_size(size)
  {
  }

public:
  static std::uint64_t GetBits(const std::uint64_t* values, std::size_t i, std::size_t size)
  {
    std::uint64_t bits = 0;
    for (std::size_t j = 0; j < size; ++j)
      Bit::SetBit(&bits, j, CBit::GetBit(values, i + j));
    return bits;
  }

public:
  [[nodiscard]] std::uint64_t get() const
  {
    return GetBits(m_data, m_offset, m_size);
  }

  CBit at(IsIntegral auto i) const
  {
    return { m_data, m_offset + HandleIndex(m_size, i) };
  }

  operator std::uint64_t() const // NOLINT(*-explicit-constructor)
  {
    return get();
  }

  // clang-format off
  CBit operator[](IsUnsigned auto i)const{return at(i);}
  std::uint64_t operator!()const{return!get();}
  std::uint64_t operator~()const{return~get();}
  std::uint64_t operator+()const{return+get();}
  std::uint64_t operator-()const{return-get();}
  std::uint64_t operator>>(auto v)const{return get()>>ToU64(v);}
  std::uint64_t operator<<(auto v)const{return get()<<ToU64(v);}
  // clang-format on
};

class Bits : public CBits
{
private:
  std::uint64_t* data()
  {
    // even tho m_data is stored by CBit
    // to Bits it was passed non-const, and we
    // are allowed to access it.
    return const_cast<std::uint64_t*>(m_data);
  }

public:
  Bits(std::uint64_t* data, std::size_t offset, std::size_t size)
    : CBits(data, offset, size)
  {
  }

public:
  static void SetBits(std::uint64_t* values, std::size_t i, std::size_t size, auto bits)
  {
    const auto b = ToU64(bits);
    for (std::size_t j = 0; j < size; ++j)
      Bit::SetBit(values, i + j, CBit::GetBit(&b, j));
  }

public:
  void set(auto value)
  {
    SetBits(data(), m_offset, m_size, ToU64(value));
  }

  Bit at(IsIntegral auto i)
  {
    return { data(), m_offset + HandleIndex(m_size, i) };
  }

  // clang-format off
  Bit operator[](IsUnsigned auto i){return access(i);}
  Bits&operator=(auto v){set(ToU64(v));return*this;}
  Bits&operator+=(auto v){set(get()+ToU64(v));return*this;}
  Bits&operator-=(auto v){set(get()-ToU64(v));return*this;}
  Bits&operator*=(auto v){set(get()*ToU64(v));return*this;}
  Bits&operator/=(auto v){set(get()/ToU64(v));return*this;}
  Bits&operator%=(auto v){set(get()%ToU64(v));return*this;}
  Bits&operator&=(auto v){set(get()&ToU64(v));return*this;}
  Bits&operator|=(auto v){set(get()|ToU64(v));return*this;}
  Bits&operator^=(auto v){set(get()^ToU64(v));return*this;}
  Bits&operator<<=(auto v){set(get()<<ToU64(v));return*this;}
  Bits&operator>>=(auto v){set(get()>>ToU64(v));return*this;}
  Bits&operator++(){set(get()+ToU64(1));return*this;}
  Bits&operator--(){set(get()-ToU64(1));return*this;}
  // clang-format on
};

// clang-format off
template<typename...Ts>concept ContainsBits=(...||std::is_base_of_v<CBits,Ts>);
template<typename L,typename R>requires ContainsBits<L,R>inline bool operator==(L l,R r){return static_cast<bool>(l)==static_cast<bool>(r);}
template<typename L,typename R>requires ContainsBits<L,R>inline bool operator!=(L l,R r){return static_cast<bool>(l)!=static_cast<bool>(r);}
template<typename L,typename R>requires ContainsBits<L,R>inline bool operator&&(L l,R r){return static_cast<bool>(l)&&static_cast<bool>(r);}
template<typename L,typename R>requires ContainsBits<L,R>inline bool operator||(L l,R r){return static_cast<bool>(l)||static_cast<bool>(r);}
template<typename L,typename R>requires ContainsBits<L,R>inline std::uint64_t operator+(L l,R r){return ToU64(l)+ToU64(r);}
template<typename L,typename R>requires ContainsBits<L,R>inline std::uint64_t operator-(L l,R r){return ToU64(l)-ToU64(r);}
template<typename L,typename R>requires ContainsBits<L,R>inline std::uint64_t operator*(L l,R r){return ToU64(l)*ToU64(r);}
template<typename L,typename R>requires ContainsBits<L,R>inline std::uint64_t operator/(L l,R r){return ToU64(l)/ToU64(r);}
template<typename L,typename R>requires ContainsBits<L,R>inline std::uint64_t operator%(L l,R r){return ToU64(l)%ToU64(r);}
template<typename L,typename R>requires ContainsBits<L,R>inline std::uint64_t operator&(L l,R r){return ToU64(l)&ToU64(r);}
template<typename L,typename R>requires ContainsBits<L,R>inline std::uint64_t operator|(L l,R r){return ToU64(l)|ToU64(r);}
template<typename L,typename R>requires ContainsBits<L,R>inline std::uint64_t operator^(L l,R r){return ToU64(l)^ToU64(r);}
// clang-format on

#endif
