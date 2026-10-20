# `<xstd/ints/concepts/integer.hpp>`

## `integer`

```cpp
template<class T>
concept integer =
  integer_class<T> and
  requires {
    typename make_signed_t<T>;
    typename make_unsigned_t<T>;
  } and
  same_as<make_signed_t<make_unsigned_t<T>>, make_signed_t<T>> and
  same_as<make_unsigned_t<make_signed_t<T>>, make_unsigned_t<T>> and
  (same_as<T, make_signed_t<T>> or same_as<T, make_unsigned_t<T>>);
```

It is [P3701R0](https://github.com/cplusplus/papers/issues/2330)'s arithmetic boundary over the built-ins, extended to integer-class
types that a specialization of `make_signed` and `make_unsigned` pairs.

- Every exact-width type models it, the 128-bit, Boost, Abseil and bit-precise ones included, under every
  cv-qualification.
- `signed char`, `unsigned char`, `int8_t` and `uint8_t` model it; `char`, `wchar_t`, `char8_t`, `char16_t`, `char32_t`
  and `bool` do not under any cv-qualification, and neither do `double`, `void` and `int*`.
- The last clause is what turns the character types away, none being named: a character type is neither half of its own
  pair, `make_signed_t<char>` being `signed char`.
- `bool` fails `integer_class` and never reaches the pair.
- `unsigned _BitInt(1)`, which has no signed counterpart, does not model it, and asking is not an error.
- A trip through the other signedness and back is the identity.
- An integer-class type that no specialization pairs does not model it.
- A pair whose round trip lands on a third type is turned away by the two `same_as` clauses that compose the
  transformations.
