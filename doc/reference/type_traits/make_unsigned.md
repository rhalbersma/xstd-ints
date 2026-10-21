# `<xstd/ints/type_traits/make_unsigned.hpp>`

## `xstd::make_unsigned`, `xstd::make_unsigned_t`

```cpp
template<class T>
struct make_unsigned;

template<class T>
using make_unsigned_t = typename make_unsigned<T>::type;
```

The unsigned counterpart of an integral, enumeration or integer-class type.

### Member types

| Type | Definition |
| :--- | :--------- |
| `type` | the unsigned counterpart of `T`, where it has one, as below |

- For a cv-unqualified integral or enumeration type other than `bool`, the member `type` is `std::make_unsigned_t<T>`.
- For a cv-unqualified type that models `integer_class`, is not `std::integral` and is unsigned, `type` is `T`.
- For a cv-qualified `T` whose unqualified type has a member `type`, `type` is that with the same qualifiers. A
  cv-qualified type takes no other path, so it has a member `type` exactly when its unqualified type does.
- Otherwise there is no member `type`, so `make_unsigned_t<T>` is a substitution failure where `std::make_unsigned_t<T>`
  would be a hard error: `bool`, `double`, `int*`, `complex<double>` and `void` have none.

### Remarks

A program may specialize it for the signed half of an integer-class pair, as the `ext` headers do.

### Notes

Every signed exact-width type maps to a type modelling `unsigned_integer`.

### See also

| Entity | Relation |
| :----- | :------- |
| [`make_signed`](make_signed.md) | the other direction |
| [`make_unsigned<signed _BitInt(N)>`](make_signed.md#xstdmake_signedunsigned-_bitintn-xstdmake_unsignedsigned-_bitintn) | the bit-precise specializations |
| [`integer`](../concepts/integer.md) | asks that the two be mutually inverse |
| [`boost::int128`](../boost/int128.md), [`absl::int128`](../absl/int128.md) | the `ext` headers' specializations |
| [`std::make_unsigned`](https://en.cppreference.com/w/cpp/types/make_unsigned) | the standard trait it extends |
