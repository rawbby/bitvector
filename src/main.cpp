#include <bitvector/accelerator/accessor.h>
#include <bitvector/accelerator/better_select.h>
#include <bitvector/accelerator/const_rank.h>
#include <bitvector/accelerator/linear_rank.h>
#include <bitvector/accelerator/linear_select.h>
#include <bitvector/bitvector.h>
#include <bitvector/util/sanity.h>

#include <cassert>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "parse.h"

#ifdef INCLUDE_PASTA_BIT_VECTOR
#include "pasta_bit_vector.h"
#endif

template<typename Accessor>
struct Arguments
{
  std::size_t n{};
  std::shared_ptr<BitVector> bit_vector{};
  std::shared_ptr<Accessor> accessor{};
  std::vector<std::function<std::uint64_t()>> operations{};

  [[nodiscard]] inline auto prepare_results() const
  {
    auto results = std::vector<std::uint64_t>{};
    results.resize(operations.size());
    return results;
  }
};

template<typename Accessor>
Arguments<Accessor>
parse(std::ifstream& f)
{
  parse_init(f);
  parse_size(n); // read first line

  auto bit_vector = std::make_shared<BitVector>();
  do { // read second line
    parse_get_bool(b);
    bit_vector->push(b);
  } while (parse_next() != '\n');
  parse_next(); // skip new-line character

  auto accessor = std::make_shared<Accessor>(bit_vector);
  auto accessor_raw = accessor.get();
  auto operations = std::vector<std::function<std::uint64_t()>>{};
  operations.reserve(n);

  // parse the operations and bind parameter
  for (std::size_t i = 0; i < n; ++i) {
    switch (parse_get()) {
      case 'a': {
        parse_skip_str("access ");
        parse_size(index);
        operations.emplace_back([accessor_raw, index]() { return accessor_raw->at(index); });
        break;
      }
      case 'r': {
        parse_skip_str("rank ");
        parse_bool(b);
        parse_skip();
        parse_size(index);
        if (b)
          operations.emplace_back([accessor_raw, index]() { return accessor_raw->rank1(index); });
        else
          operations.emplace_back([accessor_raw, index]() { return accessor_raw->rank0(index); });
        break;
      }
      case 's': {
        parse_skip_str("select ");
        parse_bool(b);
        parse_skip();
        parse_size(index);
        if (b)
          operations.emplace_back([accessor_raw, index]() { return accessor_raw->select1(index); });
        else
          operations.emplace_back([accessor_raw, index]() { return accessor_raw->select0(index); });
        break;
      }
      default:
        AssertionExit("invalid operation!");
    }
  }

  return { n, std::move(bit_vector), std::move(accessor), operations };
}

#define BEG_BENCH(v) const auto beg_##v = std::chrono::high_resolution_clock::now()
#define END_BENCH(v) const auto time_##v = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - beg_##v).count()

template<typename Accessor>
void
bench(std::ifstream& in, std::ofstream& out)
{
  BEG_BENCH(parse);
  auto args = parse<Accessor>(in);
  auto results = args.prepare_results();
  END_BENCH(parse);

  BEG_BENCH(acceleration);
  args.accessor->build();
  END_BENCH(acceleration);

  BEG_BENCH(operations);
  for (std::size_t i = 0; i < args.n; ++i)
    results[i] = args.operations[i]();
  END_BENCH(operations);

  for (const auto result : results)
    out << result << '\n';

  const auto time = time_acceleration + time_operations;
  const auto space = args.bit_vector->size() + args.accessor->bits();
  std::cout << "RESULT name=robert_fritsch time=" << time << " space=" << space << '\n';
}

int
main(int argc, char** argv)
{
  std::ifstream in(argv[1]);
  if (!in.is_open())
    exit(1);

  std::ofstream out(argv[2]);
  if (!out.is_open())
    exit(1);

  if (argc > 3) {
    const auto x = std::string{ argv[3] };

    if (x == "linear") {
      bench<BitVectorAccessor<LinearRank, LinearSelect>>(in, out);
    }
    if (x == "const") {
      bench<BitVectorAccessor<ConstRank, BetterSelect>>(in, out);
    }
#ifdef INCLUDE_PASTA_BIT_VECTOR
    if (x == "pasta") {
      bench<PastaBitVectorAccessor>(in, out);
    }
#endif
  } else {
    bench<BitVectorAccessor<ConstRank, BetterSelect>>(in, out);
  }
}
