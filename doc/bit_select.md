    Document Number: DXXXXR0 (draft; no number requested yet)
    Date:            2026-10-17
    Reply-to:        Rein Halbersma <rhalbersma at gmail dot com>
    Audience:        SG6, LEWG

`bit_select`: the position of the k-th set bit
===============================================

1 Introduction
--------------

We propose one function template for `<bit>`:

    template<class T>
      constexpr int bit_select(T x, int k);

It returns the position of the `k`-th set bit of `x`, counting from zero at the least significant end, for every
unsigned integer type `x` can have. It is the inverse of a masked `popcount`: if `p == bit_select(x, k)`, exactly `k`
bits of `x` below position `p` are set, and bit `p` is set.

    static_assert(std::bit_select(0b1011'0100u, 0) == 2);
    static_assert(std::bit_select(0b1011'0100u, 2) == 5);
    static_assert(std::bit_select(0b1011'0100u, 3) == 7);

2 Motivation and Scope
----------------------

**2.1** *Select* and *rank* are the two primitives of succinct data structures: bit vectors with constant-time rank and
select queries underlie compressed suffix arrays and FM-indexes, wavelet trees, Elias–Fano coded sequences and succinct
trees [Navarro]. Rank on a word is a masked `popcount`, which `<bit>` already has. Select on a word has no standard
spelling, so every library in the field carries its own: sux's `select64` [sux], SDSL's `bits::sel` [SDSL], folly's
`select64` [folly], among others (section 4.1).

**2.2** Outside that field, select is how a bitset's iterator advances `n` positions at once, how a sparse set maps a
dense index to a member, and how a uniformly random element of a bit set is drawn: pick `k` uniformly below
`popcount(x)` and return `bit_select(x, k)`.

**2.3** The function is short to state and easy to get wrong to implement. The obvious loop is linear in the position
of the result. The fast portable method, broadword select [Vigna], is about twenty operations whose correctness rests on
a byte-wise prefix population count and a lookup table. On x86-64 with BMI2 it is two instructions,
`TZCNT(PDEP(1 << k, x))`, except on processors whose `PDEP` is microcoded, where it is the slowest choice (section 5.3).
Getting the dispatch right per target is the kind of work the standard library is placed to do once.

**2.4** Scope: one function template, for every unsigned integer type as `<bit>` already defines that set, with a
precondition and no new types. It does not propose a rank function; section 4.4 explains why, and the alternative.

3 Proposed interface
--------------------

    namespace std {
      template<class T>
        constexpr int bit_select(T x, int k);
    }

- **Constraints:** `T` is an unsigned integer type, exactly as for `popcount` and `countr_zero` ([bit.count]).
- **Preconditions:** `0 <= k < popcount(x)`.
- **Returns:** the smallest `i` such that `k + 1` bits of `x` at positions `0` through `i` are set.
- **Remarks:** a call that violates the precondition is not a core constant expression, as for `bit_ceil`.
- **Not `noexcept`:** the contract is narrow, as for `bit_ceil`.

4 Design decisions
------------------

**4.1 Prior art.** *(Survey in progress: index base, argument order, return type and out-of-range behaviour for sux,
SDSL, folly, the Rust crates, and any C or Rust proposal. To be filled in before review.)*

**4.2 Index base.** `k` counts from zero, so that `bit_select(x, 0) == countr_zero(x)` for every nonzero `x`, and so that
`bit_select(x, k)` for `k` in `[0, popcount(x))` enumerates the set bits in order, as a loop over a range would. A
one-based `k` would match SDSL's container-level `select_support`, but not `<bit>`, where every position counts from zero.

**4.3 Out of range.** `k >= popcount(x)`, or `k < 0`, violates a precondition rather than returning a sentinel. The
alternatives considered:

- *Return `numeric_limits<T>::digits`*, as `countr_zero(0)` does. This makes `bit_select(x, k)` total, at the cost of a
  comparison on the fast path: `TZCNT(PDEP(1 << k, x))` already returns 64 when the `k`-th one does not exist, but only
  when `k < 64`, and the portable kernel would need an explicit test. It also gives `k < 0` a meaning it should not have.
- *Return `-1`*, which no other `<bit>` function does.

A precondition keeps the function a single expression where the hardware has one, lets hardened implementations check
it, and makes a violating call ill-formed in constant evaluation (section 3). *(To be revisited once 4.1 shows what the
libraries do.)*

**4.4 Rank.** Rank on a word, the number of set bits below position `i`, is `popcount(x & ((T(1) << i) - 1))` for `i` in
`[0, digits)`. At `i == digits` the shift is undefined, which is the one argument for a `bit_rank(x, i)` defined on
`[0, digits]`. We do not propose it here: the masked `popcount` is the spelling every surveyed library uses, and the
`i == digits` case is `popcount(x)`. *(To be revisited with 4.1.)*

**4.5 Return type.** `int`, as `countr_zero`, `countl_zero`, `popcount` and `bit_width` return. `k` is `int` for the
same reason and because it is compared against `popcount(x)`.

**4.6 Name.** `bit_select` follows `bit_ceil`, `bit_floor`, `bit_width` and P3104's `bit_compress`/`bit_expand`. The name
"select" is the field's own [Jacobson; Vigna]. *(Naming risk to check: OpenCL's `bitselect(a, b, c)` is a bitwise
multiplexer; whether any std::simd or `<bit>` proposal uses `select` for that meaning.)*

5 Implementation experience
---------------------------

**5.1** xstd-ints [xstd] implements this interface for every unsigned integer type it supports: the standard ones,
`unsigned __int128`, `unsigned _BitInt(N)` of any width, and integer classes such as `absl::uint128` and
`boost::int128::uint128`. A type wider than 64 bits is processed a 64-bit block at a time: whole blocks are skipped by
their population until the `k`-th one lies in the current block, which is then answered by the block kernel.

**5.2** The block kernel is chosen at compile time, outside constant evaluation:

- with fast BMI2 on x86-64, `countr_zero(_pdep_u64(1 << k, x))`, which GCC and Clang compile to `SHLX`, `PDEP` and
  `TZCNT` with no branch;
- otherwise, and always in constant evaluation, Vigna's broadword select [Vigna]: a byte-wise prefix population count by
  one multiplication, the byte holding the `k`-th one found by a comparison against all eight prefixes at once, and the
  position within the byte read from a 2 KiB table.

**5.3** The BMI2 path is excluded on targets where `PDEP` is microcoded, AMD's Excavator and Zen 1 and 2 [uops.info],
where its latency is an order of magnitude above Zen 3's and Intel's since Haswell, and grows with the operands.

**5.4 Measurements.** Cycles per call, over 4096 independent calls on pseudo-random words, the fastest of five passes,
from the checked-in benchmark pages [xstd-bench]:

| Machine | Compiler | broadword | `PDEP`, `TZCNT` |
| :-- | :-- | --: | --: |
| Cascade Lake (cloud VM) | GCC 15 / Clang 22 | 12.1 / 11.7 | 3.0 / 2.0 |
| Emerald Rapids (GitHub runner) | GCC 15 / Clang 22 | 8.4 / 8.2 | 2.0 / 2.0 |
| Neoverse N2 (GitHub runner) | GCC 15 / Clang 22 | 10.0 / 8.9 | — |

The harness alone costs 1.2 to 2.1 cycles a call on the same machines.

**5.5 In a succinct data structure.** A rank/select dictionary built on `bit_select` and `popcount`, over 2^20 and 2^30
bits at densities from 1% to 90%, agrees with sux's `Rank9Sel` and SDSL's `select_support_mcl` on every one of millions
of queries [xstd-succinct]. With BMI2, replacing `bit_select` by one `countr_zero` changes a query's cost by less than the
run-to-run noise: the in-word select is a small part of a query, which is dominated by finding the word. Without BMI2,
the broadword kernel costs about 10 ns of a 60 to 70 ns query when the vector is in cache. The case for standardizing is
therefore that every such library needs the step and writes its own, not that it is a bottleneck.

**5.6 Verification.**

- *Proof:* Z3 proves that the broadword kernel returns the position of the `k`-th one for every 64-bit `x` and every
  `k < popcount(x)`, and at 16 bits [xstd-proofs].
- *Oracle tests:* every supported type, edge values and pseudo-random values, against a bit-at-a-time reference, at run
  time and in constant evaluation.
- *Fuzzing:* under libFuzzer with ASan and UBSan, `bit_select` against the oracle, its result bit set with exactly `k`
  set bits below it, the portable kernel against the oracle at every `k`, and the BMI2 path against the portable one.
- *Codegen tests:* the BMI2 build must contain `PDEP` then `TZCNT` and no branch; the portable build must contain no
  `PEXT` or `PDEP`.

**5.7** The BMI2 path and the portable kernel are offered to libstdc++ and libc++ alongside P3104's permutations.

6 Impact on the Standard
------------------------

A pure library addition to `<bit>`. It depends on nothing beyond what `<bit>` has, and an implementation that has
`popcount` and `countr_zero` can implement it portably.

7 Proposed Wording
------------------

Wording relative to the working draft. *(Draft number to be filled in.)*

**7.1** In [version.syn], bump `__cpp_lib_bitops` to the date of adoption.

**7.2** In [bit.syn], after the declarations of [bit.count], add:

    // [bit.select], selecting
    template<class T>
      constexpr int bit_select(T x, int k);

**7.3** After [bit.count], add a new subclause:

> **Selecting [bit.select]**
>
>     template<class T>
>       constexpr int bit_select(T x, int k);
>
> *Constraints:* `T` is an unsigned integer type ([basic.fundamental]).
>
> *Preconditions:* `0 <= k` is `true` and `k < popcount(x)` is `true`.
>
> *Returns:* The smallest integer `i` such that `k + 1` of the bits of `x` at positions `0` through `i` have the value 1.
>
> *Remarks:* A function call expression that violates the precondition in the *Preconditions* element is not a core
> constant expression ([expr.const]).

8 Acknowledgments
-----------------

*(To be written.)*

9 References
------------

- [Jacobson] G. Jacobson, *Space-efficient static trees and graphs*, FOCS 1989.
- [Navarro] G. Navarro, *Compact Data Structures*, Cambridge University Press, 2016.
- [Vigna] S. Vigna, *Broadword implementation of rank/select queries*, WEA 2008.
- [sux] https://github.com/vigna/sux
- [SDSL] https://github.com/xxsds/sdsl-lite
- [folly] https://github.com/facebook/folly
- [uops.info] https://uops.info/html-instr/PDEP_R64_R64_R64.html
- [xstd] https://github.com/rhalbersma/xstd-ints
- [xstd-bench] https://github.com/rhalbersma/xstd-ints/tree/main/bench/results
- [xstd-succinct] https://github.com/rhalbersma/xstd-ints/blob/main/bench/succinct/results.md
- [xstd-proofs] https://github.com/rhalbersma/xstd-ints/tree/main/proofs
- [P3104] J. Müller, *Bit permutations*, https://wg21.link/P3104
