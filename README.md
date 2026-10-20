# Integer extensions to the C++ Standard Library

> "Go to the ant, thou sluggard; consider her ways, and be wise."
>
> Proverbs 6:6, King James Version

<!-- two epigraphs, not one -->

> "When in doubt, do as the ints do."
>
> Scott Meyers, Effective C++, 3rd edition, Item 18

[![Project Status: Active](https://www.repostatus.org/badges/latest/active.svg)](https://www.repostatus.org/#active)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/rhalbersma/xstd-ints/badge)](https://scorecard.dev/viewer/?uri=github.com/rhalbersma/xstd-ints)

xstd-ints is a small, portable, header-only collection of integer extensions to
the C++ standard library: concepts, traits, limits, division, alignment and
character conversion, open to integer-class types where the standard is closed
over the built-ins. It relies on the [C++23](https://wg21.link/N4950) standard
and targets the draft [C++29](https://wg21.link/std) standard library. All
public APIs are in namespace `xstd`.

## Do as the ints do

Abseil's `absl::int128` and Boost.Int128's `boost::int128::int128`, and their
unsigned halves, do as the ints do: they spell out every operator a built-in
integer has, specialize `std::numeric_limits` as integers, and compare with a
strong ordering. The standard library does not return the favour.
`std::integral` rests on `std::is_integral`, which a program may not specialize,
so no class type can satisfy it, and the standard's functions over integers,
such as those of `<bit>` and `<charconv>`, take only the built-in ones. The
standard does know class types that behave as integers, the integer-class types
of [[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc), but
admits only those an implementation supplies itself.

xstd's `integer_class` asks a type that subclause's questions rather than for
its name, so a type that does as the ints do qualifies, and so do the built-in
integers it imitates. Abseil's and Boost's 128-bit types qualify as they are.
`integer` asks one thing more, a signed and unsigned pair, named through xstd's
specializable `make_signed` and `make_unsigned`. Those two sign
transformations are all that `<xstd/ints/ext/absl/int128.hpp>` and
`<xstd/ints/ext/boost/int128.hpp>` adapt.

## Headers

| Header | Description |
| :----- | :---------- |
| `<xstd/ints.hpp>` | Every group below but the `ext/` adaptors, through one include. |
| `<xstd/ints/bit.hpp>` | The `<bit>` functions, widened to every unsigned integer with the 128-bit integer classes and `_BitInt` included, with C++29's bit permutations and shifts by any count. |
| `<xstd/ints/charconv.hpp>` | `std::to_chars`, widened to the integers it does not cover, and the buffer size that holds any value at any base. |
| `<xstd/ints/concepts.hpp>` | Open counterparts of `std::integral` and its signed and unsigned halves that admit integer-class types, with the bitmask, alignment and `noexcept`-operator concepts. |
| `<xstd/ints/cstdint.hpp>` | `bit_int<N>`, `bit_uint<N>`, `int128` and `uint128`: the platform's bit-precise and 128-bit integers, where it has them. |
| `<xstd/ints/cstdlib.hpp>` | `abs`, `unsigned_abs`, `sign`, and truncating, floored, ceiling and Euclidean division of any integer. |
| `<xstd/ints/format.hpp>` | A `std::formatter` that writes a division's quotient and remainder. |
| `<xstd/ints/limits.hpp>` | An open `std::numeric_limits`, specialized for the bit-precise integers. |
| `<xstd/ints/memory.hpp>` | Rounding a value up or down to a power-of-two alignment, and testing whether it is aligned. |
| `<xstd/ints/type_traits.hpp>` | Open sign traits and sign transformations, which users may specialize, and the type an integer's own operators yield. |
| `<xstd/ints/ext/absl.hpp>` | Pairs Abseil's `int128` and `uint128`, so that each models `integer`. |
| `<xstd/ints/ext/boost.hpp>` | Pairs Boost.Int128's 128-bit integers, and hashes any integer's value into a Boost.Hash2 algorithm. |

[The reference](doc/reference/index.md) lists each group's headers and specifies each entity, and [the design notes](doc/design.md) give the rationale, the
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
- CMake 3.30 or later when using the supplied CMake project
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

Beyond the compilers, CI holds coverage at 100%, consumes the library through `find_package`, `add_subdirectory` and
`FetchContent`, runs the sanitizers, analyzes the code with Clang-Tidy, MSVC's `/analyze` and CodeQL, and builds the
documentation site:

[![Coverage](https://codecov.io/gh/rhalbersma/xstd-ints/branch/main/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd-ints)
[![Consumption](https://github.com/rhalbersma/xstd-ints/actions/workflows/consumption.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/consumption.yml)
[![Sanitizers](https://github.com/rhalbersma/xstd-ints/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/sanitizers.yml)
[![Clang-Tidy](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-tidy.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/clang-tidy.yml)
[![MSVC-Analyze](https://github.com/rhalbersma/xstd-ints/actions/workflows/msvc-analyze.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/msvc-analyze.yml)
[![CodeQL](https://github.com/rhalbersma/xstd-ints/actions/workflows/codeql.yml/badge.svg)](https://github.com/rhalbersma/xstd-ints/actions/workflows/codeql.yml)
[![Documentation](https://github.com/rhalbersma/xstd-ints/actions/workflows/pages.yml/badge.svg)](https://rhalbersma.github.io/xstd-ints/)

## License


<pre>
         Copyright Rein Halbersma 2014-2026.
Distributed under the <a href="http://www.boost.org/users/license.html">Boost Software License, Version 1.0</a>.
   (See accompanying file LICENSE_1_0.txt or copy at
         <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
</pre>
