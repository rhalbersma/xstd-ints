# `<xstd/ints/type_traits/promoted.hpp>`

## `xstd::promoted_t`

```cpp
template<class T>
using promoted_t = see below;
```

The type of a `T` operand after integral promotion.

`promoted_t<T>` is the type of `+a` for an lvalue `a` of type `const T` where `T` is `std::integral`, which is what
[[conv.prom]](https://eel.is/c++draft/conv.prom) gives, and `T` otherwise. Narrow built-ins promote to `int`, or to
`int` or `unsigned` as the platform decides, `bool` to `int`, and `int` and wider are their own; a cv-qualified type
promotes as the unqualified type does, `short const` to `int`.

### Notes

- A non-integral type's own `operator+` is never asked: `decltype(+a)` is formed only for an integral `T`.
- For an integer-class type it is the type itself, which the closure requirements of `integer_class` rely on, and so it
  is for a bit-precise type, which is exempt from integral promotion whichever branch it takes: `uint128`,
  `absl::uint128`, `bit_uint<17>` and `bit_int<9>` are their own.
- It keeps every signed exact-width type signed.

### See also

| Entity | Relation |
| :----- | :------- |
| [`integer_class`](../concepts/integer_class.md) | states its closure requirements against it |
| [`bit_mask`](../concepts/bit_mask.md) | states its operators' results against it |
| [`boost::integral_promotion_t`](https://www.boost.org/doc/libs/latest/libs/type_traits/doc/html/boost_typetraits/reference/integral_promotion.html) | Boost.TypeTraits' counterpart, which keeps cv-qualifiers, promotes enumerations and leaves `char8_t`, `char16_t` and `char32_t` alone |
