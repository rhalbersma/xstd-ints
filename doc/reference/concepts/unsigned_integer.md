# `<xstd/ints/concepts/unsigned_integer.hpp>`

## `unsigned_integer`

```cpp
template<class T>
concept unsigned_integer = integer<T> and is_unsigned_v<T>;
```

- Every unsigned exact-width type models it under every cv-qualification, and no signed one does.
- *Remarks:* It subsumes `integer<T>`, as `std::unsigned_integral` subsumes `std::integral`. An overload on
  `unsigned_integer` is chosen over one on `integer` for `unsigned`, not ambiguous with it.
