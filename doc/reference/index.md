# Reference

Each public entity of xstd-ints is specified here the way the standard specifies the library: a synopsis per header,
then for each entity the elements of [[structure.specifications]](https://eel.is/c++draft/structure.specifications) that apply. The baseline is
[C++23](https://wg21.link/N4950). A facility from a later working draft or a proposal cites the paper it follows, and a
name with no namespace is in `std`.

The synopses leave out `[[nodiscard]]`, as the standard's own have since [P2422R1](https://github.com/cplusplus/papers/issues/1744), which
made it a recommended practice rather than part of a library's specification. The code carries it on every function that
returns a value, so discarding a result here draws a warning.

## How to read this reference

Each header has a page, laid out as [cppreference](https://en.cppreference.com/) lays out its pages:

- **The declarations,** numbered (1), (2), … where there are several, so the text below can say what each does. In a
  declaration, `see below` stands for what the text specifies.
- **A short description,** then one item per numbered declaration.
- **The parameters and the members,** as tables: template parameters, parameters, member types, member functions.
- **Semantic requirements** and **Models,** for a concept: what a type must do beyond the syntax, and which real
  types model it and which do not. Each of those types is tested.
- **Notes,** for what holds of the entity as a whole.
- **See also,** for the entities a reader looks up next.

Within a section, the elements of [[structure.specifications]](https://eel.is/c++draft/structure.specifications) keep
their meanings:

| Element | What it tells the caller |
| :------ | :----------------------- |
| *Constraints:* | When the overload takes part in overload resolution. A call that fails one is not an error by itself: it finds another overload, or none. |
| *Preconditions:* | What the caller must ensure. A call that breaks one has undefined behaviour, and where a page says so, a hardened build or a constant evaluation diagnoses it. |
| *Effects:* | What the call does. |
| *Returns:* | The value the call returns. |
| *Throws:* | The exceptions the call throws, and when. A function with none listed and no `noexcept` may still throw from what it calls. |
| *Remarks:* | Anything else the caller can rely on, such as when the function is `noexcept` or a constant expression. |

## Relation to the standard library

Each entity is one of five kinds:

- **Copied:** the standard's own exposition-only wording, word for word, under a name users can spell.
- **Extends:** a facility of the standard or of its working draft under its own name, giving the standard's answer
  wherever the standard gives one, with overloads or specializations for more types.
- **Generalizes:** the job of a standard facility, done for more types through an interface of its own.
- **Spells:** a portable spelling of a standard attribute or macro.
- **Proposed** or **new:** no counterpart in the standard or its working draft. A proposed entity follows a WG21
  paper; a new one follows none.

The Counterpart column leads with the clause of the working draft each entity answers, then says how it extends or
generalizes what that clause specifies. The rows follow the draft's order of those clauses, and a row with no
counterpart sits beside its nearest kin, citing the kin's clause where one is shared.

<div class="relation" markdown>

| Entity | Kind | Counterpart |
| :-- | :-- | :-- |
| `promoted_t` | New | [[conv.prom]](https://eel.is/c++draft/conv.prom) none in the standard: Boost.TypeTraits' `integral_promotion_t`, as the unqualified type of `+a` for an integral type and the type itself for any other, an enumeration included |
| `bit_mask` | Generalizes | [[bitmask.types]](https://eel.is/c++draft/bitmask.types) as a concept: an unsigned integer, or any type that is no kind of integer and has the bitmask operators |
| `div` | Extends | [[cstdlib.syn]](https://eel.is/c++draft/cstdlib.syn) [[cinttypes.syn]](https://eel.is/c++draft/cinttypes.syn) truncated division for every `integer`, returning `div_result<I>` at the operands' own width |
| `div_result` | Generalizes | [[cstdlib.syn]](https://eel.is/c++draft/cstdlib.syn) [[cinttypes.syn]](https://eel.is/c++draft/cinttypes.syn) one aggregate for every `integer`, with [P3724R4](https://github.com/cplusplus/papers/issues/2354)'s member names `quotient` and `remainder` |
| `div_euclid`<br>`div_floor`<br>`div_ceil` | Proposed | [[cstdlib.syn]](https://eel.is/c++draft/cstdlib.syn) with the Euclidean, floored and ceiling rounding [P3724R4](https://github.com/cplusplus/papers/issues/2354) proposes, each returning quotient and remainder as a `div_result`; `div_ceil` follows its `div_rem_to_pos_inf` |
| `numeric_limits`<br>`numeric_limits<signed _BitInt(N)>`<br>`numeric_limits<unsigned _BitInt(N)>` | Extends | [[numeric.limits]](https://eel.is/c++draft/numeric.limits) as a base for an object type that is not an array and an empty class for every other type, with a cv-qualified type forwarded to xstd's own specialization and a `_BitInt(N)` answered at its width |
| `bit_int`<br>`bit_uint` | New | [[cstdint.syn]](https://eel.is/c++draft/cstdint.syn) none: aliases for `signed _BitInt(N)` and `unsigned _BitInt(N)`, from a width of 2, which [P3666](https://github.com/cplusplus/papers/issues/2420) proposed until LEWG found no consensus for them |
| `bit_int_max_width`<br>`XSTD_HAS_BIT_INT` | New | [[cstdint.syn]](https://eel.is/c++draft/cstdint.syn) none: the compiler's `__BITINT_MAXWIDTH__`, detected and as a `size_t` |
| `int128`<br>`uint128`<br>`XSTD_HAS_INT128` | New | [[cstdint.syn]](https://eel.is/c++draft/cstdint.syn) none: the platform's 128-bit pair, the Microsoft STL's integer classes or `__int128`, and the macro that detects it |
| `integer`<br>`signed_integer`<br>`unsigned_integer` | Proposed | [[concepts.arithmetic]](https://eel.is/c++draft/concepts.arithmetic) as [P3701R0](https://github.com/cplusplus/papers/issues/2330) proposes, extended to the integer-class types that a specialization of `make_signed` and `make_unsigned` pairs |
| `is_aligned`<br>`align_up`<br>`align_down` | Proposed | [[ptr.align]](https://eel.is/c++draft/ptr.align) as [N4201](https://wg21.link/n4201) proposes them for `<memory>`, beside `align`, which fits a block into a buffer it checks, and `is_sufficiently_aligned`, which takes the alignment as a template argument and asks it of a pointer only |
| `alignable`<br>`nothrow_alignable` | New | [[ptr.align]](https://eel.is/c++draft/ptr.align) none: what the alignment functions ask of a type, and whether its operations are `noexcept` |
| `nothrow_const_operators` | New | none: the exception specification of the integer functions |
| `is_signed`<br>`is_unsigned`<br>`is_signed_v`<br>`is_unsigned_v` | Extends | [[meta.unary.prop]](https://eel.is/c++draft/meta.unary.prop) with the same value, except for an integer-class type that is not `integral`, which answers by comparing `-1` with `0` |
| `make_signed`<br>`make_unsigned`<br>`make_signed_t`<br>`make_unsigned_t` | Extends | [[meta.trans.sign]](https://eel.is/c++draft/meta.trans.sign) with the same type for an integral or enumeration type other than `bool`; an integer-class type is its own half, a program may specialize them to pair it with the other, a cv-qualified type follows its unqualified type, and any other type has no member `type` |
| `make_signed<unsigned _BitInt(N)>`<br>`make_unsigned<signed _BitInt(N)>` | Extends | [[meta.trans.sign]](https://eel.is/c++draft/meta.trans.sign) specializations of xstd's traits that pair each width of `_BitInt(N)` with the other signedness |
| `make_signed<absl::uint128>`<br>`make_unsigned<absl::int128>`<br>`make_signed<boost::int128::uint128>`<br>`make_unsigned<boost::int128::int128>` | Extends | [[meta.trans.sign]](https://eel.is/c++draft/meta.trans.sign) specializations of xstd's traits that pair Boost.Int128's and Abseil's 128-bit classes |
| `hash_append_int` | New | [[unord.hash]](https://eel.is/c++draft/unord.hash) none in the standard: Boost.Hash2's `hash_append`, for the value of every `integer` rather than its object representation |
| `byteswap` | Extends | [[bit.byteswap]](https://eel.is/c++draft/bit.byteswap) forwarding where the standard library takes the argument, and swapping the value of every other `integer` whose width is a whole number of bytes |
| `has_single_bit`<br>`bit_ceil`<br>`bit_floor`<br>`bit_width` | Extends | [[bit.pow.two]](https://eel.is/c++draft/bit.pow.two) forwarding where the standard library takes the argument, and answering every other `unsigned_integer` |
| `shl`<br>`shr` | Extends | [[bit.shift]](https://eel.is/c++draft/bit.shift) from [P3793R2](https://github.com/cplusplus/papers/issues/2399), forwarding where the standard library declares them, and answering every pair of `integer` types elsewhere |
| `rotl`<br>`rotr` | Extends | [[bit.rotate]](https://eel.is/c++draft/bit.rotate) forwarding where the standard library takes the argument, and answering every other `unsigned_integer` |
| `countl_zero`<br>`countl_one`<br>`countr_zero`<br>`countr_one`<br>`popcount` | Extends | [[bit.count]](https://eel.is/c++draft/bit.count) forwarding where the standard library takes the argument, and answering every other `unsigned_integer` |
| `bit_reverse`<br>`bit_repeat`<br>`bit_compress`<br>`bit_expand` | Extends | [[bit.permute]](https://eel.is/c++draft/bit.permute) from [P3104R6](https://github.com/cplusplus/papers/issues/1768), forwarding where the standard library declares them for the type, except `bit_compress` and `bit_expand` on libstdc++ where xstd has BMI2, and answering every `unsigned_integer` it does not forward |
| `bit_select` | New | [[bit.permute]](https://eel.is/c++draft/bit.permute) none: no paper proposes it, and `<bit>` has nothing to forward to |
| `integer_class` | Generalizes | [[iterator.concept.winc]](https://eel.is/c++draft/iterator.concept.winc) the operations that subclause states of an integer-class type, as a concept, opened to the built-in integers and to every width |
| `to_chars` | Extends | [[charconv.to.chars]](https://eel.is/c++draft/charconv.to.chars) forwarding for `char` and the standard integer types, and writing every other `integer` as that subclause specifies |
| `to_chars_max_size` | New | [[charconv.to.chars]](https://eel.is/c++draft/charconv.to.chars) none: the size of a buffer that holds every value of `I` at every base |
| `formatter<xstd::div_result<I>, charT>` | Extends | [[format.formatter.spec]](https://eel.is/c++draft/format.formatter.spec) specialized for `div_result`, as [[namespace.std]](https://eel.is/c++draft/namespace.std) admits for a program-defined type, built on the tuple formatter of [[format.tuple]](https://eel.is/c++draft/format.tuple) where the library has one |
| `abs` | Extends | [[c.math.abs]](https://eel.is/c++draft/c.math.abs) [[cinttypes.syn]](https://eel.is/c++draft/cinttypes.syn) one template for every `integer`, returning the promoted type, as they do |
| `sign` | New | [[c.math.abs]](https://eel.is/c++draft/c.math.abs) none in the standard: Boost.Math's `sign`, for every `integer`, as an `int` |
| `unsigned_abs` | New | [[c.math.abs]](https://eel.is/c++draft/c.math.abs) none in the standard: the magnitude of every `integer`, in its unsigned counterpart |

</div>

## Prior work

What each entity builds on in C++, and, for an entity that introduces a name or a concept of its own, its counterpart
in another language where that counterpart shares its purpose. Papers are linked through wg21.link.

- **`integer_class`:** integer-class types come from [P1522R1](https://wg21.link/p1522r1), *Iterator Difference Type
  and Integer Overflow* (Eric Niebler), and [P2393R1](https://wg21.link/p2393r1), *Cleaning up integer-class types*
  (Tim Song), revises their specification.
- **`integer`, `signed_integer`, `unsigned_integer`:** [P3701R0](https://wg21.link/p3701r0), *Concepts for integer
  types, not integral types* (Jan Schultke), whose built-in boundary these follow.
- **`promoted_t`:** Boost.TypeTraits'
  [`integral_promotion_t`](https://www.boost.org/doc/libs/latest/libs/type_traits/doc/html/boost_typetraits/reference/integral_promotion.html),
  which `promoted_t` follows for `short`, `bool`, `wchar_t`, `unsigned __int128` and `_BitInt(N)`, and departs from
  where `integral_promotion_t` does not give the type of `+a`, or gives what an enumeration's own operators do not:
    - it keeps the cv-qualifiers, where `promoted_t<short const>` is `int`;
    - it leaves `char8_t`, `char16_t` and `char32_t` as they are, where `+a` and `promoted_t` give `int`;
    - it promotes an unscoped enumeration to `int`, where one such as libstdc++'s `ios_base::fmtflags` overloads `|`,
      `&`, `^` and `~` to return the enumeration, which `bit_mask` asks `promoted_t` to be;
    - it does not compile for a scoped enumeration such as `std::byte` or `std::launch`, as it applies `+` to it.
- **`int128`, `uint128`:**
    - **C++:** Abseil's [`absl::int128` and `absl::uint128`](https://abseil.io/docs/cpp/guides/numeric#128-bit-integers)
      and [Boost.Int128](https://github.com/cppalliance/int128)'s pair are 128-bit integer classes, which the `ext`
      headers pair. [P3140R0](https://wg21.link/p3140r0), *`std::int_least128_t`* (Jan Schultke), proposes a standard
      128-bit integer type.
    - **Rust** has [`i128`](https://doc.rust-lang.org/std/primitive.i128.html) and
      [`u128`](https://doc.rust-lang.org/std/primitive.u128.html) as primitive types.
- **`bit_int`, `bit_uint`:**
    - **C++:** [P3666R4](https://wg21.link/p3666r4), *Bit-precise integers* (Jan Schultke), brings `_BitInt(N)` into
      C++ as integer types. Up to R3 it proposed the aliases too; R4 drops them, as LEWG found no consensus for them.
    - **C23** has `_BitInt(N)`, which the aliases name, and Clang offers it in C++ as an
      [extension](https://clang.llvm.org/docs/LanguageExtensions.html#bitint-extint).
- **`byteswap`:** [P1272R4](https://wg21.link/p1272r4), *Byteswapping for fun&&nuf* (Isabella Muerte, Corentin Jabot).
  [N3864](https://wg21.link/n3864), *A constexpr bitwise operations library for C++* (Matthew Fioravante), proposed
  `reverse_bytes` before it, which reverses the bytes of an integer at its default arguments.
- **`rotl`, `rotr` and the counting functions:** [P0553R4](https://wg21.link/p0553r4), *Bit operations* (Jens Maurer).
  [N3864](https://wg21.link/n3864) proposed them before it, as `rotl`, `rotr`, `cntl0`, `cntt0`, `cntl1`, `cntt1` and
  `popcount`.
- **`has_single_bit`, `bit_ceil`, `bit_floor`, `bit_width`:** [P0556R3](https://wg21.link/p0556r3), *Integral
  power-of-2 operations* (Jens Maurer), under the names of [P1956R1](https://wg21.link/p1956r1), *On the naming of
  low-level bit manipulation functions* (Vincent Reverdy). [LWG 3656](https://wg21.link/LWG3656), *Inconsistent bit
  operations returning a count*, gives `bit_width` its `int`. [N3864](https://wg21.link/n3864) proposed the first
  three as `ispow2`, `ceilp2` and `floorp2`.
- **`shl`, `shr`:** [P3793R2](https://wg21.link/p3793r2), *Better shifting* (Brian Bi, Jan Schultke).
  [N3864](https://wg21.link/n3864) proposed logical and arithmetic shifts as `shll`, `shlr`, `shal` and `shar`,
  undefined for a negative count or one past the width.
- **`bit_reverse`, `bit_repeat`, `bit_compress`, `bit_expand`:** [P3104R6](https://wg21.link/p3104r6), *Bit
  permutations* (Jan Schultke). [N3864](https://wg21.link/n3864) proposed `reverse_bits`, and compress and expand as
  `deposit_bits_right` and `extract_bits_right`: its definitions and examples give `deposit_bits_right` what
  `bit_compress` does and `extract_bits_right` what `bit_expand` does, while its table of instructions maps them to
  `PDEP` and `PEXT`, the other way round.
- **`bit_select`:** Sebastiano Vigna's
  [*Broadword Implementation of Rank/Select Queries*](https://vigna.di.unimi.it/ftp/papers/Broadword.pdf), whose
  broadword select it runs wherever it does not use `PDEP`.
- **`abs`, `div`:** [P0533R9](https://wg21.link/p0533r9), *`constexpr` for `<cmath>` and `<cstdlib>`* (Oliver Rosten,
  Edward Rosten), which makes the standard functions `constexpr`.
- **`div_result`, `div_euclid`, `div_floor`, `div_ceil`:**
    - **C++:** [P3724R4](https://wg21.link/p3724r4), *Integer division* (Jan Schultke), for the member names and the
      rounding. Daan Leijen's
      [*Division and Modulus for Computer Scientists*](https://www.microsoft.com/en-us/research/publication/division-and-modulus-for-computer-scientists/)
      sets truncated, floored and Euclidean division side by side.
    - **Rust's** [`div_euclid`](https://doc.rust-lang.org/std/primitive.i32.html#method.div_euclid) is `div_euclid`'s
      quotient, and its [`div_ceil`](https://doc.rust-lang.org/std/primitive.u32.html#method.div_ceil), stable on the
      unsigned types, is `div_ceil`'s.
    - **Python's** [`divmod`](https://docs.python.org/3/library/functions.html#divmod) returns the quotient and
      remainder of floored division together, as `div_floor` does.
- **`sign`:**
    - **C++:** Boost.Math's
      [`sign`](https://www.boost.org/doc/libs/latest/libs/math/doc/html/math_toolkit/sign_functions.html), which
      returns `1`, `-1` or `0` as an `int`.
    - **Rust's** [`signum`](https://doc.rust-lang.org/std/primitive.i32.html#method.signum) gives the same three values,
      in the operand's own type and for the signed types only.
- **`unsigned_abs`:** **Rust's** [`unsigned_abs`](https://doc.rust-lang.org/std/primitive.i32.html#method.unsigned_abs)
  returns the magnitude in the unsigned counterpart, `128u8` for `-128i8`, as `unsigned_abs` does.
- **`to_chars`:** [P0067R5](https://wg21.link/p0067r5), *Elementary string conversions, revision 5* (Jens Maurer), and
  [P2291R3](https://wg21.link/p2291r3), *Add Constexpr Modifiers to Functions to_chars and from_chars for Integral
  Types in `<charconv>` Header* (Daniil Goncharov, Karaev Alexander), which makes the integer overloads `constexpr`.
- **`formatter<xstd::div_result<I>, charT>`:** [P2286R8](https://wg21.link/p2286r8), *Formatting Ranges* (Barry
  Revzin), for the tuple formatter it builds on, and [P3391R2](https://wg21.link/p3391r2), *`constexpr std::format`*
  (Barry Revzin).
- **`align_up`, `align_down`, `is_aligned`:**
  [Boost.Align](https://www.boost.org/doc/libs/latest/doc/html/align/reference.html)'s functions of the same names,
  with which they agree on values and pointers, and LLVM's
  [`alignTo` and `alignDown`](https://llvm.org/doxygen/namespacellvm.html). [N4201](https://wg21.link/n4201),
  *Alignment Helpers for C++* (Matthew Fioravante), proposes alignment helpers for the standard.
  [N3864](https://wg21.link/n3864) proposed `is_aligned`, `align_up` and `align_down` before it, on integers and on
  pointers.
- **`hash_append_int`:** [Boost.Hash2](https://www.boost.org/doc/libs/latest/libs/hash2/)'s `hash_append`, which it
  calls, from [N3980](https://wg21.link/n3980), *Types Don't Know #* (Howard Hinnant, Vinnie Falco, John Bytheway).

## Headers

Each group of headers has a page, and each header a page under it:

- [`<xstd/ints.hpp>`](ints/index.md)
- [`<xstd/ints/bit.hpp>`](bit/index.md)
- [`<xstd/ints/charconv.hpp>`](charconv/index.md)
- [`<xstd/ints/concepts.hpp>`](concepts/index.md)
- [`<xstd/ints/cstdint.hpp>`](cstdint/index.md)
- [`<xstd/ints/cstdlib.hpp>`](cstdlib/index.md)
- [`<xstd/ints/ext/absl.hpp>`](absl/index.md)
- [`<xstd/ints/ext/boost.hpp>`](boost/index.md)
- [`<xstd/ints/format.hpp>`](format/index.md)
- [`<xstd/ints/limits.hpp>`](limits/index.md)
- [`<xstd/ints/memory.hpp>`](memory/index.md)
- [`<xstd/ints/type_traits.hpp>`](type_traits/index.md)
