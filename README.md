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

## Headers

| Header | Description |
| :----- | :---------- |
| `<xstd/ints.hpp>` | Every header below but the `ext/` adaptors, through one include. |
| `<xstd/ints/bit.hpp>` | The `<bit>` functions below, through one include. |
| `<xstd/ints/bit/bit_ceil.hpp>` | `std::bit_ceil`, widened to every unsigned integer, the 128-bit integer classes and `_BitInt` included. |
| `<xstd/ints/bit/bit_compress.hpp>` | Gathers the bits a mask selects into the low bits, as x86's `PEXT` does. |
| `<xstd/ints/bit/bit_expand.hpp>` | Scatters the low bits to the positions a mask selects, as x86's `PDEP` does. |
| `<xstd/ints/bit/bit_floor.hpp>` | `std::bit_floor`, widened to every unsigned integer, the 128-bit integer classes and `_BitInt` included. |
| `<xstd/ints/bit/bit_repeat.hpp>` | Repeats the low `l` bits across the whole width. |
| `<xstd/ints/bit/bit_reverse.hpp>` | Mirrors the bits of any unsigned integer. |
| `<xstd/ints/bit/bit_select.hpp>` | The position of the `k`-th one bit, the inverse of rank. |
| `<xstd/ints/bit/bit_width.hpp>` | `std::bit_width`, widened to every unsigned integer, the 128-bit integer classes and `_BitInt` included. |
| `<xstd/ints/bit/byteswap.hpp>` | `std::byteswap`, widened to every integer whose width is a whole number of bytes. |
| `<xstd/ints/bit/countl_one.hpp>` | `std::countl_one`, widened to every unsigned integer, the 128-bit integer classes and `_BitInt` included. |
| `<xstd/ints/bit/countl_zero.hpp>` | `std::countl_zero`, widened to every unsigned integer, the 128-bit integer classes and `_BitInt` included. |
| `<xstd/ints/bit/countr_one.hpp>` | `std::countr_one`, widened to every unsigned integer, the 128-bit integer classes and `_BitInt` included. |
| `<xstd/ints/bit/countr_zero.hpp>` | `std::countr_zero`, widened to every unsigned integer, the 128-bit integer classes and `_BitInt` included. |
| `<xstd/ints/bit/has_single_bit.hpp>` | `std::has_single_bit`, widened to every unsigned integer, the 128-bit integer classes and `_BitInt` included. |
| `<xstd/ints/bit/popcount.hpp>` | `std::popcount`, widened to every unsigned integer, the 128-bit integer classes and `_BitInt` included. |
| `<xstd/ints/bit/rotl.hpp>` | `std::rotl`, widened to every unsigned integer, the 128-bit integer classes and `_BitInt` included. |
| `<xstd/ints/bit/rotr.hpp>` | `std::rotr`, widened to every unsigned integer, the 128-bit integer classes and `_BitInt` included. |
| `<xstd/ints/bit/shl.hpp>` | Shifts left by any count of any integer type, a negative count shifting right, never undefined. |
| `<xstd/ints/bit/shr.hpp>` | Shifts right the same way, rounding towards negative infinity. |
| `<xstd/ints/charconv.hpp>` | `to_chars` and `to_chars_max_size`, through one include. |
| `<xstd/ints/charconv/to_chars.hpp>` | `std::to_chars`, widened to the integers it does not cover, and the buffer size that holds any value at any base. |
| `<xstd/ints/concepts.hpp>` | The concepts below, through one include. |
| `<xstd/ints/concepts/alignable.hpp>` | What the alignment functions ask of a type, and whether its operations are `noexcept`. |
| `<xstd/ints/concepts/bit_mask.hpp>` | The standard's bitmask type: an unsigned integer, or a type with the bitmask operators, such as `std::bitset`. |
| `<xstd/ints/concepts/integer.hpp>` | An integer as [P3701](https://github.com/cplusplus/papers/issues/2330) bounds them, extended to paired integer-class types. |
| `<xstd/ints/concepts/integer_class.hpp>` | The operations [[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc) states of an integer-class type. |
| `<xstd/ints/concepts/nothrow_const_operators.hpp>` | Whether a type's operators are `noexcept`, which sets the integer functions' exception specifications. |
| `<xstd/ints/concepts/signed_integer.hpp>` | An open `std::signed_integral`, which admits signed integer-class types. |
| `<xstd/ints/concepts/unsigned_integer.hpp>` | An open `std::unsigned_integral`, which admits unsigned integer-class types. |
| `<xstd/ints/cstdint.hpp>` | The integer aliases below, through one include. |
| `<xstd/ints/cstdint/bit_int.hpp>` | `bit_int<N>` and `bit_uint<N>`, the compiler's bit-precise integers where it has them, and their widest width. |
| `<xstd/ints/cstdint/int128.hpp>` | `int128` and `uint128`, the platform's 128-bit integers where it has them. |
| `<xstd/ints/cstdlib.hpp>` | The integer functions below, through one include. |
| `<xstd/ints/cstdlib/abs.hpp>` | `std::abs`, as a `constexpr` function over every integer. |
| `<xstd/ints/cstdlib/div.hpp>` | Truncating division of any integer. |
| `<xstd/ints/cstdlib/div_ceil.hpp>` | Ceiling division. |
| `<xstd/ints/cstdlib/div_euclid.hpp>` | Euclidean division. |
| `<xstd/ints/cstdlib/div_floor.hpp>` | Floored division. |
| `<xstd/ints/cstdlib/div_result.hpp>` | The quotient and remainder the four divisions return. |
| `<xstd/ints/cstdlib/sign.hpp>` | `-1`, `0` or `1`, by the sign of an integer. |
| `<xstd/ints/cstdlib/unsigned_abs.hpp>` | The absolute value as the unsigned counterpart, defined for every argument. |
| `<xstd/ints/ext/absl.hpp>` | The Abseil adaptor below, through one include. |
| `<xstd/ints/ext/absl/int128.hpp>` | Pairs Abseil's `int128` and `uint128`, so that each models `integer`. |
| `<xstd/ints/ext/boost.hpp>` | The Boost adaptors below, through one include. |
| `<xstd/ints/ext/boost/hash2.hpp>` | `hash_append_int`, which hashes any integer's value, never its object representation, into a Boost.Hash2 algorithm. |
| `<xstd/ints/ext/boost/int128.hpp>` | Pairs Boost.Int128's `int128` and `uint128`, so that each models `integer`. |
| `<xstd/ints/format.hpp>` | The formatter below, through one include. |
| `<xstd/ints/format/div_result.hpp>` | A `std::formatter` that writes a `div_result` as `(quotient, remainder)`. |
| `<xstd/ints/limits.hpp>` | `numeric_limits`, through one include. |
| `<xstd/ints/limits/numeric_limits.hpp>` | An open `std::numeric_limits`, with specializations for the bit-precise `_BitInt(N)` types. |
| `<xstd/ints/memory.hpp>` | The alignment functions below, through one include. |
| `<xstd/ints/memory/align_down.hpp>` | Rounds a value down to a power-of-two alignment. |
| `<xstd/ints/memory/align_up.hpp>` | Rounds a value up to a power-of-two alignment. |
| `<xstd/ints/memory/is_aligned.hpp>` | Whether a value is a multiple of a power-of-two alignment. |
| `<xstd/ints/type_traits.hpp>` | The traits below, through one include. |
| `<xstd/ints/type_traits/is_signed.hpp>` | `std::is_signed`, opened to integer-class types. |
| `<xstd/ints/type_traits/is_unsigned.hpp>` | `std::is_unsigned`, opened to integer-class types. |
| `<xstd/ints/type_traits/make_signed.hpp>` | An open `std::make_signed`, which users may specialize. |
| `<xstd/ints/type_traits/make_unsigned.hpp>` | An open `std::make_unsigned`, which users may specialize. |
| `<xstd/ints/type_traits/promoted.hpp>` | `promoted_t`, the type a type's own operators yield: [[conv.prom]](https://eel.is/c++draft/conv.prom) for a built-in, the type itself otherwise. |

[The reference](doc/reference.md) specifies each entity, and [the design notes](doc/design.md) give the rationale, the
measurements, and how to customize an integer-class type. [CONTRIBUTING.md](CONTRIBUTING.md) builds the library itself.

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

If you lint with `clang-tidy` and use any Boost library, `misc-include-cleaner` reports that nothing provides the Boost
names you write: no Boost library ships the IWYU pragmas the check reads. The answer is one line in your own
`.clang-tidy`, which no library can supply for you:

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
