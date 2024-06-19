#ifndef TRAITS_H
#define TRAITS_H

#include <cstdlib>
#include <memory>
#include <type_traits>

class CBit;
class CBits;
class BitVector;

template<typename T>
concept IsIntegral = std::is_integral_v<T> || std::is_base_of_v<CBit, T> || std::is_base_of_v<CBits, T>;

template<typename T>
concept IsUnsigned = std::is_unsigned_v<T> || std::is_base_of_v<CBit, T> || std::is_base_of_v<CBits, T>;

template<typename T>
concept IsSigned = !IsUnsigned<T>;

constexpr std::size_t
HandleIndex(std::size_t size, IsIntegral auto index)
{
  if constexpr (IsSigned<decltype(index)>)
    if (index < 0)
      return static_cast<std::size_t>(size + index);

  return static_cast<std::size_t>(index);
}

template<typename T>
  requires(std::is_pointer_v<T> && !std::is_pointer_v<std::remove_pointer_t<T>>)
inline auto
RemoveConst(T t)
{
  return const_cast<std::add_pointer_t<std::remove_cv_t<std::remove_pointer_t<T>>>>(t);
}

template<typename T>
concept Rank = requires(T t) {
  { t.build() } -> std::same_as<void>;
} && requires(const T t, const std::size_t i) {
  { t.bits() } -> std::same_as<std::size_t>;
  { t.rank0(i) } -> std::same_as<std::size_t>;
  { t.rank1(i) } -> std::same_as<std::size_t>;
} && std::is_constructible_v<T, std::shared_ptr<BitVector>>;

template<typename T>
concept Select = requires(T t) {
  { t.build() } -> std::same_as<void>;
} && requires(const T t, const std::size_t i) {
  { t.bits() } -> std::same_as<std::size_t>;
  { t.select0(i) } -> std::same_as<std::size_t>;
  { t.select1(i) } -> std::same_as<std::size_t>;
} && std::is_constructible_v<T, std::shared_ptr<BitVector>>;

#endif
