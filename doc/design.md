# Design notes

## Purpose

xstd is a header-only collection of small C++ standard-library extensions.
There is no single proposal behind it: it collects facilities suggested by
existing practice that can be implemented portably, with a standard-library
interface. The baseline is [C++23](https://wg21.link/N4950), and may move to C++26 once that is common
across the tested toolchains. Consumers need no third-party dependencies.

## Principles

- **Prefer `constexpr`.** Value-oriented functions are usable during constant
  evaluation unless the standard library operation they delegate to prevents it.
- **Generalize integer support.** The standard integral concepts and traits are
  closed over built-in types; xstd's integer concepts also accept paired
  integer-class types, based on their behavior and `xstd::numeric_limits`.
- **Make semantics explicit.** The four division functions name their rounding
  convention rather than hiding it behind `/` and `%`.
- **Stay modular and dependency-free.** Linking `xstd::ints` adds include paths
  and the [C++23](https://wg21.link/N4950) requirement, but no runtime library or transitive package.

## API shape

### Integer types

[P3701R0](https://github.com/cplusplus/papers/issues/2330) supplies the `integer`,
`signed_integer`, and `unsigned_integer` vocabulary and its built-in arithmetic
boundary. The public concept is cv-transparent and accepts standard and extended
integer types except `bool`, `char`, `wchar_t`, `char8_t`, `char16_t`, and `char32_t`.
`signed char` and `unsigned char` remain integers, so `int8_t` and `uint8_t`
remain supported where they are aliases of those types.

Unlike the paper's closed standard-library domain, xstd also admits types that
model `integer_class`, and reaches the built-ins through that concept too: since
it admits them, a `std::integral` disjunct would only restate what it already
grants, and `bool` is excluded by failing it rather than by name. The rest of
the boundary is drawn the same way, by behaviour rather than by a list. Every
accepted type must have valid `make_signed_t` and `make_unsigned_t`
transformations, which rejects an unpaired integer-class type; those
transformations must compose back, each settled by signedness alone; and the
type must be one of its own pair. A character type is not -
`make_signed_t<char>` is `signed char`, a third type beside it - which is what
excludes the five of them without naming any. The refinements use xstd's open
signedness traits, preserving concept subsumption across built-in, extended,
bit-precise, and paired class integers.

Character conversion delegates `char` and a standard integer type to
`std::to_chars` when it is no wider than `uint128`, which is where both standard
libraries stop; every other `integer` takes xstd's own digit generation, and the
other character types are refused.

`integer_class` follows the requirement order in [[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc), but is
an open structural concept rather than an implementation-defined set. It admits
integer types of every width instead of requiring them to be wider than every
integral type, and it deliberately checks only same-type arithmetic rather than
mixed-mode operations or common types. Its range metadata comes from the open
`xstd::numeric_limits`, which delegates to the standard trait for every non-array
object type.

It does not exclude the built-in integers. The subclause's own note says an
integer-class type need not be a class type, so excluding them was xstd's
addition rather than the standard's, and it made the requirements unaskable of
the types they were modelled on instead of merely false for them. The closure
requirements in /7.3 and /7.6 are therefore stated against `promoted_t` rather
than `T`: identity for a class type, and for a built-in whatever [[conv.prom]](https://eel.is/c++draft/conv.prom)
yields, which is what the same requirement means once promotion is in scope.
Asked that way every standard integer satisfies the concept except `bool`, whose
`++` and `--` C++17 removed. `promoted_t` asks the language — `decltype(+a)` —
rather than modelling the rules, and only where the language decides: a
non-integral type is its own, so a user-defined `operator+` cannot redefine its
promoted type into compliance.

That widening moves one boundary rather than removing it. `is_signed`,
`is_unsigned`, `make_signed` and `make_unsigned` each pair a standard-library
specialization with an `integer_class` one, and the two were disjoint only
because the concept excluded built-ins. Each now says `not std::integral` where
it means it — the "where std stops" line belongs to the trait that draws it, not
to the concept.

When the compiler defines `__BITINT_MAXWIDTH__`, as Clang does in C++,
`<xstd/ints/cstdint.hpp>` names the native types as `bit_int<N>` and `bit_uint<N>`.
They are aliases, not wrappers: their ABI, conversions, promotions, and operators
remain the compiler's. xstd specializes
its open limits and transformation traits for them, which makes paired widths
model `integer` even before the standard library recognizes the extension. The
specializations sit in the headers that declare `numeric_limits`, `make_signed` and
`make_unsigned`, not beside the aliases: `_BitInt` is a keyword, so a program can ask
about one having included nothing else, and a translation unit that asked before the
aliases' header would otherwise keep a different answer from one that asked after.
They state every member the width decides, `digits10` included, which libstdc++ leaves
at zero and libc++ takes from `sizeof`; it is `floor(digits * log10(2))` in 64.64
fixed point, exact at every width Clang offers. `unsigned _BitInt(1)` is its own
unsigned counterpart and has no signed one.
They reach it as `integer_class` on either standard library, though only libc++
makes `_BitInt` integral: that choice moves which arm `promoted_t` takes, and
both name `_BitInt` itself, since it is exempt from promotion. The public
domain currently begins at two because Clang has not yet implemented [N3747](https://wg21.link/N3747)'s
signed `_BitInt(1)`.

The `<bit>` counterparts follow the same split. Where the standard library's `<bit>` takes a
type, xstd forwards to it. Every other `unsigned_integer` — an integer class, or an
`unsigned _BitInt(N)` on libstdc++, whose `<bit>` declines it — is answered by one template per function
that works a `std::size_t` block at a time, the block being `std::size_t` because
[[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc) promises every integer-class type an explicit conversion to and from
it, and each block goes to the standard function, which knows its own instructions. The
128-bit classes get no overloads of their own: the template reaches them through their
operators, and being a template declared beside the forwarding overload, it is the same
overload set in every translation unit whatever order the headers come in, so `bit_width`,
written on `countl_zero`, never sees a different `countl_zero` for the same type. For
`countl_zero`, `countr_zero` and `popcount` on `unsigned _BitInt(N)`, a more specialized
overload deduces `N` and calls the compiler's type-generic builtin, passing `N` as the count
for zero so that the builtin is total. It is constrained on the standard call being
ill-formed rather than on the library in use, so it steps aside on its own the day a
standard library learns the type. The builtins are younger than the type: Clang 19 added
them, where `_BitInt` reached C++ in Clang 14, so that overload sits behind `__has_builtin`,
and the block template answers where it is absent. `byteswap` alone also takes signed types,
as `std::byteswap` does, swapping through the unsigned counterpart, and asks for a whole
number of bytes rather than for the absence of padding bits that `std::byteswap` mandates,
since it swaps the value and not the object representation.

The four permutations from [P3104R6](https://github.com/cplusplus/papers/issues/1768) — `bit_reverse`,
`bit_repeat`, `bit_compress` and `bit_expand` — have no `<bit>` to defer to yet, so each is
one template over `unsigned_integer` beside an overload forwarding to the standard function,
declared once `__cpp_lib_bitops` reaches the working draft's `202607L` and constrained, like
the counters, on the standard call being well-formed. `bit_compress` and `bit_expand` are not
forwarded to libstdc++ where xstd has fast BMI2: GCC trunk's implementation is Hacker's Delight's
parallel suffix for every type, about 28 ns a call on a Broadwell Xeon against 0.6 ns for
`PEXT`, so xstd keeps its kernels, and the Toolchain Canary's `libstdcxx_pext` job fails the week
GCC trunk starts emitting either instruction. The template works a block at a time.
A block is mirrored, compressed or expanded by one of two families of kernels on a single
`std::size_t`, each family in a directory of its own under `bit/detail/` with one header per
kernel:

- `detail::intrinsic` holds `block_reverse`, which is Clang's `__builtin_bitreverse32` and
  `__builtin_bitreverse64` where `XSTD_INTS_HAS_BUILTIN_BITREVERSE` says the compiler has them,
  and like them a constant expression. Elsewhere it is one instruction where the target has one,
  each detected by a header of its own as `bmi2.hpp` detects BMI2: ACLE's `__rbit` and `__rbitll`,
  one `RBIT`, where `XSTD_INTS_HAS_RBIT` says the target is AArch64 with `<arm_acle.h>`, which
  brings GCC on AArch64 the instruction Clang reaches through its builtin; or, where
  `XSTD_INTS_HAS_GFNI` says the target is x86-64 with `__GFNI__`, one `GF2P8AFFINEQB` against the
  matrix `0x8040201008040201`, which mirrors the bits of every byte, and a byte swap. llvm-mca puts
  that at 1.4 cycles a block on Ice Lake against 9.0 for GCC 15's code for the portable kernel, as
  [`bench/mca`](../bench/mca/README.md) records; Clang emits the same two instructions for the
  portable kernel unasked, with the same matrix. MSVC
  exposes no `RBIT` intrinsic and defines no `__GFNI__`, so it keeps the portable kernel. It also holds `block_compress` and
  `block_expand`, which are `_pext_u64` and `_pdep_u64`,
  and `block_select`, which is `std::countr_zero(_pdep_u64(1 << k, x))`, all three declared only
  where `XSTD_INTS_HAS_FAST_BMI2` says the target makes them fast: x86-64 with `__BMI2__`, or
  MSVC with `/arch:AVX2`, which implies BMI2 but never defines the macro. A target of `bdver4`,
  `znver1` or `znver2` is excluded even though it enables BMI2, since `PEXT` and `PDEP` are
  microcoded there, as uops.info's measurements of
  [`PEXT (R64, R64, R64)`](https://uops.info/html-instr/PEXT_R64_R64_R64.html) and
  [`PDEP (R64, R64, R64)`](https://uops.info/html-instr/PDEP_R64_R64_R64.html) on Zen+ and Zen 2
  show against Zen 3 and Haswell.
- `detail::portable` holds the `constexpr` kernels, which a constant expression always takes
  and a run-time call takes wherever the intrinsics are absent. `block_reverse_by_swaps` mirrors
  the bytes by `std::byteswap`, then each byte's nibbles, pairs and single bits by three masked
  swaps. `block_compress_by_rank` loops over
  the selected bits, placing each at its rank among the mask's ones by one `popcount`;
  `block_expand_by_walk` loops over the mask's one-bits, depositing the next bit of the value by a
  mask rather than a branch; `block_expand_by_select` loops over the value's one-bits instead,
  placing each on the mask's corresponding one by `block_select_by_broadword`, which suits a
  sparse value over a wide mask. `block_select_by_broadword` is Vigna's broadword select, from
  "Broadword Implementation of Rank/Select Queries" (WEA 2008): three shift-and-mask steps count
  the ones of every byte, one multiply sums those counts into a prefix count per byte, one
  parallel compare against `k` copied into every byte, counted by a `popcount`, finds the byte
  holding the `k`-th one, and a 2 KiB table indexed by that byte and the rank left over gives
  the bit within it. It never loops and never branches on the value.

`block_reverse`, `block_compress`, `block_expand` and `block_select`, each in a header of its own, pick between the two:
`block_reverse` takes the builtin wherever it is declared, in a constant expression too, and `RBIT` or `GFNI` outside a constant expression
where those are; the other three take the intrinsic outside a constant
expression where it is declared, the portable kernel otherwise. [`bench/`](../bench) times each
kernel and the alternatives it beat, in cycles per independent call on a 64-bit block of stated
density, the harness that feeds each call included. [`bench/results/`](../bench/results) keeps
each run with the machine, compiler, flags and commit it names, and
[CONTRIBUTING.md](../CONTRIBUTING.md#benchmarks) gives the command; besides this container's
run it holds the Bench workflow's on an [Emerald Rapids](../bench/results/emerald-rapids.md)
and a [Neoverse N2](../bench/results/neoverse-n2.md) runner. From the
[Cascade Lake run](../bench/results/cascade-lake.md), at `-march=native -O3`, with a cost per bit
taken as the slope from the sparsest row of its table to the densest:

| Kernel | Cost | GCC 15 | Clang 22 |
| :-- | :-- | --: | --: |
| `PEXT`, `PDEP` | per call | 1.7 | 1.7 to 2.0 |
| `block_compress_by_rank` | per selected bit | 3.2 | 2.5 |
| `block_expand_by_walk` | per mask bit | 3.0 | 2.5 |
| `block_expand_by_select` | per source bit | 7.4 | 7.0 |
| `std::countr_zero` of `PDEP` | per call | 3.0 | 2.0 |
| `block_select_by_broadword` | per call | 12.1 | 11.7 |
| the harness alone | per call | 2.1 | 1.8 |

With two source ones, `block_expand_by_select` beats the walk from about nine mask ones up. The
alternatives measured and left out cost, on the same run: the mask walk with an `if` on each bit,
which both compilers turn into a conditional move rather than a branch, 1.75 times the rank kernel
to compress at half density, since it pays for every mask bit rather than every selected one, and
to expand as much as the masked walk with Clang and less with GCC, 2.1 cycles a mask bit against
3.0; Hacker's Delight's parallel suffix, a flat 70 cycles to compress and 77 to expand; and
Wegner's zp7, a flat 52 to 58 and 61 to 66 without `PCLMULQDQ`, and 25 to 26 and 34 with it.
Tests check every portable kernel against a
bit-at-a-time oracle at compile time and at run time, and the run-time block kernels against
the portable ones, so a build with BMI2 checks the instructions too. Agreement cannot show which
kernel ran, so codegen tests compile the public functions to assembly for each target that
selects a kernel and check the instructions that come out, and that they are absent where the
target is excluded. Beyond what tests can sample, Z3 proves each portable kernel equal to its
specification for every input at 32 and 64 bits: PEXT and PDEP as the Intel SDM's pseudocode
gives them, RBIT as the Arm ARM's ASL does, and select by its defining property, each shift
within the width and the select's precondition met at every call.
Branch-free is a claim about control flow; whether a kernel's control flow or memory addresses
depend on secret data was checked separately, per kernel and per secret argument
([`constant_time/`](../constant_time), [results](../constant_time/results.md)). Valgrind's memcheck,
with the secret marked undefined, reports every branch and address computed from it, the same for
GCC 15 and Clang 22 with and without `-march=x86-64-v3`:

| Kernel | Secret | Memcheck |
| :-- | :-- | :-- |
| `block_reverse_by_swaps` | the word | clean |
| `block_expand_by_walk` | the source, the mask public | clean |
| `block_expand_by_walk` | the mask | a branch: one trip per mask one |
| `block_compress_by_rank` | the source | a branch: one trip per selected bit |
| `block_expand_by_select` | the source | a branch, and the table lookup |
| `block_select_by_broadword` | the word and the rank | no branch, but one address from them: `select_in_byte` |
| `PEXT`, `PDEP` | the source or the mask | clean |

So only the byte reversal, the walk over a public mask, and the two instructions are free of
secret-dependent branches and addresses. The select never branches, yet indexes a table by the
secret, which a cache-timing attacker can observe, and the loops branch on no bit of the value but
run a number of times that depends on it. Memcheck says nothing about time itself: the instructions'
own latency can depend on their operands, as uops.info measures for
[`PEXT (R64, R64, R64)`](https://uops.info/html-instr/PEXT_R64_R64_R64.html) on Zen 2.
Wider types compose the blocks: reversal sends each mirrored block to the mirror of its offset,
compression packs each block's result above the bits the lower blocks selected, expansion
draws each block's bits from where the lower blocks stopped, and selection skips whole blocks,
subtracting each one's population from `k`, until the `k`-th one lies in the current block,
where the block kernel finds it and the block's offset is added. Shift counts are `unsigned`
for a built-in type, so no count is signed, and `int` for a class type, the one amount every
integer class in the test matrix takes without a narrowing conversion.

[P3793R2](https://github.com/cplusplus/papers/issues/2399)'s `shl` and `shr` are forwarded the same way, once `__cpp_lib_bitops` reaches the
`202606L` that libstdc++ gives them in C++29 mode, one below the value [P3104](https://github.com/cplusplus/papers/issues/1768) asks for. Their
template takes any two `integer` types and reduces every count to three cases: a magnitude at
or past the width gives zero, or minus one for a negative value shifted right; any other
magnitude is a single shift. The magnitude is the count's `unsigned_abs`, so the most negative
count of a signed type needs no negation, and it is compared with the width in whichever of
the two types holds the other, so a 256-bit width is not truncated into an `unsigned char`
count. A left shift goes through the unsigned counterpart, which keeps a signed value from
overflowing, and the result converts back modulo the width, as C++20 defines the conversion.

`bit_select(x, k)` is xstd's own, beside the four: it is not a standard function and no part of
[P3104](https://github.com/cplusplus/papers/issues/1768), so it has no `<bit>` overload to forward to and is one template over `unsigned_integer`.
It answers the position of the `k`-th one bit of `x`, counting from zero at the least
significant end, which makes it the inverse of rank: `popcount(x & ((T{1} << bit_select(x, k)) - 1)) == k`
for every `k` below `popcount(x)`. Its signature reads as `<bit>`'s do: `x` taken as the other
functions take it, `k` an `int` as `bit_repeat`'s length and `rotl`'s count are, and an `int`
result as `countr_zero` returns. Its one-block kernel under BMI2 rests on a property of `PDEP`:
it deposits the `k`-th bit of its source on the `k`-th one of its mask, so expanding the lone bit
`1 << k` through `x` leaves exactly that one, whose position `TZCNT` reads off, two
instructions with no loop and no table. `TZCNT` is BMI1's, which every processor with BMI2 has
but `-mbmi2` alone does not enable: there `countr_zero` falls back to `BSF`, which leaves zero
undefined, so a zero test joins it, though the precondition leaves the deposit nonzero. A type
no wider than a block calls the block kernel directly. Its contract is narrow, as `bit_repeat`'s is, so it is not `noexcept`.

### `bit_mask`

[[bitmask.types]](https://eel.is/c++draft/bitmask.types) gives a bitmask type three implementations: an enumeration
that overloads the operators, an integer type, or a `std::bitset`. The
standard's own are mostly the first kind (`std::filesystem::perms`,
`std::launch`, `std::chars_format`, the `regex_constants` flags), and the
standard names the requirement without a concept to check it. `bit_mask` asks
for what /2 writes out: regularity, the complement, and the three logical
operators in both their compound and their binary form. Nothing arithmetic,
nothing about range, no conversions, no increment, no ordering, and no shifts:
[[bitmask.types]](https://eel.is/c++draft/bitmask.types) has none, and an enumeration overloading the rest need not add
them.

The shape is also `std::bitset`'s, which generalized exactly these operators
from the built-in integers and added only the shifts; it has no `+`, no `<`,
and no conversion to a number.

The compound forms need only be valid. /4 defines setting a value as
evaluating `X |= Y` and clearing it as `X &= ~Y`, and says nothing of what
either returns; /2's `X&` is one way the type "can be written", not a
requirement. Asking for it would turn away libstdc++'s `ios_base` flag types,
whose compound assignments return `const X&`, which the standard names as
bitmask types.

Two omissions are load-bearing rather than incidental, and one boundary is.

No strong ordering is asked for because `std::bitset` has no `operator<`
at all. Requiring an order would exclude the very type the concept is shaped
after, so a bit container's order stays its wrapper's business.

`-=` is not asked for either, and for one reason beyond being arithmetic: on an unsigned integer it is subtraction, and on a container of
bits the same spelling reads as set difference. A structural concept cannot
tell those apart, so it declines to ask for the operator whose meaning it
could not pin down.

The domain is narrowed to a **field of bits**, by a law rather than a list:

```cpp
unsigned_integer<T> or not (signed_integer<T> or std::integral<T>)
```

An integer must be an unsigned one; a type that is no kind of integer is
judged by its operators alone. The second disjunct is what admits
`std::bitset` and the bit containers, which are fields of bits without being
numbers at all, and the test pins that `std::bitset` models the concept.

Some narrowing is needed because the operators alone admit too much. `bool`,
the five character types and every signed built-in answer every requirement
stated here, through integral promotion, so no structural clause among the
operators can reach them. `integer_class` keeps `bool` out via `++` and `--`, which are not bitwise and
are not asked for, and keeps the character types out via the pair law, which is
arithmetic's law and says nothing about holding bits.

**This is the line `<bit>` already draws.** Measured across all sixteen
built-in candidates, the types this concept admits are exactly the types
`std::popcount`, `std::countr_zero`, `std::rotl` and `std::bit_width` accept
— the standard unsigned integer types and nothing else. `bool`, the character
types and every signed built-in are refused there, so a concept for a
fixed-width field of bits refuses them here. A case in the test asserts that
agreement type by type rather than describing it.

What each half turns away. `std::integral` catches `bool`, whose conversion
back from the promoted `int` is a nonzero test rather than a reduction —
`~true` is `true`, where a one-bit field gives `false`, and `&=`, `^=` and
`|=` come out right only because the nonzero test and mod 2 agree on `{0, 1}`.
Both front ends say so independently: `~b` is `-Wbool-operation` on GCC and
Clang alike, and GCC diagnoses it even inside an unevaluated requires-expression, which is why the test pins the
refusal by its result rather than by writing the expressions out. The same
predicate catches the character types and the signed built-ins, whose `>>` is
arithmetic — it preserves the sign rather than moving the bits. `char` and
`wchar_t` are only implementation-definedly signed; `int` is signed always,
so if the one disqualifies then the other does a fortiori.

`signed_integer` catches that same thing among integer classes, which
`std::integral` does not see. Without it `absl::int128` would be admitted
while `long long` was refused; the exact-width signed list in the test reaches
both, so the asymmetry is pinned where those types exist.

`unsigned char` stays admitted, which is what keeps `uint8_t` — a block type
the bits library actually instantiates — on the right side. `signed char` does
not, and `int8_t` goes with it.

Two alternatives were considered. Spelling the six out by name works, but
stops one step short of its own argument: it excludes `char` for
*maybe* sign-extending while admitting `int`, which certainly does. And a
nested requirement *can* state modularity — `requires T(~T(~T{})) == T{}` is
well-formed and does reject `bool` — but it cannot quantify, since a
requires-expression's own parameters are not constant expressions and naming
one there is a hard error rather than a substitution failure; it drags
`constexpr` into the concept, so `std::bitset` would model it under C++23 and
hard-error under C++20; and it wrongly *admits* `char`, which is genuinely
modular.

`promoted_t` does the work it does in `integer_class`, for the same reason:
`~` on an `unsigned char` yields `int`, so stating the closure against `T`
would reject every narrow built-in. The concept is cv-transparent by the same
idiom too — every requirement is stated of the cv-stripped `T`, so a `const`,
`volatile` or `const volatile` spelling models it exactly when the bare type
does.

### Conditional `noexcept`

All integer operations are `constexpr`. Those with a wide contract have a conditional `noexcept`,
on whether the element type's own operations carry the specifier, because
[[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc) does not ask for `noexcept` anywhere — the word does not
occur in the subclause. Requiring it would narrow the extension point to
integer-class types whose authors wrote it: `absl::uint128` is two `uint64_t`
halves with no way to throw and not one `noexcept` in its header, and was turned
away. Dropping it would go too far the other way, since `std::abs` and `std::div`
*are* `noexcept` as both major implementations ship them. Conditioning keeps
both, and refuses neither. `xstd::to_chars` is unconditional in the other
direction, having no `noexcept` at all, because `std::to_chars` has none either.

The predicate is named for what it can see — a type's declarations, not its
behavior. "Integral" rather than "arithmetic", because an arithmetic category would also include floating point and this answers false for every
floating-point type; "operators", because it ranges over a type's operators
rather than its category. It is public because it *is* those exception
specifications, seven in `<xstd/ints/cstdlib/>` and sixteen in `<xstd/ints/bit/>`, and a
caller asking whether one throws over their own type is asking exactly this.

Its operations are every requirement `integer_class` states over `const`
operands, and no more. That boundary is not this library's invention:
`absl::uint128` declares each of them `constexpr` and not one of its mutating
operators, so the `const` half is what an integer-class type in the field treats
as its value surface. It is also the only half these functions can reach, each
taking and returning a value. Within it the coverage is total rather than
itemized — a list drawn from what the bodies contain has to be revised whenever a
body changes, and being over-cautious about an operation a function never
performs costs nothing, where missing one it does perform is a wrong
`noexcept(true)`.

None of this would be here if the language could deduce it. `noexcept(auto)` was
proposed against exactly this shape of problem —
[N3207](https://wg21.link/N3207)
(2010), still open as
[P3166R0](https://github.com/cplusplus/papers/issues/1829)
(2024) — and [C++23](https://wg21.link/N4950) has no such thing. The manual `noexcept(noexcept(e))` reaches
only a single-expression function, which these are not.

A `= default` function needs no stand-in and gets neither specifier: defaulted on
its first declaration it is implicitly `constexpr` if the implicit declaration
would be, and its exception specification is computed. So `div_result`'s equality
writes only `[[nodiscard]]`, the one of the three with no implicit form. Neither
is restated for symmetry, because the two fail differently: a `constexpr` that
cannot hold is refused where a constant
expression needs it, while
[P1286R2](https://github.com/cplusplus/papers/issues/154)
(C++20) removed the rule that deleted such a defaulted function, so an over-broad
`noexcept` buys a `std::terminate` in place of a diagnostic.

### The 128-bit aliases

They are spelled `int128` and `uint128`, without the `_t` that `<cstdint>`'s
exact-width names carry. C reserves typedef names beginning with `int` or `uint`
and ending in `_t` for future additions to `<stdint.h>`. The reservation does not
reach into a user-defined namespace, but `<xstd/ints/cstdint.hpp>` exists precisely
because the standard header lacks the type, so the day it gains one is the day
the two spellings collide. [Boost.Int128](https://github.com/cppalliance/int128) renamed the same way during its review.

Their two associations live in `<xstd/ints/cstdint/int128.hpp>`, beside the aliases
and not inside the traits; `<xstd/ints/cstdint.hpp>` is the umbrella that exports
them. The header that introduces a pair registers it, so whoever can name the
type has the specializations in scope and cannot reach the type through a
translation unit that would answer differently. It also leaves `make_signed` and
`make_unsigned` knowing about no type in particular. What the aliases cannot do
is demonstrate the widening on their own, being specialized for by a header of
this library's; the exact-width test lists therefore also carry Boost.Int128 and
`absl::int128`, whose pairs no library of ours defines. Both dependencies are
optional; see [CONTRIBUTING.md](../CONTRIBUTING.md).

The two are not interchangeable, which is why both are here. Boost annotates its
operations and Abseil annotates none, so they land on opposite sides of
`nothrow_const_operators`. They differ once more: `absl::int128`'s `operator/`
and `operator%` are `constexpr` only where a 128-bit intrinsic backs them, and
out-of-line otherwise. That is the one place these tests assert something about
the target rather than the type. The whole-surface battery in
`test/src/ints/cstdlib.cpp` runs over it everywhere, constant-evaluated where the
intrinsic is there and checked at run time where it is not; its place in the
exact-width lists is conditional on `ABSL_HAVE_INTRINSIC_INT128` instead, those
cases constant-evaluating everything. Only the four divisions object at all, and
those are exactly what the battery covers.

### External pairs

Abseil and Boost.Int128 each introduce an integer-class pair, and neither can
name `xstd::make_signed`. Their associations are the two lines nobody can
derive, identical for every consumer, so `<xstd/ints/ext/absl/int128.hpp>` and
`<xstd/ints/ext/boost/int128.hpp>` ship them rather than leaving each user to write
the same specializations. Because they are explicit specializations, shipping
them later would have been the breaking change, not shipping them now: a user
who had written their own would meet a redefinition.

The layout is [Boost.Hana](https://www.boost.org/doc/libs/latest/libs/hana/)'s `boost/hana/ext/`, one directory per adapted library
and one header per upstream header, contents in this library's namespace rather
than the adapted one's. The prefix is load-bearing where Hana's is: these
headers are installed, and an unprefixed top-level `ext/` would put a generic
directory in a shared include namespace, where two libraries that both did it
would collide silently on the first `-I`. Nothing under `include/xstd/` includes
them and no umbrella sits above `<xstd/ints/ext/absl.hpp>` and `<xstd/ints/ext/boost.hpp>`,
so an adapted library is asked for by name and `<xstd/ints/cstdint.hpp>` never drags
in Abseil. Each hard-includes the library it adapts, leaving the `__has_include`
probe to the consumer — which the test suite writes for itself, having no
fixture headers for these two and reaching them through the shipped adapter the
way anyone else would.

Those includes are exported, so one is all a consumer writes: a translation unit
including `<xstd/ints/ext/absl/int128.hpp>` has `absl::int128` and `absl::uint128`
with it, and include-cleaner over that unit asks for nothing further, under a
configuration that inherits nothing from this repository.

Each adapter also carries `IWYU pragma: always_keep`, both the pair headers and
the two umbrellas, which do not inherit it. A header included for its
specializations is used in a way no use of a name can show, and without
the pragma a translation unit that includes one to register the pair and then
names nothing from it is told the include is unused.

That is the whole of it for Abseil, which declares both types in the header it
advertises and carries `IWYU pragma: export` on its own two includes. Boost
declares this pair in `detail/int128_imp.hpp` and `detail/uint128_imp.hpp` and
re-exports them from no public header, so the chain stops one hop short and a
consumer naming `boost::int128::uint128` is told nothing provides it. Nothing
this library ships can close that. An `export` pragma attaches to an include
directive, so reaching the declaration means naming Boost's `detail/` — a
liability, since upstream may rename or split either file and a pinned CI would
not see it. `always_keep` answers the half that is ours, and the half that is
left is a true statement about the consumer's own code.

So the answer is the linter's configuration rather than a header:
`misc-include-cleaner` is not asked about `boost/.*` at all, in the root
`.clang-tidy` and so in the test tree that inherits it. This is not a
Boost.Int128 workaround. No Boost library ships IWYU pragmas — Boost.Hana has
none across 450 headers — and [Boost.Test](https://www.boost.org/doc/libs/latest/libs/test/)'s macros expand through private
implementation headers for the same reason, which is why the test tree carried
this option long before these adapters existed. Any project including any Boost
library and linting its own sources needs the same line, and no library can
supply it on their behalf: the option belongs to the linter, and CMake carries
no usage requirement that could propagate one.

### Hashing

[Boost.Hash2](https://www.boost.org/doc/libs/latest/libs/hash2/) serializes an integer through byte writers that stop at 8 bytes, and
selects that path by `std::is_integral`, which libstdc++ 15 and older grant
`__int128` only outside `__STRICT_ANSI__`. Neither the Microsoft STL's `std::_Signed128` nor a
fundamental type such as `__int128` or `_BitInt(N)` has a namespace in which a
`tag_invoke` hook could be found by ADL, so no customization reaches them.
`<xstd/ints/ext/boost/hash2.hpp>` therefore supplies a function to call rather than a
hook to find: `xstd::hash_append_int(h, f, v)` feeds any xstd integer to Hash2's
existing writers, which is what keeps every flavor and constant evaluation working.

The message is the value, never the object. A `_BitInt(24)` occupies four bytes
one of which is padding with no specified contents, so hashing its storage would
break "equal values hash equal"; the value is read instead. Up to 64 bits it is
converted to the narrowest exact-width standard type of its signedness that holds
it, and handed to `hash_append`; past 64 bits it is split into 64-bit words, low
word first, as many as its width needs. Both steps sign-extend a signed value and
zero-extend an unsigned one. Two consequences follow, and the tests pin both.
Under `little_endian_flavor` the message is the value's little-endian bytes, so
every 128-bit type (native, the Microsoft STL's, Boost.Int128's, Abseil's and
`_BitInt(128)`) gives one digest. And types of one width agree whatever their
representation: `bit_int<32>` hashes as `int32_t`, `bit_int<64>` as `int64_t`, and
`bit_int<100>` as an `int128` of the same value.

Extending every bit-precise width to a whole 64-bit word was rejected: it would
have hashed `bit_int<32>` as eight bytes and `int32_t` as four, and libc++, which
makes `_BitInt` integral, already gives Hash2 the four. Under a big-endian flavor
the words stay low first and only the bytes within each word reverse, which is
how xstd-bits has always hashed its 128-bit blocks.

### Traits and concepts

The type utilities intentionally remain narrow:

- `is_signed` and `is_unsigned` are open counterparts of the standard traits.
- `make_signed` and `make_unsigned` are their open, user-specializable transformations.
- `promoted_t` names what a type's own operators yield.
- `nothrow_const_operators` answers whether the conditional `noexcept` holds.

A concept spelling is provided when the standard library has an analogous
concept; otherwise the trait is the interface. `nothrow_const_operators`,
like `integer_class`, `bit_mask`, `alignable` and `nothrow_alignable`, has no standard trait
to mirror and so no trait beside it, and it is spelled the way a caller writes it: inside a
`noexcept`.

Which decides where the `is` goes, and the subclause is no guide there. Its
concept is *is-integer-like* — the only concept in the library or the standard
whose name starts with `is`, because it is exposition-only and answers to nobody.
The convention outside it is the opposite and is worth following: `is` marks a
trait, as in `std::is_integral`, and a concept goes bare, as in `std::integral`.
So the concept here is `integer`, bare, rather than an `is_integer`
transliterating a hyphenated name that was never meant to be public; where a trait
does stand beside a concept, as `is_signed` does beside `signed_integer`, the
`is` is what marks which is which.

### Character conversion

`xstd::to_chars` delegates `char` and every standard integer type directly to
the standard library, so callers get its tuned implementation. Those are the types
[[charconv.to.chars]](https://eel.is/c++draft/charconv.to.chars) declares an overload for. `wchar_t`, `char8_t`,
`char16_t` and `char32_t` are refused rather than forwarded: `std::to_chars` would
take each only by integral promotion and write the code unit as a number, which no
caller converting a character type means. For an xstd
`integer` that `std::to_chars` does not cover, the digits are produced here to
the same specification, bases 2 through 36 and `value_too_large` included. The
default base is written as the literal `10`, as [[charconv.to.chars]](https://eel.is/c++draft/charconv.to.chars) writes it, so the two
signatures read alike; `readability-magic-numbers` is suppressed a line at a time
rather than taught to ignore `10` throughout.

The digits come off the unsigned counterpart of `I`, the only type in which the
magnitude of `min()` is representable, and everything after that is unsigned
arithmetic. Staying in `I` would work, `%` yielding a remainder of the value's
own sign that a digit table read backwards absorbs, but it pays for the sign at
every digit rather than once and forces both loops to test `rest / radix == 0`
where an unsigned magnitude tests `rest >= radix` — one division per digit rather
than two, and for an integer-class type a division is a call. libstdc++ makes the
same reduction in `__to_chars_i`, and the loop shape here is its
`__detail::__to_chars`.

Neither loop counts characters. The first walks a pointer forward from where the
most significant digit goes, and a step onto `last` is the short buffer; the
second walks it back down, writing as it goes. The buffer's end is the walk's own
bound rather than a count, which keeps the comparison between two pointers and
leaves no signed-to-unsigned mismatch to answer; the one place a length is asked
for at all is the check ahead of the walk, which spells it `std::distance`. The
sign is written last, the walk stepping back into the one position it reserved
and no digit claimed; writing it first is what obliges libstdc++ to leave a `'-'`
in a buffer it then reports as too small. That the walk cannot start before there is
room for the first digit and the sign is why `value_too_large` has two returns,
both covered per type since gcov records them per instantiation.

The overloads state the split literally, through one exposition-only concept
both name. The delegating overload accepts `char` and every `integer` that is no
wider than `uint128` and that a `std::to_chars` call takes; the fallback accepts
every other `integer`. The call is detected inside that concept; it stays an
answer rather than an error because `I` keeps the expression dependent until the
constraint is checked. Between the two
templates, a deleted `bool` overload mirrors the standard explicitly rather than
leaving the rejection as an incidental result of their constraints.

Their order in the header is reading order: the standard's own domain, then the
deleted `bool`, then the fallback for what it does not cover. It once could not
be. gcov names only the
first group of functions sharing a start line in a file, and gcovr keyed its
cross-translation-unit merge on those names,
so an unnamed group merged with nothing; the digits body contains lines no other
translation unit can reach — the short-buffer returns — so it had to be the named
one, and ordered the other way the file measured 98% and the coverage gate
failed. The same sensitivity was why the digits body kept its sign and magnitude
as two plain declarations. Passing `--merge-lines` to gcovr merges a template's
instantiations by line rather than by name, which dissolved that constraint and
with it the reason the digits overload came first.

Two places the standard library does not cover are worth separating. Integer-class
types are never covered, `to_chars` being specified over the built-in integer
types. The *built-in* 128-bit types are covered everywhere but one place:
libstdc++ 15 in the strict dialect, with `CMAKE_CXX_EXTENSIONS OFF`, counts
neither `__int128` nor `unsigned __int128` as `std::integral` and declares no
`std::to_chars` for them. There they take xstd's own digits. libstdc++ 16 and 17
and libc++ take both in either dialect, and forward. The fallback's constraint
needs no special case for this: a type that is not integral never reaches the
`std::to_chars` call the constraint asks about.

`<format>` does not follow `<charconv>` here. `std::formattable<__int128, char>`
holds on libstdc++ 15, 16 and 17 and on libc++, in both dialects, so the one
library and dialect that leaves the type out of `<charconv>` still formats it.
`<ostream>` writes it in neither.
`bool` is outside both conversion constraints.

### Formatting

`<xstd/ints/format.hpp>` formats a `div_result` as `(quotient, remainder)`, through three partial
specializations of `std::formatter`. `div_result` is not tuple-like, but `std::tie` makes a
tuple of its two members, so where the standard library can format a tuple of the element
type the specialization to use inherits
`std::formatter<std::tuple<I const&, I const&>>` and hands it `std::tie` — the
standard's own rendering, with no intermediate string.

That delegation can be missing in two ways that do not coincide: the element type
may not be formattable, which is Abseil's `int128` and the Microsoft STL's 128-bit
classes, or tuple formatting itself may be missing, [P2286](https://github.com/cplusplus/papers/issues/977) having reached libstdc++
only in GCC 15. Where only the element type is missing, the second specialization
renders each member through `xstd::to_chars` and hands the two digit strings to a
tuple formatter of an exposition-only type, so the spec grammar stays the tuple's:
`{:n}` and `{:m}` mean the same for `div_result<absl::int128>` as for
`div_result<int>`, and a format string written for one element type compiles for the
other. Where tuple formatting itself is missing, the third renders the same text into
an inherited string formatter, so `div_result` formats on every implementation and
for either signedness — for every element type `xstd::to_chars` itself covers, which
is to say every one with an unsigned counterpart.

The choice is left to partial ordering, on the same footing as `to_chars`'s
overloads: each specialization's constraints are a superset of the next one's, the
tuple-of-digits one adding that the library formats tuples, and the tuple-of-`I` one
adding that it formats `I`. All three produce `(quotient, remainder)`. Which one runs
shows only in the spec grammar, which is the tuple's wherever the library formats
tuples, and the string's on a library that formats none.

xstd specializes `std::formatter` only for `div_result`, which is program-defined. The
128-bit types are not xstd's to specialize for: they are built-ins or
standard-library types, which [[namespace.std]/2](https://eel.is/c++draft/namespace.std#2) does not cover. There is no
stream inserter either — a built-in has no associated namespace for ADL, and a
standard-library type's is one no program may add to. Boost.Test, the only
consumer that needed it, asks through `print_log_value`, which the tests
specialize directly.

### Alignment

`align_up` and `align_down` round a value to a power-of-two alignment, spelled as
[Boost.Align](https://www.boost.org/doc/libs/latest/libs/align/), LLVM's `alignTo`/`alignDown` and the kernel's `ALIGN` macros all
spell it: value first, alignment second. `is_aligned` asks whether rounding would
change anything.

All three are one operation: alignment is addition modulo a power of two.
`align_down(v, a)` is `v - (v mod a)`, `align_up(v, a)` is `align_down(v + a - 1,
a)`. The mask is not a different algorithm but a cheaper spelling of the
remainder, licensed by `v % 2^N == v & (2^N - 1)` — which holds only for an
unsigned type of radix 2, which is why `alignable` asks for radix 2 rather than
assuming it. Subtracting the remainder suffices; complementing the mask to keep
the high bits instead is a second trick that costs a requirement and, both
compilers agree, emits the same instructions, at most scheduled in another order.

`alignable` shares `integer_class`'s opening clauses and then stops, adding two of its own:
specialized, integer, radix 2, unsigned, wider than one bit, convertible both ways with
`size_t`, totally ordered, closed under `+`, `-` and `&`. Every
`unsigned_integer` is `alignable` and no `signed_integer` is, while the converse
fails — which is what kept it here when the storage utilities left for
[xstd-misc](https://github.com/rhalbersma/xstd-misc).

Signed types are excluded, though their arithmetic is fine: on two's complement
`align_up(-5, 4)` is `-4`. They are excluded because `align_up`'s precondition
cannot be checked for them. For a modular type the wrapped sum is a value, so
`assert(sum >= value)` detects it; for a signed type the overflow is undefined
before the assert runs. Stating it as `value <= max - mask` would cover signed
too, at the cost of asking `numeric_limits` for a bound at every call.

Each function takes a pointer overload that converts to `uintptr_t`, delegates and
converts back, aligning an address being this same arithmetic in the address
space. Delegating rather than repeating the mask work is what gives the pointer
case that precondition: Boost's pointer `align_up` duplicates it and quietly
returns null when an address near the top of the space wraps. Pointer overloads
cannot be `constexpr`, `reinterpret_cast` being barred from constant expressions.

Boost.Align's `detail::not_pointer` is neither redundant nor a diagnostic: it is
what makes Boost's own pointer overloads reachable, a `T*` argument otherwise
deducing exactly and beating the conversion to `void*`. A concept needs no such
trait, pointers failing `alignable` on their own. Boost's domain is decided by
its bodies, with `not_pointer` the one thing said at the interface; here the
interface says all of it. `std::align` remains the checked version of the pointer
case: it takes the remaining space too, so it can report that a block does not
fit, which these cannot.

## Integer division

`div_result` is xstd's common result for four complete integer-division
conventions: truncating `div`, Euclidean `div_euclid`, floored `div_floor`, and ceiling `div_ceil`.
Its result vocabulary follows [P3724R4](https://github.com/cplusplus/papers/issues/2354), while xstd
intentionally limits the operation family to these established conventions and
extends all four across its broader `integer` domain.

```cpp
template<xstd::integer I>
struct div_result
{
    I quotient;
    I remainder;
};
```

Every operation returns quotient and remainder together and preserves:

```text
numerator == denominator * quotient + remainder
```

| Function | Quotient rule | Remainder rule |
|---|---|---|
| `div` | toward zero | zero or numerator's sign |
| `div_euclid` | chosen for a nonnegative remainder | `0 <= remainder < abs(denominator)` |
| `div_floor` | toward negative infinity | zero or denominator's sign |
| `div_ceil` | toward positive infinity | zero or the opposite of denominator's sign; modulo 2^N when unsigned |

```cpp
constexpr auto result = xstd::div_floor(-8, 3);
static_assert(result.quotient == -3);
static_assert(result.remainder == 1);

auto const [quotient, remainder] = xstd::div_euclid(-8, 3);
auto const [q, r] = xstd::div_floor(-8, 3);
```

xstd adopts the relevant [P3724](https://github.com/cplusplus/papers/issues/2354) result vocabulary and rounding semantics, not the
complete proposal; no other rounding modes are part of xstd. All four
operations support signed, unsigned, extended, bit-precise, and paired
integer-class types. For unsigned types the first three conventions coincide, while
`div_ceil` rounds any inexact quotient up and leaves the remainder that requires,
`numerator - denominator * quotient`, taken modulo 2^N as [P3724R4](https://github.com/cplusplus/papers/issues/2354)'s
`div_rem_to_pos_inf` words it, so the identity above holds in the type's own
arithmetic. Its quotient is what counting whole units asks for: the blocks that
N bits take, at any block width.

## Requirements and evolution

xstd requires a conforming [C++23](https://wg21.link/N4950) compiler, and the CMake project CMake 3.30 or
later, exporting the header-only `xstd::ints` target. Consumers build neither the
tests nor their dependencies. New facilities should stay small, portable and
motivated by current practice; one that enters the standard library can be
retired, and the baseline can advance to C++26 once that is a practical default.

## Future directions

Directions considered and deliberately not taken yet. Each records what it would
take, so the question can be picked up without rediscovering the answer.

### A module interface

`import xstd.ints;` beside the headers, never instead of them, the way
[fmt](https://github.com/fmtlib/fmt) ships one: a second, optional target
(`XSTD_INTS_MODULE`) whose interface unit includes today's headers in its global
module fragment and exports the public names, with a `FILE_SET CXX_MODULES` over
CMake 3.28 or later. The `INTERFACE` target, its headers and its install rules stay
as they are. A compiled mode, fmt's third, has no counterpart here: there is no
non-template code to move into a translation unit.

Two costs decide when. The interface unit must enumerate the public API, a list to
keep current, though it would also make "public" a statement rather than whatever
sits under `include/xstd/`. And Apple Clang's version numbers do not map onto
upstream Clang's, so whether Xcode 16.4 and 26.6 can build it needs checking. CI
needs nothing new: a module leg is a stub passing `-DXSTD_INTS_MODULE=ON`, since
every leg's generator can scan module dependencies. Installing module interface
sources for a `find_package` consumer is the least settled part, and fmt is the
worked example to follow. xstd-misc and xstd-bits have the same shape, and this
library, the smallest surface, is where to answer it first.

### Choosing the bit-permutation kernel

The kernel `block_compress`, `block_expand` and `block_select` run follows the target macros and
nothing else, at the costs measured above. Four refinements are recorded, the
first concrete and the other three waiting on a measurement:

1. **The consumer states the path.** A CMake cache variable
   `XSTD_INTS_BIT_PERMUTATION` becomes an `INTERFACE` compile definition on
   `xstd::ints`, and a non-CMake user defines the same macro. `auto`, the default,
   is today's behaviour; `portable` always takes the `constexpr` kernels; `bmi2`
   takes `PEXT` and `PDEP` and is an `#error` on a target without fast BMI2, so a
   build whose flags drift fails rather than quietly changing kernels. Constant
   evaluation stays portable under every value, and every translation unit must see
   the same value, since the kernels are inline. There is no run-time dispatch: a
   library of headers has no binary to dispatch from, and a consumer shipping one
   artifact for many CPUs builds one copy per target, as Stockfish's universal
   build does. CI would add a `-march=x86-64-v3 -DXSTD_INTS_BIT_PERMUTATION=bmi2`
   leg for GCC and for Clang, and a compile-time test that each value reaches its
   kernel.
2. **Dense masks without fast BMI2.** Compressing by rank costs 2.5 to 3.2 cycles
   a selected bit on the Cascade Lake run, where zp7 is a flat 25 with `PCLMULQDQ`,
   so the two cross at eight or nine selected bits. Zen 1 and 2 have `PCLMULQDQ`
   though their `PEXT` is microcoded, so choosing by `popcount(mask)` needs the
   crossover measured on them, with and without it.
3. **Expand by select.** The one-block expand always walks the mask, though
   `block_expand_by_select` wins on a sparse value: on the Cascade Lake run it takes
   a flat 24 to 25 cycles for two source ones, against 2.5 to 3.0 a mask bit for the
   walk, while at half density the walk wins at every width. Choosing by
   `popcount(src)` against `popcount(mask)`, in the portable path only, needs that
   break-even measured on the targets the portable path serves, so that the check
   pays for itself on dense values.
4. **SVE2 BITPERM.** AArch64's optional `BEXT` and `BDEP`, under
   `__ARM_FEATURE_SVE2_BITPERM`, are the counterparts of `PEXT` and `PDEP`, and need
   their latency measured on a core that implements them.

Each new kernel joins the agreement checks against the portable kernels and the
oracle, fuzzing included. One mask applied to many values, where Hacker's Delight's
compress and zp7 both compute their shift masks once and apply them to each value,
needs a batch interface that the per-call signature cannot amortize, and belongs in
xstd-bits.
wasm32 has no bit-permute instruction, so the portable kernels are already its
answer.

## CI policy

The matrix follows the channel model used by
[apt.llvm.org](https://apt.llvm.org/): **stable** is the established release,
**qualification** the newest release still being qualified, and **development**
the current development branch. These are tested across the three main compiler
families, the three main standard libraries and the three main desktop platforms,
with MinGW, Apple Clang and Clang-CL filling out the remaining combinations.
Development jobs are required where a usable development compiler exists; the
README records the versions currently assigned to each channel. Building the
tests requires the dependencies documented in
[CONTRIBUTING.md](../CONTRIBUTING.md).

The tests compile in the library's strictly conforming dialect wherever that
dialect keeps the 128-bit column. `<bit>`'s functions take
`std::unsigned_integral`, which libstdc++ 15 and older grant `unsigned __int128`
only outside `__STRICT_ANSI__`; there `xstd::uint128` would drop out of the bit
functions' instantiation lists rather than fail to compile. A configure-time
probe asks the toolchain whether strict mode drops the type, and only where it
does, on libstdc++ 15 whether under GCC or Clang, do the tests, the fuzz targets
and the mutation build compile as `gnu++23`. libstdc++ 16 and later, libc++,
MSVC and the 32-bit legs build them strictly.

## Contracts


Each function states its domain twice. What a *type* must be is a concept, and a
type outside it is a compile error. What a *value* must be is a precondition, and
a value outside it is an `assert`: diagnosed where assertions are enabled, and
undefined behaviour where `NDEBUG` has removed them. Neither is reported as a
value at run time, and `to_chars` is no exception: its `std::errc` says the buffer
was too small, never that the call was out of domain.

| Function | Precondition |
| :------- | :----------- |
| `abs(x)` | `x != numeric_limits<I>::min()`, whose negation is unrepresentable; `unsigned_abs` is the total form of the same question and has none |
| `div(numer, denom)` <br> `div_euclid(numer, denom)` <br> `div_floor(numer, denom)` <br> `div_ceil(numer, denom)` | `denom` is not zero, and `numer` is not `numeric_limits<I>::min()` with `denom` at `-1`. Both are the undefined behaviour `/` and `%` already have; the assert is reached first |
| `to_chars(first, last, value, base)` | `2 <= base` and `base <= 36`, and `[first, last)` a valid range |
| `align_up(value, alignment)` <br> `align_down(value, alignment)` <br> `is_aligned(value, alignment)` | `alignment` is a power of two, which is not zero; `align_up` additionally that the rounded result is representable |
| `bit_ceil(x)` | The result is representable: `x` is at most the highest power of two of its type. A call that violates it is not a constant expression even under `NDEBUG`, as `[bit.pow.two]` requires |
| `bit_repeat(x, l)` | `l > 0`. A call that violates it is not a constant expression even under `NDEBUG`, as [P3104R6](https://github.com/cplusplus/papers/issues/1768) requires |
| `bit_select(x, k)` | `0 <= k` and `k < popcount(x)`, so `x` has a `k`-th one. A call that violates it is not a constant expression even under `NDEBUG`, as for `bit_repeat` |

`sign`, `unsigned_abs`, the `<bit>` counterparts other than `bit_ceil` and `bit_repeat`, and every concept and
trait are total: every value of a type they accept is in their domain.

The pointer overloads of the three alignment functions are address arithmetic and
nothing more: convert to `std::uintptr_t`, round, convert back. What comes out is
an address. It is not checked against the object the argument pointed into, no
object's lifetime begins there, and nothing makes it safe to dereference.
`std::align` is the checked form of the same case: it takes the space remaining
as well, so it can report that a block does not fit, which these cannot.
