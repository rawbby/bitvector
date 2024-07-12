#ifndef SANITY_H
#define SANITY_H

#include <csignal>
#include <cstdlib>
#include <iostream>

// clang-format off
#define MACRO_STRINGIFY_(x) #x
#define MACRO_STRINGIFY(x) MACRO_STRINGIFY_(x)
template<class Lambda,int=(Lambda{}(),0)>
constexpr inline bool IsConstexprLambda(Lambda){return true;}
constexpr inline bool IsConstexprLambda(...){return false;}
// clang-format on

// ensure DEBUG and NDEBUG (not DEBUG) is set properly ...
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
#undef NDEBUG
#define DEBUG
#else
#define NDEBUG
#undef DEBUG
#endif

inline void
AssertionExit(const char* msg)
{
  std::ios_base::sync_with_stdio(false);
  std::cout.tie(nullptr);
  puts(msg);
  std::raise(SIGTRAP);
  std::exit(1);
}

#define ASSERT(COND)                                                                                                                                                               \
  if (!(COND))                                                                                                                                                                     \
    AssertionExit("Assertion \"" #COND "\" failed at " __FILE__ ":" MACRO_STRINGIFY(__LINE__) "!");                                                                                \
  ((void)0)

#ifdef DEBUG
#define DEBUG_ASSERT(COND) ASSERT(COND)
#else
#define DEBUG_ASSERT(COND) ((void)0)
#endif

#endif
