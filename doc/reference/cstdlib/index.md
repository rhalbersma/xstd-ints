# `<xstd/ints/cstdlib.hpp>`

```cpp
namespace xstd {
  template<integer I>
    struct div_result;
  template<integer I>
    div_result(I, I) -> div_result<I>;

  template<integer I>
    constexpr auto sign(I x) noexcept(nothrow_const_operators<I>) -> int;
  template<integer I>
    constexpr auto abs(I x) noexcept(nothrow_const_operators<I>) -> I;
  template<integer I>
    constexpr auto unsigned_abs(I x) noexcept(nothrow_const_operators<I>) -> xstd::make_unsigned_t<I>;

  template<integer I>
    constexpr auto div(I numer, I denom) noexcept(nothrow_const_operators<I>) -> div_result<I>;
  template<integer I>
    constexpr auto div_euclid(I numer, I denom) noexcept(nothrow_const_operators<I>) -> div_result<I>;
  template<integer I>
    constexpr auto div_floor(I numer, I denom) noexcept(nothrow_const_operators<I>) -> div_result<I>;
  template<integer I>
    constexpr auto div_ceil(I numer, I denom) noexcept(nothrow_const_operators<I>) -> div_result<I>;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/ints/cstdlib/div_result.hpp>`](div_result.md) | `div_result` |
| [`<xstd/ints/cstdlib/sign.hpp>`](sign.md) | `sign` |
| [`<xstd/ints/cstdlib/abs.hpp>`](abs.md) | `abs` |
| [`<xstd/ints/cstdlib/unsigned_abs.hpp>`](unsigned_abs.md) | `unsigned_abs` |
| [`<xstd/ints/cstdlib/div.hpp>`](div.md) | `div` |
| [`<xstd/ints/cstdlib/div_euclid.hpp>`](div_euclid.md) | `div_euclid` |
| [`<xstd/ints/cstdlib/div_floor.hpp>`](div_floor.md) | `div_floor` |
| [`<xstd/ints/cstdlib/div_ceil.hpp>`](div_ceil.md) | `div_ceil` |

`xstd::make_unsigned_t` and `xstd::numeric_limits` below are xstd's, which extend the standard's to every `integer`.

The umbrella exports each header below.

Each function is constrained on `integer`, so a character type and `bool`, which are integral, are turned away.

Each division header exports `div_result` with it.

The four divisions take their result vocabulary and their rounding semantics from [P3724R4](https://github.com/cplusplus/papers/issues/2354),
and only these four of its rounding modes.
