# `<xstd/ints/concepts/integer_class.hpp>`

## `xstd::integer_class`

```cpp
template<class T_cv, class T = remove_cv_t<T_cv>>
concept integer_class =
  same_as<T, remove_cv_t<T_cv>> and
  numeric_limits<T>::is_specialized and
  numeric_limits<T>::is_integer and
  (numeric_limits<T>::radix == 2) and
  convertible_to<int, T> and
  convertible_to<size_t, T> and
  constructible_from<int, T> and
  constructible_from<size_t, T> and
  requires(T a) {
    { a++ } -> same_as<T>;
    { a-- } -> same_as<T>;
  } and
  requires(T a) {
    { ++a } -> same_as<T&>;
    { --a } -> same_as<T&>;
  } and
  requires(const T a) {
    { +a } -> same_as<promoted_t<T>>;
    { -a } -> same_as<promoted_t<T>>;
    { !a } -> same_as<bool>;
    { ~a } -> same_as<promoted_t<T>>;
  } and
  requires(T a, const T b) {
    { a *= b } -> same_as<T&>;
    { a /= b } -> same_as<T&>;
    { a %= b } -> same_as<T&>;
    { a += b } -> same_as<T&>;
    { a -= b } -> same_as<T&>;
  } and
  requires(T a, const size_t n) {
    { a <<= n } -> same_as<T&>;
    { a >>= n } -> same_as<T&>;
  } and
  requires(T a, const T b) {
    { a &= b } -> same_as<T&>;
    { a ^= b } -> same_as<T&>;
    { a |= b } -> same_as<T&>;
  } and
  requires(const T a, const T b) {
    { a * b } -> same_as<promoted_t<T>>;
    { a / b } -> same_as<promoted_t<T>>;
    { a % b } -> same_as<promoted_t<T>>;
    { a + b } -> same_as<promoted_t<T>>;
    { a - b } -> same_as<promoted_t<T>>;
  } and
  requires(const T a, const size_t n) {
    { a << n } -> same_as<promoted_t<T>>;
    { a >> n } -> same_as<promoted_t<T>>;
  } and
  requires(const T a, const T b) {
    { a & b } -> same_as<promoted_t<T>>;
    { a ^ b } -> same_as<promoted_t<T>>;
    { a | b } -> same_as<promoted_t<T>>;
  } and
  constructible_from<bool, T> and
  regular<T> and
  three_way_comparable<T, strong_ordering> and
  requires {
    { numeric_limits<T>::min() } -> same_as<T>;
    { numeric_limits<T>::max() } -> same_as<T>;
  };
```

An integer of any width, built-in or a class. The definition follows the order of
[[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc), generalized to integers of any width and
opened to built-in types, which that subclause's note admits.

### Semantic requirements

- A type models it only if its range is the one [[iterator.concept.winc]/3](https://eel.is/c++draft/iterator.concept.winc#3)
  gives a width: `min() + max()` is −1 for the signed half of a pair, and the unsigned half runs from 0 to twice the
  signed maximum plus one, with one digit more.
- A type models it only if value-initialization produces zero,
  [[iterator.concept.winc]/10](https://eel.is/c++draft/iterator.concept.winc#10): it does for every signed exact-width
  type and its unsigned counterpart in constant evaluation, Abseil's pair where it has a 128-bit intrinsic.

### Models

- Every standard integer type models it, the five character types included, and narrow types qualify because the closure
  requirements are stated against `promoted_t`.
- Every exact-width type models it, the 128-bit, Boost, Abseil and bit-precise ones included, under every
  cv-qualification.
- `bool` does not: C++17 removed its `++` and `--`.
- `byte` does not: it has every bitwise operator an integer has, and no arithmetic and no `numeric_limits`.

### Notes

It does not ask for the width requirement of [/3](https://eel.is/c++draft/iterator.concept.winc#3), mixed integral and
integer-class compound assignment ([/7.4](https://eel.is/c++draft/iterator.concept.winc#7.4)) or the common types of
[/12](https://eel.is/c++draft/iterator.concept.winc#12).

### See also

| Entity | Relation |
| :----- | :------- |
| [`integer`](integer.md) | the refinement that pairs each half with the other |
| [`alignable`](alignable.md) | shares its opening clauses |
| [`numeric_limits`](../limits/numeric_limits.md) | the limits it reads |
| [`promoted_t`](../type_traits/promoted.md) | the type its closure requirements are stated against |
| [`std::integral`](https://en.cppreference.com/w/cpp/concepts/integral) | the standard's concept for the built-in integral types |
