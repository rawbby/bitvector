#ifndef PARSE_H
#define PARSE_H

#define parse_init(f)                                                                                                                                                              \
  char parse_c;                                                                                                                                                                    \
  auto* parse_f = &f;                                                                                                                                                              \
  parse_next()

#define parse_next()                                                                                                                                                               \
  [&parse_f, &parse_c]() {                                                                                                                                                         \
    parse_f->get(parse_c);                                                                                                                                                         \
    return parse_c;                                                                                                                                                                \
  }()

#define parse_skip()                                                                                                                                                               \
  parse_next();                                                                                                                                                                    \
  ((void)0)

#define parse_skip_n(n)                                                                                                                                                            \
  parse_f->ignore(n - 1);                                                                                                                                                          \
  parse_skip()

#define parse_skip_str(str) parse_skip_n(sizeof(str) - 1)

#define parse_get() (parse_c)

#define parse_size(v)                                                                                                                                                              \
  std::size_t v = 0;                                                                                                                                                               \
  do {                                                                                                                                                                             \
    v = v * static_cast<std::size_t>(10) + static_cast<std::size_t>(parse_get() - '0');                                                                                            \
  } while (parse_next() != '\n');                                                                                                                                                  \
  parse_next()

#define parse_get_bool(b)                                                                                                                                                          \
  bool b = (parse_get() - '0');                                                                                                                                                    \
  ((void)0)

#define parse_bool(b)                                                                                                                                                              \
  bool b = (parse_get() - '0');                                                                                                                                                    \
  parse_next()

#endif
