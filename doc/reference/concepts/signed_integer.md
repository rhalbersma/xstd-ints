# `<xstd/ints/concepts/signed_integer.hpp>`

## `signed_integer`

```cpp
template<class T>
concept signed_integer = integer<T> and is_signed_v<T>;
```

- Every signed exact-width type models it under every cv-qualification, and no unsigned one does.
- *Remarks:* It subsumes `integer<T>`, which it names as an atomic conjunct, as `std::signed_integral` subsumes
  `std::integral`. An overload on `signed_integer` is chosen over one on `integer` for `int`, not ambiguous with it.
