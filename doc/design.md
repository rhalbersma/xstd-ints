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
- **Make semantics explicit.** The three division functions name their rounding
  convention rather than hiding it behind `/` and `%`.
- **Stay modular and dependency-free.** Linking `xstd::ints` adds include paths
  and the [C++23](https://wg21.link/N4950) requirement, but no runtime library or transitive package.

## API shape

### Integer types

[P3701R0](https://wg21.link/P3701R0) supplies the `integer`,
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

Character conversion delegates a non-`bool` standard integral type to
`std::to_chars` when it is no wider than `uint128`, which is where both standard
libraries stop; every other `integer` takes xstd's own digit generation.

`integer_class` follows the requirement order in [iterator.concept.winc], but is
an open structural concept rather than an implementation-defined set. It admits
integer types of every width instead of requiring them to be wider than every
integral type, and it deliberately checks only same-type arithmetic rather than
mixed-mode operations or common types. Its range metadata comes from the open
`xstd::numeric_limits`, whose primary template delegates to the standard trait.

It does not exclude the built-in integers. The subclause's own note says an
integer-class type need not be a class type, so excluding them was xstd's
addition rather than the standard's, and it made the requirements unaskable of
the types they were modelled on instead of merely false for them. The closure
requirements in /7.3 and /7.6 are therefore stated against `promoted_t` rather
than `T`: identity for a class type, and for a built-in whatever [conv.prom]
yields, which is what the same requirement means once promotion is in scope.
Asked that way every standard integer satisfies the concept except `bool`, whose
`++` and `--` C++17 removed. `promoted_t` asks the language — `decltype(+a)` —
rather than modelling the rules, and only where the language decides: a
non-integral type is its own, so a user-defined `operator+` cannot define its
own reference type into compliance.

That widening moves one boundary rather than removing it. `is_signed`,
`is_unsigned`, `make_signed` and `make_unsigned` each pair a standard-library
specialization with an `integer_class` one, and the two were disjoint only
because the concept excluded built-ins. Each now says `not std::integral` where
it means it — the "where std stops" line belongs to the trait that draws it, not
to the concept.

When Clang exposes `_BitInt`, `<xstd/ints/cstdint.hpp>` names the native types as
`bit_int<N>` and `bit_uint<N>`. They are aliases, not wrappers: their ABI,
conversions, promotions, and operators remain the compiler's. xstd specializes
its open limits and transformation traits for them, which makes paired widths
model `integer` even before the standard library recognizes the extension.
They reach it as `integer_class` on either standard library, though only libc++
makes `_BitInt` integral: that choice moves which arm `promoted_t` takes, and
both name `_BitInt` itself, since it is exempt from promotion. The public
domain currently begins at two because Clang has not yet implemented N3747's
signed `_BitInt(1)`.

### `bitwise_operators`

`integer_class` pruned to the half that is about bits rather than numbers:
regularity, the complement, and the three logical operators and the two
shifts, each in both its compound and its binary form. Nothing arithmetic,
nothing about range, no conversions, no increment, no ordering.

The shape is `std::bitset`'s. That template generalized exactly this set from
the built-in integers — [template.bitset] has `&=`, `|=`, `^=`, `<<=`, `>>=`,
`~`, `&`, `|`, `^`, `<<` and `>>`, and no `+`, no `<`, and no conversion to a
number — so a concept for *a fixed-width field of bits* is `integer_class`
with the numeric half removed. The clause references are kept where they
stood, so what was dropped is visible beside what was not.

Two omissions are load-bearing rather than incidental, and one exclusion is.

The strong ordering under /9 is gone because `std::bitset` has no `operator<`
at all. Requiring an order would exclude the very type the concept is shaped
after, so a bit container's order stays its wrapper's business.

`-=` goes with the rest of /7.5's arithmetic, and for one reason beyond being
arithmetic: on an unsigned integer it is subtraction, and on a container of
bits the same spelling reads as set difference. A structural concept cannot
tell those apart, so it declines to ask for the operator whose meaning it
could not pin down.

`bool` and the five character types are excluded **by name** — the one
boundary in this library drawn by a list rather than by behaviour, and stated
as such where it is drawn.

The pruning would admit all six. Every operator this concept asks for is valid
on each of them through integral promotion, so no structural clause can reach
them; `integer_class` kept `bool` out only via `++` and `--`, which are not
bitwise and are not asked for here, and kept the character types out via the
pair law, which is arithmetic's law and has nothing to say about holding bits.
What disqualifies them is what the operators *mean*, and meaning is exactly
what a concept cannot interrogate.

`bool` is not modular. Its conversion back from the promoted `int` is a
nonzero test rather than a reduction, so `~true` is `true` and `true <<= 1` is
`true`, where a one-bit field gives `false` both times; `&=`, `^=` and `|=`
come out right only because the nonzero test and mod 2 agree on `{0, 1}`. Both
front ends already say so independently — `~b` is `-Wbool-operation` on GCC
and Clang alike, and `b <<= 1` is `-Wint-in-bool-context` on GCC — so the
concept is restating in its own vocabulary a judgement the compilers had
already made.

`char` and `wchar_t` have implementation-defined signedness, so `>>` may
sign-extend and the same source yields different bits on different targets.
`char8_t`, `char16_t` and `char32_t` are guaranteed unsigned and would be
sound; they go for the other reason, that they are text and every width they
reach is reached better by the `uintN_t` that names it.

`signed char` and `unsigned char` are deliberately untouched. They are narrow
character types but standard *integer* types, which is what keeps `int8_t` and
`uint8_t` — a block type the bits library actually instantiates — on the
admitted side. `is_character_v` draws that line and is closed where xstd's
signedness traits are open: the core language fixes this list and no user type
can join it.

The alternative was considered and rejected. A nested requirement *can* state
modularity — `requires T(~T(~T{})) == T{}` is well-formed and does reject
`bool` — but it cannot quantify, since a requires-expression's own parameters
are not constant expressions and naming one there is a hard error rather than
a substitution failure; it drags `constexpr` into the concept, so `std::bitset`
would model it under C++23 and hard-error under C++20; and it wrongly *admits*
`char`, which is genuinely modular. A spot check that is weaker than the list
it replaces is not worth the machinery.

`promoted_t` does the work it does in `integer_class`, for the same reason:
`~` on an `unsigned char` yields `int`, so stating the closure against `T`
would reject every narrow built-in. The concept is cv-transparent by the same
idiom too — every requirement is stated of the cv-stripped `T`, so a `const`,
`volatile` or `const volatile` spelling models it exactly when the bare type
does.

### Conditional `noexcept`

All integer operations are `constexpr`. Their `noexcept` is conditional, on
whether the element type's own operations carry the specifier, because
[iterator.concept.winc] does not ask for `noexcept` anywhere — the word does not
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
rather than its category. It is public because it *is* those six exception
specifications, and a caller asking whether one throws over their own type is
asking exactly this.

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
[N3207](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2010/n3207.htm)
(2010), still open as
[P3166R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p3166r0.html)
(2024) — and [C++23](https://wg21.link/N4950) has no such thing. The manual `noexcept(noexcept(e))` reaches
only a single-expression function, which these are not.

A `= default` function needs no stand-in and gets neither specifier: defaulted on
its first declaration it is implicitly `constexpr` if the implicit declaration
would be, and its exception specification is computed. So `div_result`'s equality
writes only `[[nodiscard]]`, the one of the three with no implicit form. Neither
is restated for symmetry, because the two fail differently: a `constexpr` that
cannot hold is refused where a constant
expression needs it, while
[P1286R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1286r2.html)
(C++20) removed the rule that deleted such a defaulted function, so an over-broad
`noexcept` buys a `std::terminate` in place of a diagnostic.

### The 128-bit aliases

They are spelled `int128` and `uint128`, without the `_t` that `<cstdint>`'s
exact-width names carry. C reserves typedef names beginning with `int` or `uint`
and ending in `_t` for future additions to `<stdint.h>`. The reservation does not
reach into a user-defined namespace, but `<xstd/ints/cstdint.hpp>` exists precisely
because the standard header lacks the type, so the day it gains one is the day
the two spellings collide. Boost.Int128 renamed the same way during its review.

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
`test/src/cstdlib.cpp` runs over it everywhere, constant-evaluated where the
intrinsic is there and checked at run time where it is not; its place in the
exact-width lists is conditional on `ABSL_HAVE_INTRINSIC_INT128` instead, those
cases constant-evaluating everything. Only the three divisions object at all, and
those are exactly what the battery covers.

### External pairs

Abseil and Boost.Int128 each introduce an integer-class pair, and neither can
name `xstd::make_signed`. Their associations are the two lines nobody can
derive, identical for every consumer, so `<xstd/ints/ext/absl/int128.hpp>` and
`<xstd/ints/ext/boost/int128.hpp>` ship them rather than leaving each user to write
the same specializations. Because they are explicit specializations, shipping
them later would have been the breaking change, not shipping them now: a user
who had written their own would meet a redefinition.

The layout is Boost.Hana's `boost/hana/ext/`, one directory per adapted library
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
specializations is used in a way no reference to a name can show, and without
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
none across 450 headers — and Boost.Test's macros expand through private
implementation headers for the same reason, which is why the test tree carried
this option long before these adapters existed. Any project including any Boost
library and linting its own sources needs the same line, and no library can
supply it on their behalf: the option belongs to the linter, and CMake carries
no usage requirement that could propagate one.

### Traits and concepts

The type utilities intentionally remain narrow:

- `is_signed` and `is_unsigned` are open counterparts of the standard traits.
- `make_signed` and `make_unsigned` are their open, user-specializable transformations.
- `promoted_t` names what a type's own operators yield.
- `nothrow_const_operators` answers whether the conditional `noexcept` holds.

A concept spelling is provided when the standard library has an analogous
concept; otherwise the trait is the interface. `nothrow_const_operators` is
the one concept with no trait beside it, having no standard trait to mirror, and
is spelled the way a caller writes it: inside a `noexcept`.

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

`xstd::to_chars` delegates every non-`bool` standard integral type directly to
the standard library, so callers get its tuned implementation. For an xstd
`integer` that `std::to_chars` does not cover, the digits are produced here to
the same specification, bases 2 through 36 and `value_too_large` included. The
default base is written as the literal `10`, as [charconv.to.chars] writes it, so the two
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

The overloads state the split literally. The delegating overload accepts
`std::integral` except `bool`; the fallback accepts `integer` only when a
`std::to_chars` call is not well-formed. That detection is written directly in
the fallback's constraint; it stays an answer rather than an error because `I`
keeps the expression dependent until the constraint is checked. Between the two
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
types. But the *built-in* 128-bit types are not covered either in this dialect:
libstdc++ gates its 128-bit pair on `__GLIBCXX_TYPE_INT_N_0`, predefined only
outside `__STRICT_ANSI__`, so with `CMAKE_CXX_EXTENSIONS OFF` the overload is
absent and the call is *ambiguous*. Detection therefore has to survive an
ambiguity rather than a clean deduction failure, which it does: overload
resolution failing is still failing in the immediate context.

`<format>` goes the other way for the same underlying reason, libstdc++ adding a
`formatter<__int128, CharT>` *only* under `__STRICT_ANSI__`. So the three
facilities disagree about one type along different axes: `<format>` works only in
the strict dialect, `<charconv>` only outside it, and `<ostream>` in neither.
`bool` is outside both conversion constraints.

### Formatting

`<xstd/ints/format.hpp>` formats a `div_result` as `(quotient, remainder)`, through two partial
specializations of `std::formatter`. `div_result` is tuple-like, so where the standard
library can format a tuple of the element type the specialization to use inherits
`std::formatter<std::tuple<I const&, I const&>>` and hands it `std::tie` — the
standard's own rendering, with no intermediate string.

That delegation can be missing in two ways that do not coincide: the element type
may not be formattable, which is the Microsoft STL's 128-bit classes, or tuple
formatting itself may be missing, p2286 having reached libstdc++ only in GCC 15.
Testing `formattable` on the *tuple* covers both with one predicate. Where it
does not hold, the other specialization renders the members through
`xstd::to_chars` into an inherited string formatter, so `div_result` formats on every
implementation and for either signedness — for every element type `xstd::to_chars`
itself covers, which is to say every one with an unsigned counterpart.

The choice is left to partial ordering, on the same footing as `to_chars`'s
overloads: the tuple one requires the base it inherits, spelled where it applies,
on top of the counterpart the other asks for, so its constraints are a superset
and it wins wherever both are viable. Both produce `(quotient, remainder)`, so which runs is
not observable in the output — only in the spec grammar, `parse()` being
inherited, which is why the README says so.

xstd specializes `std::formatter` only for `div_result`, which is program-defined. The
128-bit types are not xstd's to specialize for: they are built-ins or
standard-library types, which [namespace.std]/2 does not cover. There is no
stream inserter either — a built-in has no associated namespace for ADL, and a
standard-library type's is one no program may add to. Boost.Test, the only
consumer that needed it, asks through `print_log_value`, which the tests
specialize directly.

### Alignment

`align_up` and `align_down` round a value to a power-of-two alignment, spelled as
Boost.Align, LLVM's `alignTo`/`alignDown` and the kernel's `ALIGN` macros all
spell it: value first, alignment second. `is_aligned` asks whether rounding would
change anything.

All three are one operation: alignment is addition modulo a power of two.
`align_down(v, a)` is `v - (v mod a)`, `align_up(v, a)` is `align_down(v + a - 1,
a)`. The mask is not a different algorithm but a cheaper spelling of the
remainder, licensed by `v % 2^N == v & (2^N - 1)` — which holds only for an
unsigned type of radix 2, which is why `alignable` asks for radix 2 rather than
assuming it. Subtracting the remainder suffices; complementing the mask to keep
the high bits instead is a second trick that costs a requirement and, both
compilers agree, emits the same instructions.

`alignable` is `integer_class`'s opening clauses and then a stop: specialized,
integer, radix 2, unsigned, wider than one bit, convertible both ways with
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

`div_result` is xstd's common result for three complete integer-division
conventions: truncating `div`, Euclidean `div_euclid`, and floored `div_floor`.
Its result vocabulary follows [P3724R4](https://wg21.link/P3724R4), while xstd
intentionally limits the operation family to these established conventions and
extends all three across its broader `integer` domain.

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

```cpp
constexpr auto result = xstd::div_floor(-8, 3);
static_assert(result.quotient == -3);
static_assert(result.remainder == 1);

auto const [quotient, remainder] = xstd::div_euclid(-8, 3);
auto const [q, r] = xstd::div_floor(-8, 3);
```

xstd adopts the relevant P3724 result vocabulary and rounding semantics, not the
complete proposal. No other rounding modes are part of this change. All three
operations support signed, unsigned, extended, bit-precise, and paired
integer-class types; for unsigned types all three conventions coincide.

## Requirements and evolution

xstd requires a conforming [C++23](https://wg21.link/N4950) compiler, and the CMake project CMake 3.28 or
later, exporting the header-only `xstd::ints` target. Consumers build neither the
tests nor their dependencies. New facilities should stay small, portable and
motivated by current practice; one that enters the standard library can be
retired, and the baseline can advance to C++26 once that is a practical default.

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
