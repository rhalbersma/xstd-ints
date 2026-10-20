# `<xstd/ints/concepts/unsigned_integer.hpp>`

## `xstd::unsigned_integer`

```cpp
template<class T>
concept unsigned_integer = integer<T> and is_unsigned_v<T>;
```

The unsigned half of an `integer` pair.

### Models

Every unsigned exact-width type models it under every cv-qualification, and no signed one does.

### Notes

It subsumes `integer<T>`, as `std::unsigned_integral` subsumes `std::integral`. An overload on `unsigned_integer` is
chosen over one on `integer` for `unsigned`, not ambiguous with it.

### See also

| Entity | Relation |
| :----- | :------- |
| [`integer`](integer.md) | the concept it refines |
| [`signed_integer`](signed_integer.md) | the other half |
| [`is_unsigned`](../type_traits/is_unsigned.md) | the trait it reads |
| [`alignable`](alignable.md) | every model of it is `alignable` |
| [`std::unsigned_integral`](https://en.cppreference.com/w/cpp/concepts/unsigned_integral) | the standard's concept, whose subsumption of `std::integral` it mirrors |
