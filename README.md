# Do as the ints do

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
[![Coverage](https://codecov.io/gh/rhalbersma/xstd-ints/branch/main/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd-ints)
[![Consumption](https://github.com/rhalbersma/xstd-ints/actions/workflows/consumption.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/consumption.yml)
[![Sanitizers](https://github.com/rhalbersma/xstd-ints/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/sanitizers.yml)
[![Clang-Tidy](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-tidy.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-tidy.yml)
[![MSVC-Analyze](https://github.com/rhalbersma/xstd-ints/actions/workflows/msvc-analyze.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/msvc-analyze.yml)
[![CodeQL](https://github.com/rhalbersma/xstd-ints/actions/workflows/codeql.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/codeql.yml)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/rhalbersma/xstd-ints/badge)](https://scorecard.dev/viewer/?uri=github.com/rhalbersma/xstd-ints)

xstd-ints is a small, portable, header-only collection of integer extensions to
the C++ standard library: concepts, traits, limits, division, alignment and
character conversion, open to integer-class types where the standard is closed
over the built-ins. It relies on the [C++23](https://wg21.link/N4950) standard
and targets the draft [C++29](https://wg21.link/std) standard library. All
public APIs are in namespace `xstd`.

## Usage


```cpp
#include <climits>
#include <xstd/ints/cstdlib.hpp>
#include <xstd/ints/memory.hpp>

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

static_assert(xstd::signed_integer<xstd::int128>);
static_assert(xstd::unsigned_integer<xstd::uint128>);

#ifdef XSTD_HAS_BIT_INT
static_assert(xstd::signed_integer<xstd::bit_int<24>>);
static_assert(xstd::numeric_limits<xstd::bit_uint<24>>::digits == 24);
#endif
```

The `integer` concept and its signedness refinements are cv-transparent. The
integer functions return the argument type rather than a promoted one. Following
[P3701R0](https://wg21.link/P3701R0), `bool` and the five character types
`char`, `wchar_t`, `char8_t`, `char16_t`, and `char32_t` are excluded. `signed
char` and `unsigned char` remain integers, preserving exact-width 8-bit aliases.
Nothing is excluded by name: `integer_class` admits the built-in integers, so
`integer` is defined over it plus the signedness behaviour every integer has and
no character type does. An integer is one of its own signed/unsigned pair, and
its two transformations compose back; a character type is a third type beside
its pair, `make_signed_t<char>` being `signed char`. `bool` never reaches the
question, C++17 having removed the `++` that `integer_class` asks for. xstd
extends the paper's built-in boundary with paired `integer_class` types.
Character conversion delegates the standard integral domain directly to
`std::to_chars`.

`div_result` is the common quotient/remainder result for truncating `div`,
Euclidean `div_euclid`, and floored `div_floor`. Their vocabulary and relevant
rounding semantics follow [P3724R4](https://wg21.link/P3724R4), but xstd
intentionally implements only this established three-operation family.
A `div_result` renders as `(quotient, remainder)` for every element type it accepts, but which
format specs it accepts depends on that type: where the standard library can
format a tuple of it the tuple grammar applies, and otherwise a string one.

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

auto const value = xstd::numeric_limits<xstd::uint128>::max();
auto buffer = std::array<char, xstd::to_chars_max_size<xstd::uint128>>{};
auto const [ptr, ec] = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), value, 16);
auto const text = std::string_view(buffer.data(), ptr); // thirty-two f's
```

An `<xstd/ext/>` header pairs a third-party integer class with its counterpart,
which is all the concepts and the functions were waiting for. It brings the
library it adapts with it, so naming it is enough:

```cpp
#include <xstd/ints/concepts/signed_integer.hpp>
#include <xstd/ints/cstdlib.hpp>
#include <xstd/ints/ext/boost/int128.hpp> // brings boost::int128 with it

using i128 = boost::int128::int128;
static_assert(xstd::signed_integer<i128>);

auto const [quotient, remainder] = xstd::div_floor(i128{-8}, i128{3});
```

See [the design notes](doc/design.md) for rationale and for customizing an
integer-class type, and [CONTRIBUTING.md](CONTRIBUTING.md) to build the library itself.

## Headers


| Header | Additions | Description | Reference |
| :----- | :-------- | :---------- | :-------- |
| `<xstd/ints/concepts/integer_class.hpp>` <br> `<xstd/ints/concepts/integer.hpp>` <br> `<xstd/ints/concepts/signed_integer.hpp>` <br> `<xstd/ints/concepts/unsigned_integer.hpp>` <br> `<xstd/ints/concepts/bitwise_operators.hpp>` <br> `<xstd/ints/concepts/alignable.hpp>` <br> `<xstd/ints/concepts/nothrow_const_operators.hpp>` | `integer_class` <br> `integer` <br> `signed_integer` <br> `unsigned_integer` <br> `bitwise_operators` <br> `alignable`, `nothrow_alignable` <br> `nothrow_const_operators` | The operations [iterator.concept.winc] states of an integer-class type <br> P3701 arithmetic domain, extended to paired integer-class types <br> Open form of `std::signed_integral` <br> Open form of `std::unsigned_integral` <br> `integer_class` pruned to the bitwise half, which `std::bitset` and the bit containers model too <br> What the alignment functions ask of a type, and whether its operations carry `noexcept` <br> Exception specification of the integer functions | [iterator.concept.winc] (integer-class type) <br> [P3701R0](https://wg21.link/P3701R0), [iterator.concept.winc] <br> [iterator.concept.winc] (integer-class types) <br> [iterator.concept.winc] (integer-class types) <br> [template.bitset] <br> none <br> none |
| `<xstd/ints/bit.hpp>` | `countl_zero` <br> `countr_zero` <br> `popcount` | `std::countl_zero`, widened to the 128-bit integer classes `<bit>` declines <br> `std::countr_zero`, the same <br> `std::popcount`, the same | [bit.count] <br> [bit.count] <br> [bit.pop] |
| `<xstd/ints/charconv.hpp>` | `to_chars` <br> `to_chars_max_size` | `std::to_chars`, widened to xstd integers it does not cover <br> Buffer size that holds any value of `T` at any base | [charconv.to.chars] <br> none |
| `<xstd/ints/cstdint.hpp>` | `bit_int<N>` <br> `bit_uint<N>` <br> `bit_int_max_width` <br> `int128` <br> `uint128` | Native bit-precise signed integer (when available) <br> Native bit-precise unsigned integer (when available) <br> Maximum native bit-precise width (when available) <br> Platform 128-bit signed integer <br> Platform 128-bit unsigned integer | [P3666R0](https://wg21.link/P3666R0) <br> [P3666R0](https://wg21.link/P3666R0) <br> none <br> none <br> none |
| `<xstd/ints/cstdlib.hpp>` | `div_result` <br> `sign` <br> `abs` <br> `unsigned_abs` <br> `div` <br> `div_euclid` <br> `div_floor` | Defaulted equality comparison <br> `-1`, `0`, or `1`; `0` or `1` when unsigned <br> `constexpr`, any xstd integer <br> Total `\|x\|`, returning the unsigned counterpart <br> Truncated division, any xstd integer <br> Euclidean division <br> Floored division | none <br> [Boost.Math](https://www.boost.org/doc/libs/1_80_0/libs/math/doc/html/math_toolkit/sign_functions.html) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (reviewed implementation wording) <br> [Rust `unsigned_abs`](https://doc.rust-lang.org/std/primitive.i32.html#method.unsigned_abs) (no C++ equivalent) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (reviewed implementation wording) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) |
| `<xstd/ints/ext/absl.hpp>` | `make_unsigned<absl::int128>` <br> `make_signed<absl::uint128>` | Pairs Abseil's two integer-class types, so each models `integer` | none <br> none |
| `<xstd/ints/ext/boost.hpp>` | `make_unsigned<boost::int128::int128>` <br> `make_signed<boost::int128::uint128>` | Pairs Boost.Int128's two integer-class types, so each models `integer` | none <br> none |
| `<xstd/ints/format.hpp>` | `formatter<div_result>` | `std::format` support for every element type `div_result` accepts | [p3391](https://wg21.link/P3391R3) (reviewed constexpr-format wording) |
| `<xstd/ints/limits.hpp>` | `numeric_limits` | Open `std::numeric_limits`, specialized for xstd extension types | [numeric.limits] |
| `<xstd/ints/memory.hpp>` | `align_up` <br> `align_down` <br> `is_aligned` | Round a value up to a power-of-two alignment, any xstd unsigned integer <br> Round a value down to a power-of-two alignment <br> Whether rounding would change anything | [Boost.Align](https://www.boost.org/doc/libs/release/doc/html/align.html) (`align_up`), [LLVM `alignTo`](https://llvm.org/doxygen/namespacellvm.html) <br> [Boost.Align](https://www.boost.org/doc/libs/release/doc/html/align.html) (`align_down`), [LLVM `alignDown`](https://llvm.org/doxygen/namespacellvm.html) <br> [Boost.Align](https://www.boost.org/doc/libs/release/doc/html/align.html) (`is_aligned`) |
| `<xstd/ints/type_traits/is_signed.hpp>` <br> `<xstd/ints/type_traits/is_unsigned.hpp>` <br> `<xstd/ints/type_traits/make_signed.hpp>` <br> `<xstd/ints/type_traits/make_unsigned.hpp>` <br> `<xstd/ints/type_traits/promoted.hpp>` | `is_signed` <br> `is_unsigned` <br> `make_signed` <br> `make_unsigned` <br> `promoted_t` | `std::is_signed`, opened to integer-class types <br> `std::is_unsigned`, opened to integer-class types <br> Open, user-specializable `std::make_signed` <br> Open, user-specializable `std::make_unsigned` <br> What a type's own operators yield: [conv.prom] for a built-in, the type itself otherwise | none <br> none <br> none <br> none <br> [conv.prom] |

The native bit-precise aliases are available when the compiler defines
`__BITINT_MAXWIDTH__`; `XSTD_HAS_BIT_INT` detects their public API. Their current
minimum width is two so every exposed type has a signed or unsigned counterpart.
The 128-bit aliases and their `make_signed` and `make_unsigned` associations are
defined together in `<xstd/ints/cstdint/int128.hpp>`; `<xstd/ints/cstdint.hpp>` is their
umbrella. The two `<xstd/ext/>` umbrellas export `<xstd/ints/ext/absl/int128.hpp>` and
`<xstd/ints/ext/boost/int128.hpp>`, and each needs the library it adapts on the
include path; nothing above them exports either. Each adapted header is
exported in turn, so including one is enough: `<xstd/ints/ext/absl/int128.hpp>`
brings `absl::int128` and `absl::uint128` with it.

`std::countl_zero`, `std::countr_zero` and `std::popcount` take
`std::unsigned_integral`, which every 128-bit integer *class* fails. Each
`<xstd/ints/bit/>` header is one overload set: the standard function wherever it
accepts the argument, and beside it an overload for a class this library knows.
Where `xstd::uint128` is a class, as it is in the Microsoft STL, that overload
lives in `<xstd/ints/cstdint/int128.hpp>`, and the two `<xstd/ext/>` adaptors
carry one each for the type they adapt. Where it is the built-in `unsigned __int128`,
libstdc++ and libc++ carry it into `std::unsigned_integral` only outside
`__STRICT_ANSI__`, so a strictly conforming translation unit on GCC or Clang has
no bit basis for that one width.

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
- No third-party runtime or library dependencies outside `<xstd/ext/>`

`<xstd/ints/format.hpp>` is the only header that depends on `<format>`. The front
door `<xstd/ints.hpp>` exports it along with everything else, so a translation unit
that wants no `<format>` names the domain umbrellas it uses instead: none of those
reaches it. The `<xstd/ext/>` headers are the only ones that include a third-party
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

## License


<pre>
         Copyright Rein Halbersma 2014-2026.
Distributed under the <a href="http://www.boost.org/users/license.html">Boost Software License, Version 1.0</a>.
   (See accompanying file LICENSE_1_0.txt or copy at
         <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
</pre>
