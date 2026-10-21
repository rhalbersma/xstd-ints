# `<xstd/ints/concepts/signed_integer.hpp>`

## `xstd::signed_integer`

```cpp
template<class T>
concept signed_integer = integer<T> and is_signed_v<T>;
```

The signed half of an `integer` pair.

### Models

Every signed exact-width type models it under every cv-qualification, and no unsigned one does.

### Notes

It subsumes `integer<T>`, which it names as an atomic conjunct, as `std::signed_integral` subsumes `std::integral`. An
overload on `signed_integer` is chosen over one on `integer` for `int`, not ambiguous with it.

### See also

| Entity | Relation |
| :----- | :------- |
| [`integer`](integer.md) | the concept it refines |
| [`unsigned_integer`](unsigned_integer.md) | the other half |
| [`is_signed`](../type_traits/is_signed.md) | the trait it reads |
| [`std::signed_integral`](https://en.cppreference.com/w/cpp/concepts/signed_integral) | the standard's concept, whose subsumption of `std::integral` it mirrors |
