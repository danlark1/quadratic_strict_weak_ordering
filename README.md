Quadratic strict weak ordering check
==============================================================

This repository presents a quadratic strict weak ordering check. Strict weak
ordering for a comparator $comp$ and set $S$ means that it satisfies the conditions for all $x, y, z \in S$:

1. Irreflexivity: $comp(x, x)$ must be false
2. Antisymmetry: $comp(x, y)$ implies $!comp(y, x)$
3. Transitivity: if $comp(x, y)$ and $comp(y, z)$ then $comp(x, z)$
4. Two objects x and y are equivalent if both $comp(x, y)$ and $comp(y, x)$ are false. Then transitivity of equivalence should be met.

Naive algorithm for steps 3 and 4 require $O(|S|^3)$ comparisons. However, there are faster algorithms to do that. In order to check if set $S$ satisfies this condition, we are going to do the following:

1. Sort $S$ with some algorithm that correctly sorts it if $S$ satisfies strict weak ordering. **And does not fail if it does not satisfy it.** Bubble sort or heap sort should be good.
2. Check that the range is sorted. If it's not, we know that $S$ does not satisfy strict weak ordering.
3. If it's sorted, use the following algorithm:

1. Find the minimal $P$ such that $S[0] < S[P]$. If no such $P$, set P to SIZE.
2. For all $A < B < P$ check $(!comp(S[A], S[B])$ and $!comp(S[B], S[A]))$, i.e. all elements before $P$ are equivalent.
3. For all $A < P$ and $B \geq P$, check $(comp(S[A], S[B])$ and $!comp(S[B], S[A]))$, i.e. all elements separated by $P$ follows transitivity.
4. Remove the first P elements from S. Go to step 2.

If any condition at step 2 and 3 is not met, return FALSE. It means strict weak ordering is not met.

The runtime of this algorithm is $O(|S|^2)$. Assume we eliminate $x_1, \ldots, x_k$ elements on each iteration and thus we made

$$
  O(x_1^2 + x_1(|S| - x_1) + x_2^2 + x_2(|S| - x_1 - x_2) + \ldots) = O(x_1|S| + x_2|S| + \ldots) = O(|S| \times |S|)
$$

comparisons.

# How can you use it?

This functions can be used for testing the properties of the passed ranges. Suggested APIs:

* `bool strict_weak_ordering_check(first, last, settings, comp)`

`settings`, `comp` arguments are not mandatory. We support C++11. `settings` support `.prior_sort` and some other
implementation defined arguments. See [test](./test.cpp) for examples.

```cpp
$ clang++ test.cpp -g -std=c++11 -O3 -o test
$ clang++ -g fuzz.cpp -fsanitize=address,fuzzer -std=c++11 -O3 -o fuzz
```

In standard libraries this can be used to check if the range satisfies strict weak ordering for a sampled range of
$O(\sqrt{n})$ elements without hurting too much performance.

[Sortcheck repository](https://github.com/yugr/sortcheck#what-are-current-results) found a lot of problems out there and this can contribute to find even more.

# Proof

If strict weak ordering is met, the algorithm obviously returns TRUE. Assume it returns TRUE and strict weak ordering is not met.

## Claim 1. If $comp(S[i], S[j])$, then $i < j$

Assume $i \geq j$ and $comp(S[i], S[j])$. Then there is a moment when $P$ at step 1 exceeds element $j$ for the first time. Then it holds

$0 < j - \mathrm{removed index} = J < P$

Then element with index $i - \mathrm{removed index} = I = B$ and $A = J$ is checked at steps 2 or 3 and the opposite condition holds.

## Claim 2. $comp(S[i], S[i])$ is false

If it's true, then $i < i$.

## Claim 3. $comp(S[i], S[j])$ and $comp(S[j], S[k])$ implies $comp(S[i], S[k])$

Assume the opposite, i.e. $comp(S[i], S[j])$ and $comp(S[j], S[k])$ and $!comp(S[i], S[k])$.
According to claim 1, we have $i < j < k$.

Find $P$ which exceeds element $i$ for the first time. Then at that step we have

* $comp(S[i - \mathrm{removed index}], S[j - \mathrm{removed index}])$
* If $P$ exceeds element $j$, then step 2 should return false
* $comp(S[j - \mathrm{removed index}], S[k - \mathrm{removed index}])$
* If $P$ exceeds element $k$, then step 2 should return false
* It means $P < j - \mathrm{removed index} < k - \mathrm{removed index}$. Then for $A = i - \mathrm{removed index}$, $B = k - \mathrm{removed index}$ holds the desired condition.

## Claim 4. Assymetry $comp(S[i], S[j])$ implies $!comp(S[j], S[i])$

Obviously holds from claim 1: $i < j < i$ otherwise.

## Claim 5. Transitivity of equivalence also holds

It means that $!comp(S[I], S[J])$, and $!comp(S[J], S[I])$, and $!comp(S[J], S[K])$, and $!comp(S[K], S[J])$, and ( $comp(S[I], S[K])$ or $comp(S[K], S[I])$ ).

Without loss of generality assume $comp(S[I], S[K])$ (also means $I < K$ according to claim 1). If $I \leq J$, then

* Find first $P$ which exceeds element at position $I$
* If $P > K$, then $!comp(S[I], S[K])$ according to step 2.
* If $P \leq K$, then
  * If $P > J$, then $comp(S[J], S[K])$ according to step 3.
  * If $P \leq J$, then $comp(S[I], S[J])$ according to step 3.

If $J < I$, then

* Find first $P$ which exceeds element at position $J$
* If $K < P$, then $I < K < P$ and $!comp(S[I], S[K])$
* If $K \geq P$, then $comp(S[J], S[K])$

