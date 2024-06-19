#include <bitvector/bitvector_view.h>


#include <iostream>

int
main()
{
  auto bits = BitVector();

  for (std::size_t i = 0; i < 62; ++i)
    bits.push(0);
  for (std::size_t i = 62; i < 66; ++i)
    bits.push(1);
  for (std::size_t i = 66; i < 126; ++i)
    bits.push(0);
  for (std::size_t i = 126; i < 130; ++i)
    bits.push(1);
  for (std::size_t i = 130; i < 192; ++i)
    bits.push(0);

  const auto view = BitVectorView(std::move(bits), 0, bits.size());

  for (std::size_t i = 0; i < 62; ++i)
    if (view[i] != 0)
      std::exit(1);
  for (std::size_t i = 62; i < 66; ++i)
    if (view[i] != 1)
      std::exit(1);
  for (std::size_t i = 66; i < 126; ++i)
    if (view[i] != 0)
      std::exit(1);
  for (std::size_t i = 126; i < 130; ++i)
    if (view[i] != 1)
      std::exit(1);
  for (std::size_t i = 130; i < 192; ++i)
    if (view[i] != 0)
      std::exit(1);

  auto subview_l = BitVectorView(view, 62, 4);
  auto subview_ll = BitVectorView(subview_l, 0, 2);
  auto subview_lu = BitVectorView(subview_l, 2, 2);
  auto subview_u = BitVectorView(view, 126, 4);

  if (subview_l.pop_count() != 4)
    std::exit(1);
  if (subview_u.pop_count() != 4)
    std::exit(1);
  if (subview_ll.pop_count() != 2)
    std::exit(1);
  if (subview_lu.pop_count() != 2)
    std::exit(1);
  if (view.pop_count() != 8)
    std::exit(1);
}
