# `<xstd/ints/concepts.hpp>`

```cpp
namespace xstd {
  template<class T_cv, class T = remove_cv_t<T_cv>>
    concept integer_class = see below;
  template<class T>
    concept integer = see below;
  template<class T>
    concept signed_integer = see below;
  template<class T>
    concept unsigned_integer = see below;
  template<class T_cv, class T = remove_cv_t<T_cv>>
    concept bit_mask = see below;
  template<class T_cv, class T = remove_cv_t<T_cv>>
    concept alignable = see below;
  template<class T_cv, class T = remove_cv_t<T_cv>>
    concept nothrow_alignable = see below;
  template<class T_cv, class T = remove_cv_t<T_cv>>
    concept nothrow_const_operators = see below;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/ints/concepts/alignable.hpp>`](alignable.md) | `alignable`, `nothrow_alignable` |
| [`<xstd/ints/concepts/integer_class.hpp>`](integer_class.md) | `integer_class` |
| [`<xstd/ints/concepts/integer.hpp>`](integer.md) | `integer` |
| [`<xstd/ints/concepts/signed_integer.hpp>`](signed_integer.md) | `signed_integer` |
| [`<xstd/ints/concepts/unsigned_integer.hpp>`](unsigned_integer.md) | `unsigned_integer` |
| [`<xstd/ints/concepts/bit_mask.hpp>`](bit_mask.md) | `bit_mask` |
| [`<xstd/ints/concepts/nothrow_const_operators.hpp>`](nothrow_const_operators.md) | `nothrow_const_operators` |

In the definitions below, `numeric_limits`, `promoted_t`, `make_signed_t`, `make_unsigned_t`, `is_signed_v` and
`is_unsigned_v` are xstd's.

The umbrella exports the seven headers under `<xstd/ints/concepts/>`. Over the built-in integers, the 128-bit pair and
the near-misses `bool`, `char`, `char32_t`, `double`, `void`, `int*`, `int&` and `complex<double>`, cv-qualified or not:
`integer` holds exactly where one of its two halves does and never both; `integer` implies `integer_class`;
`nothrow_const_operators` implies `integer`; every `unsigned_integer` is `alignable` and no `signed_integer` is; and
`nothrow_alignable` implies `alignable`.
