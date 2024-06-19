#include <bitvector/bitsvector.h>

#include <iostream>

int
main()
{
  auto bits = BitsVector{ 2, 20 };
  for (size_t i = 5; i < 15; ++i)
    bits[i] = i;
  for (size_t i = 0; i < 20; ++i)
    std::cout << bits[i];
  for (size_t i = 0; i < 5; ++i)
    if (bits[i] != 0)
      throw std::runtime_error("");
  for (size_t i = 5; i < 15; ++i)
    if (bits[i] != (i & 0b11))
      throw std::runtime_error("");
  for (size_t i = 15; i < 20; ++i)
    if (bits[i] != 0)
      throw std::runtime_error("");

  const auto& bits_ = bits;
  std::cout << '\n';
  for (size_t i = 0; i < 20; ++i)
    std::cout << bits_[i].get();
  for (size_t i = 0; i < 20; ++i)
    if (bits_[i] != bits[i].get())
      throw std::runtime_error("");
}
