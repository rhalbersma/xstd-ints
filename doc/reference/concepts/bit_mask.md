# `<xstd/ints/concepts/bit_mask.hpp>`

## `xstd::bit_mask`

```cpp
template<class T_cv, class T = remove_cv_t<T_cv>>
concept bit_mask =
  same_as<T, remove_cv_t<T_cv>> and
  (unsigned_integer<T> or (not signed_integer<T> and not integral<T>)) and
  regular<T> and
  requires(const T a) {
    { ~a } -> same_as<promoted_t<T>>;
  } and
  requires(T a, const T b) {
    a &= b;
    a ^= b;
    a |= b;
  } and
  requires(const T a, const T b) {
    { a & b } -> same_as<promoted_t<T>>;
    { a ^ b } -> same_as<promoted_t<T>>;
    { a | b } -> same_as<promoted_t<T>>;
  };
```

The bitmask type of [[bitmask.types]](https://eel.is/c++draft/bitmask.types): an unsigned integer, or any type that is
no kind of integer and has the operators.

### Models

- Every unsigned exact-width type models it under every cv-qualification, and so does every unsigned built-in, the
  narrow ones through promotion.
- No signed exact-width type models it, `const` or not, nor `signed char`, `int` or `long long`, whose `~` is negative.
- `bool` and the five character types do not, cv-qualified or not, although they answer every operator clause through
  promotion: `integral<T>` turns them away.
- `bitset<N>` models it, `const` too.
- The standard's own bitmask enumerations model it: `filesystem::perms`, `perm_options`, `copy_options`,
  `directory_options`, `launch`, `chars_format`, `regex_constants::syntax_option_type` and `match_flag_type`.
- An enumeration is one by its operators alone: `byte`, which the standard never calls a bitmask type, models it, and
  `filesystem::file_type` and `regex_constants::error_type`, enumerated types declared beside bitmask types, do not.
- The compound forms need only be valid, whatever they return, so libstdc++'s `ios_base` flag types, whose compound
  assignments return `const X&`, model it where they are not signed integers.

### Notes

- Over the built-in types it admits exactly what `popcount`, `countr_zero`, `rotl` and `bit_width` accept.
- No ordering is asked for: `bitset`, which has no `operator<`, models it.
- No shifts are asked for: the standard's bitmask enumerations, which have none, model it.

### See also

| Entity | Relation |
| :----- | :------- |
| [`unsigned_integer`](unsigned_integer.md) | the integers it admits |
| [`signed_integer`](signed_integer.md) | the integers it turns away |
| [`promoted_t`](../type_traits/promoted.md) | the type its operators return |
| [`popcount`](../bit/countl_zero.md), [`rotl`](../bit/rotl.md), [`bit_width`](../bit/has_single_bit.md) | take exactly its built-in models |
| [*BitmaskType*](https://en.cppreference.com/w/cpp/named_req/BitmaskType) | the standard's named requirement |
