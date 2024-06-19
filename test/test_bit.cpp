#include <bitvector/bitvector.h>

#include <iostream>

int
main()
{
  auto x = ToU64(0);

  Bit::SetBit(&x, 2);
  std::cout << CBit::GetBit(&x, 2);
  if (!CBit::GetBit(&x, 2))
    std::exit(1);

  Bits::SetBits(&x, 3, 2, 0b11);
  std::cout << CBit::GetBit(&x, 3);
  std::cout << CBit::GetBit(&x, 4);
  if (!CBit::GetBit(&x, 3))
    std::exit(1);
  if (!CBit::GetBit(&x, 4))
    std::exit(1);

  std::cout << CBits::GetBits(&x, 2, 3);
  if (CBits::GetBits(&x, 2, 3) != 7)
    std::exit(1);
}
