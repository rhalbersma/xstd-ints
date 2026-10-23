#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""Proves each portable bit kernel equal to its specification for every input, at each std::size_t width.

Each claim is a formula Z3 must find no counterexample to; together they prove, for every input:

- block_reverse_by_swaps is RBIT, directly.
- block_compress_by_rank is PEXT, and block_expand_by_walk and block_expand_by_select are PDEP, by an inductive
  invariant per loop: it holds on entry, one pass of the body keeps it while a variable falls, so the loop ends, and on
  exit it makes the result a closed form of the instruction, which a further claim proves equal to the SDM's
  pseudocode. Every shift is within the width wherever the C++ executes it.
- block_select_by_broadword finds the k-th one wherever k < popcount(x): its counts are the prefix populations of the
  bytes, and from any such counts the rest of the kernel lands on the k-th one, a case for each byte and each bit
  within it. block_expand_by_select relies on this, not on the select's code, and meets its precondition at every call.

Three mutants, each a kernel with one deliberate error, must be refuted, which shows a passing proof is not vacuous.

    prove.py [--widths 32,64] [--jobs N]
"""

import argparse
import multiprocessing
import os
import sys
import time

import z3

import kernels
import operations
import specifications


def lowest_one(x):
    return x & (0 - x)


def below_lowest_one(processed, remaining):
    """Every processed bit lies below the lowest remaining one: the loop takes ones lowest first."""
    return z3.Implies(remaining != 0, processed & ~(lowest_one(remaining) - 1) == 0)


def rank(o, mask, m):
    """The ones of mask below the constant position m."""
    return o.popcount(mask & ((1 << m) - 1))


def closed_pext(o, src, mask):
    """PEXT in closed form: each bit m of src that mask selects lands at position rank(m)."""
    dst = o.word(0)
    for m in range(o.width):
        dst = dst | o.ite(o.and_(o.bit(src, m), o.bit(mask, m)), o.shl(1, rank(o, mask, m)), 0)
    return dst


def closed_pdep(o, src, mask, positions):
    """PDEP in closed form, at the given positions of mask: each takes bit rank(m) of src."""
    dst = o.word(0)
    for m in range(o.width):
        dst = dst | o.ite(o.bit(positions, m), o.shl(o.lshr(src, rank(o, mask, m)) & 1, m), 0)
    return dst


def words(o, *names):
    return [z3.BitVec(name, o.width) for name in names]


def with_obligations(o, formula):
    return z3.And(formula, *[condition for _, condition in o.obligations])


# Claims: each maps a Z3Ops to the formula to prove valid, obligations included.

def reverse_is_rbit(o, reverse=kernels.block_reverse_by_swaps):
    (x,) = words(o, "x")
    return reverse(o, x) == specifications.rbit(o, x)


def closed_pext_is_sdm(o):
    src, mask = words(o, "src", "mask")
    return closed_pext(o, src, mask) == specifications.pext(o, src, mask)


def closed_pdep_is_sdm(o):
    src, mask = words(o, "src", "mask")
    return closed_pdep(o, src, mask, mask) == specifications.pdep(o, src, mask)


def compress_invariant(o, src, mask, selected, dst):
    chosen = src & mask
    processed = chosen & ~selected
    return z3.And(selected & ~chosen == 0, below_lowest_one(processed, selected), dst == closed_pext(o, processed, mask))


def compress_claim(part):
    def claim(o):
        src, mask, selected, dst = words(o, "src", "mask", "selected", "dst")
        if part == "entry":
            return compress_invariant(o, src, mask, *kernels.compress_start(o, src, mask))
        holds = compress_invariant(o, src, mask, selected, dst)
        if part == "pass":
            path = z3.And(holds, kernels.compress_going(o, (selected, dst)))
            after, dst_after = kernels.compress_step(o, mask, (selected, dst), path)
            return with_obligations(o, z3.Implies(path, z3.And(compress_invariant(o, src, mask, after, dst_after), z3.ULT(after, selected))))
        return z3.Implies(z3.And(holds, z3.Not(kernels.compress_going(o, (selected, dst)))), dst == closed_pext(o, src, mask))
    return claim


def walk_invariant(o, src0, mask0, src, mask, dst):
    processed = mask0 & ~mask
    return z3.And(mask & ~mask0 == 0, below_lowest_one(processed, mask), src == o.lshr(src0, o.popcount(processed)),
                  dst == closed_pdep(o, src0, mask0, processed))


def walk_claim(part, step=kernels.walk_step):
    def claim(o):
        src0, mask0, src, mask, dst = words(o, "src0", "mask0", "src", "mask", "dst")
        if part == "entry":
            return walk_invariant(o, src0, mask0, *kernels.walk_start(o, src0, mask0))
        holds = walk_invariant(o, src0, mask0, src, mask, dst)
        if part == "pass":
            path = z3.And(holds, kernels.walk_going(o, (src, mask, dst)))
            src_after, mask_after, dst_after = step(o, (src, mask, dst), path)
            return with_obligations(o, z3.Implies(path, z3.And(walk_invariant(o, src0, mask0, src_after, mask_after, dst_after), z3.ULT(mask_after, mask))))
        return z3.Implies(z3.And(holds, z3.Not(kernels.walk_going(o, (src, mask, dst)))), dst == closed_pdep(o, src0, mask0, mask0))
    return claim


def prefix_populations(o, x, counts):
    """Byte i of counts holds the ones of x in bytes 0 to i."""
    return z3.And(*[z3.ZeroExt(o.width - 8, z3.Extract(8 * i + 7, 8 * i, counts)) == o.popcount(x & ((1 << (8 * (i + 1))) - 1))
                    for i in range(o.width // 8)])


def select_counts_claim(o):
    (x,) = words(o, "x")
    return prefix_populations(o, x, kernels.select_counts(o, x))


def select_case(byte, bit):
    """From any counts of x's prefix populations, the k-th one is found at 8 * byte + bit, wherever the kernel says so.

    The table stands in by its contract, which select_in_byte_holds checks entry by entry: its answer is a one of the
    byte with exactly the asked rank of ones below it.
    """
    def claim(o):
        x, k, counts, answer = words(o, "x", "k", "counts", "answer")
        contract = []

        def lookup(o, byte_value, rank_in_byte):
            contract.append(z3.And(o.ult(answer, kernels.BYTE_DIGITS), o.bit_at(byte_value, answer),
                                   o.popcount(byte_value & (o.shl(1, answer) - 1)) == rank_in_byte))
            return answer

        path = z3.And(o.ult(k, o.popcount(x)), prefix_populations(o, x, counts))
        position, bytes_below, _ = kernels.select_from_counts(o, x, k, counts, path, lookup=lookup)
        found = 8 * byte + bit
        case = z3.And(path, bytes_below == byte, answer == bit, *contract)
        return with_obligations(o, z3.Implies(case, z3.And(position == found, specifications.selects(o, x, k, o.word(found)))))
    return claim


def select_byte_in_range(o):
    x, k, counts = words(o, "x", "k", "counts")
    path = z3.And(o.ult(k, o.popcount(x)), prefix_populations(o, x, counts))
    _, bytes_below, _ = kernels.select_from_counts(o, x, k, counts, path)
    return z3.Implies(path, z3.ULT(bytes_below, o.width // 8))


def select_expand_invariant(o, chosen, mask, src, dst):
    processed = chosen & ~src
    return z3.And(src & ~chosen == 0, below_lowest_one(processed, src), dst == closed_pdep(o, processed, mask, mask))


def select_expand_claim(part, position=None):
    def claim(o):
        src0, mask, src, dst, p = words(o, "src0", "mask", "src", "dst", "p")
        chosen, _ = kernels.select_expand_start(o, src0, mask)
        if part == "entry":
            return select_expand_invariant(o, chosen, mask, *kernels.select_expand_start(o, src0, mask))
        holds = select_expand_invariant(o, chosen, mask, src, dst)
        going = kernels.select_expand_going(o, (src, dst))
        if part == "precondition":
            return z3.Implies(z3.And(holds, going), o.ult(o.countr_zero(src), o.popcount(mask)))
        if part == "pass":
            # The select as its proved contract, not its code: a position that is the k-th one of the mask.
            k = o.countr_zero(src)
            path = z3.And(holds, going, p == position, specifications.selects(o, mask, k, o.word(position)))
            src_after, dst_after = kernels.select_expand_step(o, mask, (src, dst), path, select=lambda o, m, k, path: p)
            return with_obligations(o, z3.Implies(path, z3.And(select_expand_invariant(o, chosen, mask, src_after, dst_after), z3.ULT(src_after, src))))
        return z3.Implies(z3.And(holds, z3.Not(going)), dst == closed_pdep(o, src0, mask, mask))
    return claim


def wrong_table():
    """The select table with the last one of 0xFF, at rank 7, answered as bit 6."""
    table = list(kernels.SELECT_IN_BYTE)
    table[0xFF | (7 << kernels.BYTE_DIGITS)] = 6
    return table


def reverse_without_pairs(o, x):
    """The reversal with its middle swap, of bit pairs, left out."""
    nibbles = kernels.ones(o) // 0x11
    singles = kernels.ones(o) // 0x03
    x = o.byteswap(x)
    x = (o.lshr(x, 4) & nibbles) | o.shl(x & nibbles, 4)
    return (o.lshr(x, 1) & singles) | o.shl(x & singles, 1)


def walk_step_ignoring_src(o, state, path):
    """The walk depositing every one of the mask, whatever the bit of src it should take."""
    src, mask, dst = state
    return (o.lshr(src, 1), mask & (mask - 1), dst | (mask & ~(mask - 1)))


def claims(width):
    """Every claim at this width, in order, as (name, builder, expected to hold)."""
    found = [
        ("select_in_byte holds the k-th one of every byte, entry by entry", lambda o: kernels.select_in_byte_holds(), True),
        ("block_reverse_by_swaps is RBIT", reverse_is_rbit, True),
        ("PEXT's closed form is the SDM's", closed_pext_is_sdm, True),
        ("PDEP's closed form is the SDM's", closed_pdep_is_sdm, True),
    ]
    found += [(f"block_compress_by_rank: invariant on {part}", compress_claim(part), True) for part in ("entry", "pass", "exit")]
    found += [(f"block_expand_by_walk: invariant on {part}", walk_claim(part), True) for part in ("entry", "pass", "exit")]
    found += [("block_select_by_broadword: counts are prefix populations", select_counts_claim, True),
              ("block_select_by_broadword: the byte found lies within the word", select_byte_in_range, True)]
    found += [(f"block_select_by_broadword: byte {byte}, bit {bit}", select_case(byte, bit), True)
              for byte in range(width // 8) for bit in range(8)]
    found += [(f"block_expand_by_select: invariant on {part}", select_expand_claim(part), True) for part in ("entry", "precondition", "exit")]
    found += [(f"block_expand_by_select: invariant on a pass selecting position {c}", select_expand_claim("pass", c), True)
              for c in range(width)]
    found += [
        ("mutant: a reversal without its swap of bit pairs", lambda o: reverse_is_rbit(o, reverse_without_pairs), False),
        ("mutant: a walk depositing whatever the source bit", walk_claim("pass", walk_step_ignoring_src), False),
        ("mutant: a select table answering bit 6 for the eighth one of 0xFF", lambda o: kernels.select_in_byte_holds(wrong_table()), False),
    ]
    return found


def solve(task):
    """One claim at one width, in a process of its own: whether it holds, how long it took, and any counterexample."""
    width, index = task
    name, build, expected = claims(width)[index]
    o = operations.Z3Ops(width)
    formula = build(o)
    if isinstance(formula, bool):
        return width, name, expected, "unsat" if formula else "sat", 0.0, "" if formula else "an entry differs"
    solver = z3.SolverFor("QF_ABV")
    solver.add(z3.Not(formula))
    start = time.monotonic()
    result = solver.check()
    elapsed = time.monotonic() - start
    detail = ""
    if result == z3.sat:
        model = solver.model()
        detail = ", ".join(f"{d.name()} = {model[d]}" for d in model.decls())
        broken = [what for what, condition in o.obligations if z3.is_false(model.eval(condition, model_completion=True))]
        detail += f"; breaks {broken}" if broken else ""
    return width, name, expected, str(result), elapsed, detail


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--widths", default="32,64", help="comma-separated std::size_t widths, each a multiple of 8 from 16")
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 1)
    args = parser.parse_args()
    tasks = [(width, index) for width in (int(w) for w in args.widths.split(",")) for index in range(len(claims(width)))]
    as_expected = True
    with multiprocessing.get_context("spawn").Pool(args.jobs) as pool:
        for width, name, expected, result, elapsed, detail in pool.imap(solve, tasks):
            holds = result == "unsat"
            verdict = "proved" if holds else ("refuted" if result == "sat" else result)
            ok = holds == expected and result in ("sat", "unsat")
            as_expected &= ok
            print(f"{width:>2} bits  {verdict:<8} {elapsed:7.1f} s  {name}" + (f": {detail}" if detail and not ok or not expected else "")
                  + ("" if ok else "  <-- not as expected"), flush=True)
    return 0 if as_expected else 1


if __name__ == "__main__":
    sys.exit(main())
