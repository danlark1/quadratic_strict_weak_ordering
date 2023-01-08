/*          Copyright Danila Kutenin, 2023-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <iostream>
#include <vector>

#include "quadratic_weak_ordering_check.h"

static constexpr int kNumElements = 10;

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data,
                                      std::size_t size) {
  std::vector<std::vector<int>> results(kNumElements,
                                        std::vector<int>(kNumElements));
  size_t current_size = 0;
  for (size_t i = 0; i < kNumElements; ++i) {
    for (size_t j = 0; j < kNumElements; ++j) {
      if (current_size < size) {
        results[i][j] = data[current_size];
        ++current_size;
      }
    }
  }
  std::vector<int> to_check(kNumElements);
  std::iota(to_check.begin(), to_check.end(), 0);
  StrictWeakOrderingSettings settings;
  settings.prior_sort = true;
  auto comp = [&results](const int x, const int y) { return results[x][y]; };
  auto result = strict_weak_ordering_check(to_check.begin(), to_check.end(),
                                           settings, comp);
  bool naive_accurate = true;
  for (size_t i = 0; i < kNumElements; ++i) {
    if (comp(i, i)) naive_accurate = false;
    for (size_t j = 0; j < kNumElements; ++j) {
      if (comp(i, j) && comp(j, i)) naive_accurate = false;
      for (size_t k = 0; k < kNumElements; ++k) {
        if (comp(i, j) && comp(j, k) && !comp(i, k)) naive_accurate = false;

        bool eq_i_j = !comp(i, j) && !comp(j, i);
        bool eq_j_k = !comp(j, k) && !comp(k, j);
        bool eq_i_k = !comp(i, k) && !comp(k, i);
        if (eq_i_j && eq_j_k && !eq_i_k) naive_accurate = false;
      }
    }
  }
  if (naive_accurate != result.ok) {
    abort();
  }
  return 0;
}
