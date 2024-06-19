#ifndef BIT_H
#define BIT_H

#include "util/math.h"

#include <cstdint>
#include <cstdlib>
#include <memory>

class CBit
{
protected:
  const std::uint64_t* m_data;
  std::size_t m_offset;

public:
  CBit(const std::uint64_t* data, std::size_t offset)
    : m_data(data)
    , m_offset(offset)
  {
  }

public:
  static bool GetBit(const std::uint64_t* values, std::size_t i)
  {
    return (values[Div64(i)] >> Mod64(i)) & 1;
  }

public:
  [[nodiscard]] bool get() const
  {
    return GetBit(m_data, m_offset);
  }

  operator bool() const // NOLINT(*-explicit-constructor)
  {
    return get();
  }

  // clang-format off
  bool operator!()const{return!get();}
  bool operator~()const{return~get();}
  bool operator+()const{return+get();}
  bool operator-()const{return-get();}
  // clang-format on
};

class Bit : public CBit
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
  Bit(std::uint64_t* data, std::size_t offset)
    : CBit(data, offset)
  {
  }

public:
  static void SetBit(std::uint64_t* values, std::size_t i)
  {
    values[Div64(i)] |= OneShift(Mod64(i));
  }

  static void UnsetBit(std::uint64_t* values, std::size_t i)
  {
    values[Div64(i)] &= ~OneShift(Mod64(i));
  }

  static void SetBit(std::uint64_t* values, std::size_t i, auto b)
  {
    const auto index = Div64(i);
    const auto shift = Mod64(i);
    values[index] &= ~(static_cast<std::uint64_t>(1) << shift);                 // unset
    values[index] |= static_cast<std::uint64_t>(static_cast<bool>(b)) << shift; // set if bit
  }

public:
  void set(auto value)
  {
    SetBit(data(), m_offset, static_cast<bool>(value));
  }

  // clang-format off
  Bit&operator=(auto val){set(val);return *this;}
  Bit&operator&=(auto val){set(get()&static_cast<bool>(val));return *this;}
  Bit&operator|=(auto val){set(get()|static_cast<bool>(val));return *this;}
  Bit&operator^=(auto val){set(get()^static_cast<bool>(val));return *this;}
  // clang-format on
};

// clang-format off
template<typename... Ts>concept ContainsBit=(...||std::is_base_of_v<CBit,Ts>);
template<typename L,typename R>requires ContainsBit<L,R>inline bool operator==(L lhs,R rhs){return static_cast<bool>(lhs)==static_cast<bool>(rhs);}
template<typename L,typename R>requires ContainsBit<L,R>inline bool operator!=(L lhs,R rhs){return static_cast<bool>(lhs)!=static_cast<bool>(rhs);}
template<typename L,typename R>requires ContainsBit<L,R>inline bool operator&&(L lhs,R rhs){return static_cast<bool>(lhs)&&static_cast<bool>(rhs);}
template<typename L,typename R>requires ContainsBit<L,R>inline bool operator||(L lhs,R rhs){return static_cast<bool>(lhs)||static_cast<bool>(rhs);}
template<typename L,typename R>requires ContainsBit<L,R>inline bool operator&(L lhs,R rhs){return static_cast<bool>(lhs)&static_cast<bool>(rhs);}
template<typename L,typename R>requires ContainsBit<L,R>inline bool operator|(L lhs,R rhs){return static_cast<bool>(lhs)|static_cast<bool>(rhs);}
template<typename L,typename R>requires ContainsBit<L,R>inline bool operator^(L lhs,R rhs){return static_cast<bool>(lhs)^static_cast<bool>(rhs);}
// clang-format on

#endif
