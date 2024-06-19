#include <bitvector/bitvector.h>

#include <iostream>

int
main()
{
  auto bits = BitVector();

  for (std::size_t i = 0; i < 80; ++i) {
    bits.push(i & 1);
    std::cout << bits[i];
  }

  for (std::size_t i = 0; i < 80; ++i)
    if (bits[i] != (i & 1))
      std::exit(1);
}
