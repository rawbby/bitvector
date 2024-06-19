#include <bitvector/bitvector.h>


#include <iostream>

int
main()
{
  auto x = ToU64(0);
  auto r = Bits{ &x, 2, 2 };

  x |= 0b1100;
  std::cout << r.get();

  if (r.get() != 3)
    std::exit(1);

  if (r.get() != CBits::GetBits(&x, 2, 2))
    std::exit(1);
}
