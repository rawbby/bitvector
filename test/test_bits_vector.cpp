#include <bitvector/bitsvector.h>

#include <iostream>

int
main()
{
  auto bits = BitsVector(2, 8);

  for (size_t i = 0; i < 8; ++i) {
    std::cout << bits[i].get();
  }
  for (size_t i = 8; i < 72; ++i) {
    bits.push(i); // = i & 0b11
    auto v = bits[i - 2];
    std::cout << v.get();
  }
  for (size_t i = 0; i < 8; ++i) {
    bits.push(0);
    std::cout << bits[72 + i].get();
  }

  for (size_t i = 0; i < 8; ++i)
    if (bits[i].get() != 0)
      std::exit(1);
  for (size_t i = 8; i < 72; ++i)
    if (bits[i].get() != (i & 0b11))
      std::exit(1);
  for (size_t i = 72; i < 80; ++i)
    if (bits[i].get() != 0)
      std::exit(1);
}
