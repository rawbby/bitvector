#ifndef UTIL_H
#define UTIL_H

#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdlib>

bool
Between(auto a, auto l, auto u)
{
  return l <= a && a < u;
}

std::size_t
Min(std::size_t a, std::size_t b)
{
  return std::min(a, b);
}

std::size_t
Max(std::size_t a, std::size_t b)
{
  return std::max(a, b);
}

std::size_t
Sqrt(std::size_t n)
{
  return static_cast<std::size_t>(sqrt(static_cast<double>(n)));
}

std::size_t
Pow2(std::size_t n)
{
  return n * n;
}

std::size_t
Pow4(std::size_t n)
{
  return Pow2(Pow2(n));
}

std::size_t
BitWidth(std::size_t n)
{
  return std::bit_width(n);
}

std::size_t
Log2(std::size_t n)
{
  return BitWidth(n) - 1;
}

std::size_t
Log2Half(std::size_t n)
{
  return Log2(n) >> 1;
}

std::size_t
Log2Pow2(std::size_t n)
{
  return Pow2(Log2(n));
}

std::size_t
Log2Pow4(std::size_t n)
{
  return Pow4(Log2(n));
}

std::size_t
FloorX(std::size_t n, std::size_t x)
{
  if (n % x)
    return (n / x) * x;
  return n;
}

std::size_t
Floor64(std::size_t n)
{
  return n & ~0b111111;
}

std::size_t
Ceil(double n)
{
  return static_cast<std::size_t>(std::ceil(n));
}

std::size_t
CeilDiv(std::size_t a, std::size_t b)
{
  return Ceil(static_cast<double>(a) / static_cast<double>(b));
}

std::size_t
Div64(std::size_t a)
{
  return a >> 6;
}

std::size_t
Mod64(std::size_t a)
{
  return a & 0b111111;
}

std::size_t
Ceil64(std::size_t n)
{
  const auto m = Floor64(n);
  return Mod64(n) ? m + 64 : m;
}

std::size_t
CeilDiv64(std::size_t a)
{
  return Div64(Ceil64(a));
}

std::size_t
CeilX(std::size_t n, std::size_t x)
{
  if (n % x)
    return ((n / x) + 1) * x;
  return n;
}

auto
OneShift(std::size_t n)
{
  return static_cast<std::size_t>(1) << n;
}

auto
ToU64(auto n)
{
  return static_cast<std::uint64_t>(n);
}

std::size_t
NextPower2(std::size_t n)
{
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n |= n >> 32;
  n++;
  return n;
}

std::size_t
NearestPower2(std::size_t n)
{
  const auto next = NextPower2(n);
  const auto prev = next >> 1;

  if (next - n > n - prev)
    return prev;
  else
    return next;
}

#endif
