/*          Copyright Danila Kutenin, 2023-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <string>

struct StrictWeakOrderingSettings {
  bool prior_sort = false;
  bool print_possible_reason = false;
  // Not yet supported.
  bool print_elements = false;
};

struct StrictWeakOrderingResult {
  bool ok = true;
  std::string possible_reason;
};

namespace internal {

inline StrictWeakOrderingResult OkResult() {
  return StrictWeakOrderingResult{};
}

inline StrictWeakOrderingResult CreateSimpleFailedResult() {
  StrictWeakOrderingResult result;
  result.ok = false;
  return result;
}

template <class Index>
inline StrictWeakOrderingResult CreateFailedResult(const std::string& reason,
                                                   Index i, Index j) {
  StrictWeakOrderingResult result;
  result.ok = false;
  result.possible_reason = reason + ": comp(first[" + std::to_string(j) +
                           "], first[" + std::to_string(i) + "]) is true;";
  return result;
}

template <class Iter, class Compare>
inline StrictWeakOrderingResult strict_weak_ordering_check_loop(
    Iter first, Iter last, const StrictWeakOrderingSettings& settings,
    Compare comp) {
  if (settings.prior_sort) {
    // Should not fail is comp is not strict weak ordering. But if comp crashes,
    // nothing is going to help.
    // Strictly speaking, this is UB but all implementations in gcc/clang/msvc
    // are fine.
    std::make_heap(first, last, comp);
    std::sort_heap(first, last, comp);
  }
  StrictWeakOrderingResult result;
  if (first != last) {
    Iter copy_first = first;
    Iter next = copy_first;
    while (++next != last) {
      if (comp(*next, *copy_first)) {
        if (settings.print_possible_reason) {
          return CreateFailedResult("Unsorted array ", copy_first - first,
                                    next - first);
        } else {
          return CreateSimpleFailedResult();
        }
      }
      copy_first = next;
    }
  }
  typedef typename std::iterator_traits<Iter>::difference_type diff_type;
  diff_type size = last - first;
  diff_type P = 0;
  while (P < size) {
    diff_type Q = P + 1;
    while (Q < size && !comp(first[P], first[Q])) {
      ++Q;
    }
    for (diff_type B = P; B < Q; ++B) {
      for (diff_type A = P; A <= B; ++A) {
        if (comp(first[A], first[B]) || comp(first[B], first[A])) {
          return CreateSimpleFailedResult();
        }
      }
    }
    for (diff_type A = P; A < Q; ++A) {
      for (diff_type B = Q; B < size; ++B) {
        if (!comp(first[A], first[B]) || comp(first[B], first[A])) {
          return CreateSimpleFailedResult();
        }
      }
    }
    P = Q;
  }
  return OkResult();
}

}  // namespace internal

template <class Iter, class Compare>
inline StrictWeakOrderingResult strict_weak_ordering_check(
    Iter first, Iter last, const StrictWeakOrderingSettings& settings,
    Compare comp) {
  return internal::strict_weak_ordering_check_loop(first, last, settings, comp);
}

template <class Iter>
inline StrictWeakOrderingResult strict_weak_ordering_check(
    Iter first, Iter last, const StrictWeakOrderingSettings& settings) {
  return strict_weak_ordering_check(
      first, last, settings,
      std::less<typename std::iterator_traits<Iter>::value_type>());
}

template <class Iter>
inline StrictWeakOrderingResult strict_weak_ordering_check(Iter first,
                                                           Iter last) {
  return strict_weak_ordering_check(
      first, last, StrictWeakOrderingSettings{},
      std::less<typename std::iterator_traits<Iter>::value_type>());
}
