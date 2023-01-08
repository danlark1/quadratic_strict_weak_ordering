/*          Copyright Danila Kutenin, 2023-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */
#include <cassert>
#include <random>
#include <vector>

#include "quadratic_weak_ordering_check.h"

void test_empty() {
  int x = 0;
  auto result = strict_weak_ordering_check(&x, &x);
  assert(result.ok);
}

void test_clean() {
  std::vector<int> v(100);
  for (int i = 0; i < 100; ++i) {
    v[i] = i;
  }
  auto result = strict_weak_ordering_check(v.begin(), v.end());
  assert(result.ok);
}

void test_random() {
  std::random_device dev;
  std::mt19937_64 rng(dev());
  std::uniform_int_distribution<int> dist(1, 1000000);
  std::vector<int> v(100);
  for (int i = 0; i < 100; ++i) {
    v[i] = dist(rng);
  }
  StrictWeakOrderingSettings settings;
  settings.prior_sort = true;
  auto result = strict_weak_ordering_check(v.begin(), v.end(), settings);
  assert(result.ok);
}

void test_nan() {
  std::random_device dev;
  std::mt19937_64 rng(dev());
  std::uniform_int_distribution<int> dist(1, 1000000);
  std::vector<float> v(100);
  for (int i = 0; i < 100; ++i) {
    v[i] = dist(rng);
  }
  v[99] = std::numeric_limits<float>::quiet_NaN();
  StrictWeakOrderingSettings settings;
  settings.prior_sort = true;
  auto result = strict_weak_ordering_check(v.begin(), v.end(), settings);
  assert(!result.ok);
  assert(result.possible_reason.empty());
}

void test_bad_comp() {
  std::random_device dev;
  std::mt19937_64 rng(dev());
  std::uniform_int_distribution<int> dist(1, 1000000);
  std::vector<int> v(100);
  for (int i = 0; i < 100; ++i) {
    v[i] = dist(rng);
  }
  v[99] = v[98];
  StrictWeakOrderingSettings settings;
  settings.prior_sort = true;
  auto result = strict_weak_ordering_check(
      v.begin(), v.end(), settings,
      [](const int x, const int y) { return x <= y; });
  assert(!result.ok);
  assert(result.possible_reason.empty());
}

int main() {
  for (int i = 0; i < 10; ++i) {
    test_empty();
    test_clean();
    test_random();
    test_nan();
    test_bad_comp();
  }
}
