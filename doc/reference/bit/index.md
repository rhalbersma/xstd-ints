# `<xstd/ints/bit.hpp>`

```cpp
namespace xstd {
  // [bit.byteswap], byteswap
  template<class T>
    constexpr auto byteswap(T x) noexcept(see below) -> T;

  // [bit.pow.two], integral powers of 2
  template<class T>
    constexpr auto has_single_bit(T x) noexcept(see below) -> bool;
  template<class T>
    constexpr auto bit_ceil(T x) -> T;
  template<class T>
    constexpr auto bit_floor(T x) noexcept(see below) -> T;
  template<class T>
    constexpr auto bit_width(T x) noexcept(see below) -> int;

  // [bit.rotate], rotating
  template<class T>
    constexpr auto rotl(T x, int s) noexcept(see below) -> T;
  template<class T>
    constexpr auto rotr(T x, int s) noexcept(see below) -> T;

  // [bit.count], counting
  template<class T>
    constexpr auto countl_zero(T x) noexcept(see below) -> int;
  template<class T>
    constexpr auto countl_one(T x) noexcept(see below) -> int;
  template<class T>
    constexpr auto countr_zero(T x) noexcept(see below) -> int;
  template<class T>
    constexpr auto countr_one(T x) noexcept(see below) -> int;
  template<class T>
    constexpr auto popcount(T x) noexcept(see below) -> int;

  // P3793R2, shifting
  template<class T, class S>
    constexpr auto shl(T x, S s) noexcept(see below) -> T;
  template<class T, class S>
    constexpr auto shr(T x, S s) noexcept(see below) -> T;

  // P3104R6, permuting
  template<class T>
    constexpr auto bit_compress(T src, T mask) noexcept(see below) -> T;
  template<class T>
    constexpr auto bit_expand(T src, T mask) noexcept(see below) -> T;
  template<class T>
    constexpr auto bit_repeat(T x, int l) -> T;
  template<class T>
    constexpr auto bit_reverse(T x) noexcept(see below) -> T;

  // selecting, an xstd extension
  template<unsigned_integer T>
    constexpr auto bit_select(T x, int k) -> int;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/ints/bit/bit_ceil.hpp>`](has_single_bit.md) | `bit_ceil` |
| [`<xstd/ints/bit/bit_compress.hpp>`](bit_compress.md) | `bit_compress` |
| [`<xstd/ints/bit/bit_expand.hpp>`](bit_expand.md) | `bit_expand` |
| [`<xstd/ints/bit/bit_floor.hpp>`](has_single_bit.md) | `bit_floor` |
| [`<xstd/ints/bit/bit_repeat.hpp>`](bit_repeat.md) | `bit_repeat` |
| [`<xstd/ints/bit/bit_reverse.hpp>`](bit_reverse.md) | `bit_reverse` |
| [`<xstd/ints/bit/bit_select.hpp>`](bit_select.md) | `bit_select` |
| [`<xstd/ints/bit/bit_width.hpp>`](has_single_bit.md) | `bit_width` |
| [`<xstd/ints/bit/byteswap.hpp>`](byteswap.md) | `byteswap` |
| [`<xstd/ints/bit/countl_one.hpp>`](countl_zero.md) | `countl_one` |
| [`<xstd/ints/bit/countl_zero.hpp>`](countl_zero.md) | `countl_zero` |
| [`<xstd/ints/bit/countr_one.hpp>`](countl_zero.md) | `countr_one` |
| [`<xstd/ints/bit/countr_zero.hpp>`](countl_zero.md) | `countr_zero` |
| [`<xstd/ints/bit/has_single_bit.hpp>`](has_single_bit.md) | `has_single_bit` |
| [`<xstd/ints/bit/popcount.hpp>`](countl_zero.md) | `popcount` |
| [`<xstd/ints/bit/rotl.hpp>`](rotl.md) | `rotl` |
| [`<xstd/ints/bit/rotr.hpp>`](rotl.md) | `rotr` |
| [`<xstd/ints/bit/shl.hpp>`](shl.md) | `shl` |
| [`<xstd/ints/bit/shr.hpp>`](shl.md) | `shr` |

The umbrella exports each header under `<xstd/ints/bit/>`, one function to a header.

Each function here is one overload set. Its first overload forwards to the standard function, constrained on the
standard call with the same arguments being well-formed rather than on `std::unsigned_integral`, which admits `bool` and
`char8_t` where `<bit>` refuses them. Beside it, constrained on that call being ill-formed, one template answers every
other type the function takes, working one `size_t` block at a time, since [[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc) gives every
integer-class type an explicit conversion to and from `size_t`. The two constraints are complementary, so at most one of
them is viable for any argument, and the overload set is the same whichever header a translation unit includes first.
