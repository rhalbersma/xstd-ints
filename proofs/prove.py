#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""Proves each portable bit kernel equal to its specification for every input, at each std::size_t width.

A claim is proved when Z3 finds no input for which the kernel and its specification differ, or for which one of the
kernel's obligations fails: its shifts within the width, its precondition met, its table index in range, its loop
done within the width's iterations. A claim that fails prints the input Z3 found. Two mutants, each a kernel with one
deliberate error, must fail, which shows a passing proof is not vacuous.

    prove.py [--widths 32,64]
"""

import argparse
import sys
import time

import z3

import kernels


def refuted(claim, obligations):
    """The counterexample to the claim and every obligation together, or None where none exists."""
    solver = z3.SolverFor("QF_ABV")
    solver.add(z3.Not(z3.And(claim, *[condition for _, condition in obligations])))
    if solver.check() == z3.unsat:
        return None
    return solver.model()


def equal_to(kernel, specification, arity):
    """A claim that the kernel equals the specification on every input."""
    def build(o):
        inputs = [z3.BitVec(name, o.width) for name in ("src", "mask")[:arity]]
        return kernel(o, *inputs) == specification(o, *inputs), inputs
    return build


def selects_every_rank(select):
    """A claim that, wherever k < popcount(x), the select finds the k-th one of x."""
    def build(o):
        x, k = z3.BitVec("x", o.width), z3.BitVec("k", o.width)
        precondition = o.ult(k, o.popcount(x))
        return z3.Implies(precondition, kernels.selects(o, x, k, select(o, x, k, precondition))), [x, k]
    return build


CLAIMS = {
    "block_reverse_by_swaps is RBIT": equal_to(kernels.block_reverse_by_swaps, kernels.rbit, 1),
    "block_compress_by_rank is PEXT": equal_to(kernels.block_compress_by_rank, kernels.pext, 2),
    "block_expand_by_walk is PDEP": equal_to(kernels.block_expand_by_walk, kernels.pdep, 2),
    "block_expand_by_select is PDEP": equal_to(kernels.block_expand_by_select, kernels.pdep, 2),
    "block_select_by_broadword selects": selects_every_rank(kernels.block_select_by_broadword),
}


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


MUTANTS = {
    "a select table answering bit 6 for the eighth one of 0xFF": selects_every_rank(
        lambda o, x, k, path: kernels.block_select_by_broadword(o, x, k, path, wrong_table())),
    "a reversal without its swap of bit pairs": equal_to(reverse_without_pairs, kernels.rbit, 1),
}


def check(name, build, width, expect_proof):
    o = kernels.Z3Ops(width)
    claim, inputs = build(o)
    start = time.monotonic()
    model = refuted(claim, o.obligations)
    elapsed = time.monotonic() - start
    if model is None:
        print(f"{width:>2} bits  proved     {elapsed:7.1f} s  {name}")
        return expect_proof
    values = ", ".join(f"{v} = {model.eval(v, model_completion=True)}" for v in inputs)
    failed = [what for what, condition in o.obligations if z3.is_false(model.eval(condition, model_completion=True))]
    print(f"{width:>2} bits  refuted    {elapsed:7.1f} s  {name}: {values}" + (f"; fails {failed}" if failed else ""))
    return not expect_proof


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--widths", default="32,64", help="comma-separated std::size_t widths, each at least 16")
    args = parser.parse_args()
    widths = [int(w) for w in args.widths.split(",")]
    as_expected = True
    for width in widths:
        for name, build in CLAIMS.items():
            as_expected &= check(name, build, width, expect_proof=True)
        for name, build in MUTANTS.items():
            as_expected &= check(f"mutant: {name}", build, width, expect_proof=False)
    return 0 if as_expected else 1


if __name__ == "__main__":
    sys.exit(main())
