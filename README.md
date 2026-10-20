# Integer extensions to the C++ Standard Library

> "Go to the ant, thou sluggard; consider her ways, and be wise."
>
> Proverbs 6:6, King James Version

[![Project Status: Active](https://www.repostatus.org/badges/latest/active.svg)](https://www.repostatus.org/#active)
[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![GCC](https://github.com/rhalbersma/xstd-ints/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/gcc.yml)
[![MinGW](https://github.com/rhalbersma/xstd-ints/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/mingw.yml)
[![Clang](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang.yml)
[![Clang-libc++](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-libc%2B%2B.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-libc%2B%2B.yml)
[![Apple Clang](https://github.com/rhalbersma/xstd-ints/actions/workflows/apple-clang.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/apple-clang.yml)
[![Clang-CL](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-cl.yml)
[![MSVC](https://github.com/rhalbersma/xstd-ints/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/msvc.yml)
[![Emscripten](https://github.com/rhalbersma/xstd-ints/actions/workflows/emscripten.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/emscripten.yml)
[![Coverage](https://codecov.io/gh/rhalbersma/xstd-ints/branch/main/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd-ints)
[![Consumption](https://github.com/rhalbersma/xstd-ints/actions/workflows/consumption.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/consumption.yml)
[![Sanitizers](https://github.com/rhalbersma/xstd-ints/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/sanitizers.yml)
[![Clang-Tidy](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-tidy.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-tidy.yml)
[![MSVC-Analyze](https://github.com/rhalbersma/xstd-ints/actions/workflows/msvc-analyze.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/msvc-analyze.yml)
[![CodeQL](https://github.com/rhalbersma/xstd-ints/actions/workflows/codeql.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/codeql.yml)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/rhalbersma/xstd-ints/badge)](https://scorecard.dev/viewer/?uri=github.com/rhalbersma/xstd-ints)
[![Documentation](https://github.com/rhalbersma/xstd-ints/actions/workflows/pages.yml/badge.svg)](https://rhalbersma.github.io/xstd-ints/)

xstd-ints is a small, portable, header-only collection of integer extensions to
the C++ standard library: concepts, traits, limits, division, alignment and
character conversion, open to integer-class types where the standard is closed
over the built-ins. It relies on the [C++23](https://wg21.link/N4950) standard
and targets the draft [C++29](https://wg21.link/std) standard library. All
public APIs are in namespace `xstd`.

## Usage


```cpp
#include <xstd/ints/cstdlib.hpp>
#include <xstd/ints/memory.hpp>
#include <climits>

static_assert(xstd::unsigned_abs(INT_MIN) == static_cast<unsigned>(INT_MAX) + 1u);
static_assert(xstd::align_up(100uz, 64) == 128);
static_assert(xstd::align_down(100uz, 64) == 64);
static_assert(not xstd::is_aligned(100uz, 64));

constexpr auto result = xstd::div_euclid(-8, 3);
static_assert(result.quotient == -3);
static_assert(result.remainder == 1);
```

The numeric concepts, limits, and traits extend their standard counterparts to
integer-class types, including native bit-precise integers and the portable
128-bit aliases:

```cpp
#include <xstd/ints/concepts/signed_integer.hpp>
#include <xstd/ints/concepts/unsigned_integer.hpp>
#include <xstd/ints/cstdint.hpp>
#include <xstd/ints/limits.hpp>

#ifdef XSTD_HAS_INT128
static_assert(xstd::signed_integer<xstd::int128>);
static_assert(xstd::unsigned_integer<xstd::uint128>);
#endif

#ifdef XSTD_HAS_BIT_INT
static_assert(xstd::signed_integer<xstd::bit_int<24>>);
static_assert(xstd::numeric_limits<xstd::bit_uint<24>>::digits == 24);
#endif
```

The `integer` concept and its signedness refinements are cv-transparent. The
integer functions return the argument type rather than a promoted one. Following
[P3701R0](https://github.com/cplusplus/papers/issues/2330), `bool` and the five character types
`char`, `wchar_t`, `char8_t`, `char16_t`, and `char32_t` are excluded. `signed
char` and `unsigned char` remain integers, preserving exact-width 8-bit aliases.
Nothing is excluded by name: `integer_class` admits the built-in integers, so
`integer` is defined over it plus the signedness behaviour every integer has and
no character type does. An integer is one of its own signed/unsigned pair, and
its two transformations compose back; a character type is a third type beside
its pair, `make_signed_t<char>` being `signed char`. `bool` never reaches the
question, C++17 having removed the `++` that `integer_class` asks for. xstd
extends the paper's built-in boundary with paired `integer_class` types.
Character conversion delegates to `std::to_chars` `char` and every standard integer type,
the types it declares overloads for, and refuses the other character types.

`div_result` is the common quotient/remainder result for truncating `div`,
Euclidean `div_euclid`, floored `div_floor`, and ceiling `div_ceil`. Their vocabulary and relevant
rounding semantics follow [P3724R4](https://github.com/cplusplus/papers/issues/2354), but xstd
intentionally implements only this established four-operation family.
A `div_result` renders as `(quotient, remainder)` for every element type it accepts, and takes
the tuple format specs, `{:n}` and `{:m}` among them, for every element type wherever the
standard library formats tuples; on one that formats none, it takes a string's.

```cpp
#include <xstd/ints/cstdlib.hpp>
#include <xstd/ints/format.hpp>
#include <format>

auto const text = std::format("{:>12}", xstd::div_euclid(-8, 3)); // "     (-3, 1)"
```

`to_chars` widens `std::to_chars` to the integers it does not cover, returning
its `std::to_chars_result`. `to_chars_max_size<T>` is the size that holds any
value of `T` at any base, so a buffer of it never comes back `value_too_large`:

```cpp
#include <xstd/ints/charconv.hpp>
#include <xstd/ints/cstdint.hpp>
#include <xstd/ints/limits.hpp>
#include <array>
#include <string_view>

#ifdef XSTD_HAS_INT128
auto const value = xstd::numeric_limits<xstd::uint128>::max();
auto buffer = std::array<char, xstd::to_chars_max_size<xstd::uint128>>{};
auto const [ptr, ec] = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), value, 16);
auto const text = std::string_view(buffer.data(), ptr); // thirty-two f's
#endif
```

An `<xstd/ints/ext/>` header pairs a third-party integer class with its counterpart,
which is all the concepts and the functions were waiting for. It brings the
library it adapts with it, so naming it is enough:

```cpp
#include <xstd/ints/concepts/signed_integer.hpp>
#include <xstd/ints/cstdlib.hpp>
#include <xstd/ints/ext/boost/int128.hpp> // brings boost::int128 with it

using i128 = boost::int128::int128;
static_assert(xstd::signed_integer<i128>);

constexpr auto result = xstd::div_floor(i128{-8}, i128{3});
static_assert(result.quotient == i128{-3} and result.remainder == i128{1});
```

[Boost.Hash2](https://www.boost.org/doc/libs/latest/libs/hash2/) writes integers of at most 64 bits, takes `__int128` as integral only
outside strict ISO mode, cannot be taught a class in namespace `std`, and hashes a
`_BitInt(24)` through bytes that include its padding. `hash_append_int` hashes any
xstd integer by its value instead: one of at most 64 bits as the narrowest
standard type that holds it, and a wider one as 64-bit words, low word first, each
sign- or zero-extended. Under `little_endian_flavor` the message is the value's
little-endian bytes, so every 128-bit type gives one digest, a `bit_int<64>` hashes
as an `int64_t` does and a `bit_int<24>` as an `int32_t`, in constant evaluation too:

```cpp
#include <xstd/ints/cstdint.hpp>
#include <xstd/ints/ext/boost/hash2.hpp> // brings Boost.Hash2's flavors with it
#include <boost/hash2/fnv1a.hpp>
#include <cstdint>

template<class... T>
[[nodiscard]] constexpr auto fnv1a_of(T... values)
        -> std::uint64_t
{
        auto h = boost::hash2::fnv1a_64{};
        (xstd::hash_append_int(h, boost::hash2::little_endian_flavor{}, values), ...);
        return h.result();
}

#ifdef XSTD_HAS_INT128
static_assert(fnv1a_of(xstd::uint128{1} << 100) == fnv1a_of(std::uint64_t{0}, std::uint64_t{1} << 36)); // low word first
#endif
```

See [the reference](doc/reference.md) for each entity's specification, [the design notes](doc/design.md) for rationale and for customizing an
integer-class type, and [CONTRIBUTING.md](CONTRIBUTING.md) to build the library itself.

## Headers


| Header | Additions | Description | Specification |
| :----- | :-------- | :---------- | :-------- |
| `<xstd/ints/concepts/integer_class.hpp>` <br> `<xstd/ints/concepts/integer.hpp>` <br> `<xstd/ints/concepts/signed_integer.hpp>` <br> `<xstd/ints/concepts/unsigned_integer.hpp>` <br> `<xstd/ints/concepts/bit_mask.hpp>` <br> `<xstd/ints/concepts/alignable.hpp>` <br> `<xstd/ints/concepts/nothrow_const_operators.hpp>` | `integer_class` <br> `integer` <br> `signed_integer` <br> `unsigned_integer` <br> `bit_mask` <br> `alignable`, `nothrow_alignable` <br> `nothrow_const_operators` | The operations [[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc) states of an integer-class type <br> [P3701](https://github.com/cplusplus/papers/issues/2330) arithmetic domain, extended to paired integer-class types <br> Open form of `std::signed_integral` <br> Open form of `std::unsigned_integral` <br> The standard's bitmask type: an unsigned integer, or any other non-integer type with the bitmask operators, such as `std::bitset` or an enumeration that overloads them <br> What the alignment functions ask of a type, and whether its operations carry `noexcept` <br> Exception specification of the integer functions | [[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc) (integer-class type) <br> [P3701R0](https://github.com/cplusplus/papers/issues/2330), [[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc) <br> [[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc) (integer-class types) <br> [[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc) (integer-class types) <br> [[bitmask.types]](https://eel.is/c++draft/bitmask.types) <br> none <br> none |
| `<xstd/ints/bit.hpp>` | `bit_ceil` <br> `bit_compress` <br> `bit_expand` <br> `bit_floor` <br> `bit_repeat` <br> `bit_reverse` <br> `bit_select` <br> `bit_width` <br> `byteswap` <br> `countl_one` <br> `countl_zero` <br> `countr_one` <br> `countr_zero` <br> `has_single_bit` <br> `popcount` <br> `rotl` <br> `rotr` <br> `shl` <br> `shr` | `std::bit_ceil`, widened to the 128-bit integer classes and bit-precise widths `<bit>` declines <br> Gather the bits a mask selects into the low bits (PEXT) <br> Scatter the low bits to the positions a mask selects (PDEP) <br> `std::bit_floor`, widened the same way <br> Repeat the low `l` bits across the width <br> Mirror the bits, any xstd unsigned integer <br> Position of the `k`-th one bit, the inverse of rank (xstd extension) <br> `std::bit_width`, widened the same way <br> `std::byteswap`, widened to every xstd integer whose width is a whole number of bytes, and taking whatever `std::byteswap` takes, `bool` and the character types included <br> `std::countl_one`, widened the same way <br> `std::countl_zero`, widened the same way <br> `std::countr_one`, widened the same way <br> `std::countr_zero`, widened the same way <br> `std::has_single_bit`, widened the same way <br> `std::popcount`, widened the same way <br> `std::rotl`, widened the same way <br> `std::rotr`, widened the same way <br> Shift left by any count of any integer type, a negative count shifting right; never undefined <br> Shift right the same way, rounding towards negative infinity | [[bit.pow.two]](https://eel.is/c++draft/bit.pow.two) <br> [P3104R6](https://github.com/cplusplus/papers/issues/1768) <br> [P3104R6](https://github.com/cplusplus/papers/issues/1768) <br> [[bit.pow.two]](https://eel.is/c++draft/bit.pow.two) <br> [P3104R6](https://github.com/cplusplus/papers/issues/1768) <br> [P3104R6](https://github.com/cplusplus/papers/issues/1768) <br> none <br> [[bit.pow.two]](https://eel.is/c++draft/bit.pow.two) <br> [[bit.byteswap]](https://eel.is/c++draft/bit.byteswap) <br> [[bit.count]](https://eel.is/c++draft/bit.count) <br> [[bit.count]](https://eel.is/c++draft/bit.count) <br> [[bit.count]](https://eel.is/c++draft/bit.count) <br> [[bit.count]](https://eel.is/c++draft/bit.count) <br> [[bit.pow.two]](https://eel.is/c++draft/bit.pow.two) <br> [[bit.count]](https://eel.is/c++draft/bit.count) <br> [[bit.rotate]](https://eel.is/c++draft/bit.rotate) <br> [[bit.rotate]](https://eel.is/c++draft/bit.rotate) <br> [P3793R2](https://github.com/cplusplus/papers/issues/2399) <br> [P3793R2](https://github.com/cplusplus/papers/issues/2399) |
| `<xstd/ints/charconv.hpp>` | `to_chars` <br> `to_chars_max_size` | `std::to_chars`, widened to xstd integers it does not cover <br> Buffer size that holds any value of `T` at any base | [[charconv.to.chars]](https://eel.is/c++draft/charconv.to.chars) <br> none |
| `<xstd/ints/cstdint.hpp>` | `bit_int<N>` <br> `bit_uint<N>` <br> `bit_int_max_width` <br> `int128` <br> `uint128` | Native bit-precise signed integer (when available) <br> Native bit-precise unsigned integer (when available) <br> Maximum native bit-precise width (when available) <br> Platform 128-bit signed integer (when available) <br> Platform 128-bit unsigned integer (when available) | [P3666R0](https://github.com/cplusplus/papers/issues/2420) <br> [P3666R0](https://github.com/cplusplus/papers/issues/2420) <br> none <br> none <br> none |
| `<xstd/ints/cstdlib.hpp>` | `div_result` <br> `sign` <br> `abs` <br> `unsigned_abs` <br> `div` <br> `div_euclid` <br> `div_floor` <br> `div_ceil` | Defaulted equality comparison <br> `-1`, `0`, or `1`; `0` or `1` when unsigned <br> `constexpr`, any xstd integer <br> Total <code>&#124;x&#124;</code>, returning the unsigned counterpart <br> Truncated division, any xstd integer <br> Euclidean division <br> Floored division <br> Ceiling division | none <br> [Boost.Math](https://www.boost.org/doc/libs/latest/libs/math/doc/html/math_toolkit/sign_functions.html) <br> [P0533R9](https://github.com/cplusplus/papers/issues/104) (reviewed implementation wording) <br> [Rust `unsigned_abs`](https://doc.rust-lang.org/std/primitive.i32.html#method.unsigned_abs) (no C++ equivalent) <br> [P0533R9](https://github.com/cplusplus/papers/issues/104) (reviewed implementation wording) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) <br> [P3724R4](https://github.com/cplusplus/papers/issues/2354) (`div_rem_to_pos_inf`) |
| `<xstd/ints/ext/absl.hpp>` | `make_unsigned<absl::int128>` <br> `make_signed<absl::uint128>` | Pairs Abseil's two integer-class types, so each models `integer` | none <br> none |
| `<xstd/ints/ext/boost.hpp>` | `make_unsigned<boost::int128::int128>` <br> `make_signed<boost::int128::uint128>` <br> `hash_append_int` | Pairs [Boost.Int128](https://github.com/cppalliance/int128)'s two integer-class types, so each models `integer` <br> Hashes any xstd integer's value, never its object representation, into a Boost.Hash2 algorithm; a function to call, which `hash_append` does not find | none <br> none <br> none |
| `<xstd/ints/format.hpp>` | `formatter<div_result>` | `std::format` support for every element type `div_result` accepts | [P3391](https://github.com/cplusplus/papers/issues/2046) (reviewed constexpr-format wording) |
| `<xstd/ints/limits.hpp>` | `numeric_limits` | Open `std::numeric_limits`, and its specializations for the bit-precise `_BitInt(N)` types | [[numeric.limits]](https://eel.is/c++draft/numeric.limits) |
| `<xstd/ints/memory.hpp>` | `align_up` <br> `align_down` <br> `is_aligned` | Round a value up to a power-of-two alignment, any xstd unsigned integer <br> Round a value down to a power-of-two alignment <br> Whether rounding would change anything | [Boost.Align](https://www.boost.org/doc/libs/latest/doc/html/align.html) (`align_up`), [LLVM `alignTo`](https://llvm.org/doxygen/namespacellvm.html) <br> [Boost.Align](https://www.boost.org/doc/libs/latest/doc/html/align.html) (`align_down`), [LLVM `alignDown`](https://llvm.org/doxygen/namespacellvm.html) <br> [Boost.Align](https://www.boost.org/doc/libs/latest/doc/html/align.html) (`is_aligned`) |
| `<xstd/ints/type_traits/is_signed.hpp>` <br> `<xstd/ints/type_traits/is_unsigned.hpp>` <br> `<xstd/ints/type_traits/make_signed.hpp>` <br> `<xstd/ints/type_traits/make_unsigned.hpp>` <br> `<xstd/ints/type_traits/promoted.hpp>` | `is_signed` <br> `is_unsigned` <br> `make_signed` <br> `make_unsigned` <br> `promoted_t` | `std::is_signed`, opened to integer-class types <br> `std::is_unsigned`, opened to integer-class types <br> Open, user-specializable `std::make_signed` <br> Open, user-specializable `std::make_unsigned` <br> What a type's own operators yield: [[conv.prom]](https://eel.is/c++draft/conv.prom) for a built-in, the type itself otherwise | none <br> none <br> none <br> none <br> [[conv.prom]](https://eel.is/c++draft/conv.prom) |

The native bit-precise aliases are available when the compiler defines
`__BITINT_MAXWIDTH__`; `XSTD_HAS_BIT_INT` detects their public API. Their current
minimum width is two so every exposed type has a signed or unsigned counterpart.
The 128-bit aliases and their `make_signed` and `make_unsigned` associations are
defined together in `<xstd/ints/cstdint/int128.hpp>`; `<xstd/ints/cstdint.hpp>` is their
umbrella. They alias the Microsoft STL's integer classes or the compiler's `__int128`, and
`XSTD_HAS_INT128` detects them: GCC and Clang on 32-bit x86 and ARM have neither, and there the
aliases are absent rather than an error. The two `<xstd/ints/ext/>` umbrellas export `<xstd/ints/ext/absl/int128.hpp>`, and
`<xstd/ints/ext/boost/int128.hpp>` with `<xstd/ints/ext/boost/hash2.hpp>`, and each needs
the libraries it adapts on the include path; nothing above them exports either. Each
adapted header is exported in turn, so including one is enough: `<xstd/ints/ext/absl/int128.hpp>`
brings `absl::int128` and `absl::uint128` with it.

The C++20 `<bit>` functions take `std::unsigned_integral`, and C++23's `std::byteswap`
takes `std::integral`, which every 128-bit integer *class* fails. Each `<xstd/ints/bit/>`
header is one overload set: the standard function wherever it accepts the argument, and
beside it one template answering every other `unsigned_integer`, or for `byteswap` every
`integer` whose width is a whole number of bytes. The template works through the value one
`std::size_t` word at a time, so it covers `xstd::uint128` where it is a class, as it is in
the Microsoft STL, and the types the two `<xstd/ints/ext/>` adaptors adapt, with nothing in those
adaptors beyond the `make_signed` and `make_unsigned` associations. Its exception
specification is that of the type's operators, so it is `noexcept` for every type but
`absl::uint128`, whose operators declare nothing; `bit_ceil`, `bit_repeat` and `bit_select` have a
precondition, and are not `noexcept` at all. Where `xstd::uint128` is the built-in
`unsigned __int128` and the standard library leaves it out of `std::unsigned_integral`, as
libstdc++ does in a strictly conforming translation unit on GCC, the word template answers
it too, so every configuration has the whole of `<bit>` at that width.

`countl_zero`, `countr_zero` and `popcount` take `bit_uint<N>` at every width the compiler
offers. libc++ makes `unsigned _BitInt(N)` integral, and its `<bit>` answers each width
correctly, so there the standard function is the one called. Where `<bit>` refuses the type,
as libstdc++'s does, an overload over `__builtin_clzg`, `__builtin_ctzg` and
`__builtin_popcountg` answers instead, the first two given `N` as the result for zero. That
overload is constrained on `<bit>` refusing the width and is more specialized than the word
template, so exactly one of the three is chosen, and it is declared only where
`XSTD_HAS_BIT_INT` is defined and `__has_builtin` reports the builtin; without it, the word
template answers. GCC has no `_BitInt` in C++, so there it declares nothing. The other
functions build on these three or on the word template, and take every `bit_uint<N>` width
the same way: `byteswap` takes a width that is a whole number of bytes, so `bit_uint<24>`
but not `bit_uint<13>`.

`shl` and `shr` are [P3793](https://github.com/cplusplus/papers/issues/2399)'s shifts, which C++29's `<bit>` adds: `x` times two to the power of
`s`, or of `-s`, rounded towards negative infinity and reduced to the width, so every count is
defined, one at or past the width included, and a negative count shifts the other way. Both
operands may be any `integer`, signed or unsigned and of different types. Where the standard
library declares them, as `__cpp_lib_bitops` of at least `202606L` says, xstd forwards to them
for every pair of types it takes both of them for; elsewhere one template answers, shifting
through the unsigned counterpart, where nothing overflows, and complementing a negative value
on either side of a right shift, so that it floors.

`bit_reverse`, `bit_repeat`, `bit_compress` and `bit_expand` are [P3104](https://github.com/cplusplus/papers/issues/1768)'s bit
permutations, which C++29's `<bit>` adds. Where the standard library declares them, as
`__cpp_lib_bitops` of at least `202607L` says, xstd forwards to them for every type they take,
with one exception: libstdc++'s `bit_compress` and `bit_expand` use neither `PEXT` nor `PDEP`,
so where xstd would, it keeps its own kernels, and a nightly canary reports when that changes.
Elsewhere one template answers every `unsigned_integer`, the 128-bit integer classes and
every `bit_uint<N>` width included, by working through it one `std::size_t` word at a time.
All four are `constexpr`, portable in a constant expression and fast at run time: Clang's
`__builtin_bitreverse64` mirrors a word, and with BMI2 on x86-64 (`-mbmi2`, or MSVC's
`/arch:AVX2`) `bit_compress` and `bit_expand` compile to `PEXT` and `PDEP`. That choice is made
at compile time, with no run-time CPU dispatch. AMD processors before Zen 3 implement both
instructions in microcode, as uops.info's measurements of
[`PEXT`](https://uops.info/html-instr/PEXT_R64_R64_R64.html) and
[`PDEP`](https://uops.info/html-instr/PDEP_R64_R64_R64.html) on Zen 2 show against Zen 3, so
a build targeting them (`-march=bdver4`, `znver1` or `znver2`) keeps the portable kernels even
though it enables BMI2. Those kernels never branch on a bit of the value: `bit_compress` places
each selected bit by one `popcount` of the mask below it, and `bit_expand` walks the mask's
one-bits. Neither is constant-time in every argument, since `bit_compress` runs once per selected
bit and `bit_expand` once per mask bit; design.md records, kernel by kernel, what Valgrind's
memcheck found. `bit_repeat` alone has a precondition, a length above
zero, and is not `noexcept`, as the paper specifies.

`bit_select(x, k)` is an xstd extension beside them, not a standard function and not part of
[P3104](https://github.com/cplusplus/papers/issues/1768): the position of the `k`-th one bit of `x`, counting from zero at the least significant
end, so `bit_select(0b1011'0100u, 2)` is `5`. It is the inverse of rank, since exactly `k` ones
of `x` lie below the one it finds. It takes every `unsigned_integer`, an `int` rank and returns
an `int`, as `<bit>`'s counting functions do. A type wider than one `std::size_t` word skips
whole words by their `popcount`; within the word, BMI2 makes it `PDEP`, the lone
bit `1 << k` deposited on the `k`-th one of `x`, then BMI1's `TZCNT` where the build enables it
too, as `-march=x86-64-v3` does, and a zero test otherwise; Vigna's broadword select finds it
elsewhere, a constant expression included. Its precondition is `0 <= k` and `k < popcount(x)`, a call that violates it is not a constant
expression even under `NDEBUG`, and it is not `noexcept`.

If you lint with `clang-tidy` and use any Boost library, `misc-include-cleaner`
will report that nothing provides the Boost names you write, here and anywhere
else you use Boost: no Boost library ships the IWYU pragmas the check reads, and
its public headers reach their declarations through private implementation ones.
The answer is one line in your own `.clang-tidy`, which no library can supply for
you:

```yaml
CheckOptions:
  misc-include-cleaner.IgnoreHeaders: 'boost/.*'
```

## Requirements


- A conforming [C++23](https://wg21.link/N4950) compiler
- CMake 3.28 or later when using the supplied CMake project
- No third-party runtime or library dependencies outside `<xstd/ints/ext/>`

`<xstd/ints/format.hpp>` is the only header that depends on `<format>`. The front
door `<xstd/ints.hpp>` exports it along with everything else, so a translation unit
that wants no `<format>` names the domain umbrellas it uses instead: none of those
reaches it. The `<xstd/ints/ext/>` headers are the only ones that include a third-party
library, and no other header includes them: an adapted library is reached by naming
it.

## Installation


The usual approach is CMake's `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
    xstd-ints
    GIT_REPOSITORY https://github.com/rhalbersma/xstd-ints.git
    GIT_TAG main # Prefer a release tag for reproducible builds.
)
FetchContent_MakeAvailable(xstd-ints)

target_link_libraries(my_target PRIVATE xstd::ints)
```

`find_package(xstd-ints CONFIG REQUIRED)` and `add_subdirectory(external/xstd-ints)` provide
the same `xstd::ints` target.

## Continuous Integration


We continuously test the stable, qualification, and development branches of the
major [C++23](https://wg21.link/N4950) toolchains (compilers and standard
libraries) in both Debug and Release mode:

| Platform | Compiler   | Standard Library | Stable                    | Qualification             | Development                    | Status |
| :------- | :--------- | :--------------- | :------------------------ | :------------------------ | :----------------------------- | :----- |
| Linux    | GCC        | libstdc++        | 15                        | 16                        | 17-SVN                         | [![GCC](https://github.com/rhalbersma/xstd-ints/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/gcc.yml) |
| Windows  | MinGW      | libstdc++        | 15                        | 16                        | —                              | [![MinGW](https://github.com/rhalbersma/xstd-ints/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/mingw.yml) |
| Linux    | Clang      | libstdc++        | 22 (libstdc++ 15)         | 23 (libstdc++ 16)         | 24-SVN (libstdc++ 17-SVN)      | [![Clang](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang.yml) |
| Linux    | Clang      | libc++           | 22                        | 23                        | 24-SVN                         | [![Clang-libc++](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-libc%2B%2B.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-libc%2B%2B.yml) |
| macOS    | Apple Clang | libc++          | 17.0.0 (Xcode 16.4)       | 21.0.0 (Xcode 26.6)       | —                              | [![Apple Clang](https://github.com/rhalbersma/xstd-ints/actions/workflows/apple-clang.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/apple-clang.yml) |
| Windows  | Clang-CL   | MSVC             | 19.1.5 (VS 2022)          | 20.1.8 (VS 2026)          | 20.1.8 (VS 2026-Preview)       | [![Clang-CL](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-cl.yml) |
| Windows  | MSVC       | MSVC             | 2022 (17.11+)             | 2026                      | 2026-Preview                   | [![MSVC](https://github.com/rhalbersma/xstd-ints/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/msvc.yml) |
| Web      | Emscripten | libc++           | 6.0.12                    | —                         | —                              | [![Emscripten](https://github.com/rhalbersma/xstd-ints/actions/workflows/emscripten.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/emscripten.yml) |

The `Web` row runs one Emscripten release on the stable rung, in Debug: wasm32 under node,
where `size_t` is 32 bits but 64-bit integers keep their 8-byte alignment. The GCC workflow
adds a second 32-bit leg, i686 with GCC `-m32`, where `size_t` is 32 bits and there is no
`__int128`. The MSVC workflow adds a third, x86 with VS 2026 in Debug and Release, where
`xstd::uint128` is the MSVC STL's portable 128-bit class and the bit permutations take their
portable kernels. One more GCC leg runs the trunk snapshot in C++29 mode,
`-std=gnu++2d`, the one mode in which libstdc++ declares `shl` and `shr`: it is where the
overloads forwarding to `<bit>` compile and their agreement tests run.

## License


<pre>
         Copyright Rein Halbersma 2014-2026.
Distributed under the <a href="http://www.boost.org/users/license.html">Boost Software License, Version 1.0</a>.
   (See accompanying file LICENSE_1_0.txt or copy at
         <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
</pre>
