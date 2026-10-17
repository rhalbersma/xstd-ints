#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""The reference digests for the exhaustive division sweep, computed by numpy rather than by xstd.

For every 16-bit numerator and nonzero denominator, signed and unsigned, it divides with numpy's own integer
routines: fmod rounds toward zero as C does, and // and % round toward negative infinity as Python does. The
Euclidean and ceiling divisions are derived from those by their defining identities. Each (numerator,
denominator, quotient, remainder) is packed into one 64-bit word, passed through SplitMix64's finalizer, and
summed modulo 2^64, which is the digest the C++ sweep computes over xstd's results.

    division_digests.py --write exhaustive/division_digests.txt
    division_digests.py --check exhaustive/division_digests.txt
"""

import argparse
import multiprocessing
import os
import sys

import numpy as np

MASK16 = np.uint64(0xFFFF)


def mix(x):
    """SplitMix64's finalizer, elementwise and modulo 2^64."""
    x = x + np.uint64(0x9E3779B97F4A7C15)
    x = (x ^ (x >> np.uint64(30))) * np.uint64(0xBF58476D1CE4E5B9)
    x = (x ^ (x >> np.uint64(27))) * np.uint64(0x94D049BB133111EB)
    return x ^ (x >> np.uint64(31))


def digest(n, d, q, r):
    """The wrapping sum of the mixed packed words, each operand reduced to its 16-bit pattern."""
    words = ((n.astype(np.uint64) & MASK16) << np.uint64(48)) | ((d.astype(np.uint64) & MASK16) << np.uint64(32)) \
        | ((q.astype(np.uint64) & MASK16) << np.uint64(16)) | (r.astype(np.uint64) & MASK16)
    return int(np.sum(mix(words), dtype=np.uint64))


def signed_roundings(n, d):
    """Quotient and remainder of each rounding, for int64 arrays holding int16 values."""
    r_trunc = np.fmod(n, d)
    q_trunc = (n - r_trunc) // d
    q_floor, r_floor = n // d, n % d
    r_euclid = n % np.abs(d)
    q_euclid = (n - r_euclid) // d
    q_ceil = -((-n) // d)
    r_ceil = n - q_ceil * d
    return {"div": (q_trunc, r_trunc), "div_floor": (q_floor, r_floor), "div_euclid": (q_euclid, r_euclid), "div_ceil": (q_ceil, r_ceil)}


def unsigned_roundings(n, d):
    """Quotient and remainder of each rounding, for int64 arrays holding uint16 values; the ceiling's remainder wraps."""
    q, r = n // d, n % d
    q_ceil = -((-n) // d)
    r_ceil = (n - q_ceil * d) % 0x10000
    return {"div": (q, r), "div_floor": (q, r), "div_euclid": (q, r), "div_ceil": (q_ceil, r_ceil)}


NAMES = ("div", "div_floor", "div_euclid", "div_ceil")


def rows(numerators):
    """The eight partial digests over the given 16-bit numerator patterns, against every denominator in the domain."""
    sums = {(t, f): 0 for t in ("int16", "uint16") for f in NAMES}
    signed_d = np.arange(-0x8000, 0x8000, dtype=np.int64)
    signed_d = signed_d[signed_d != 0]
    unsigned_d = np.arange(1, 0x10000, dtype=np.int64)
    with np.errstate(over="ignore"):
        for bits in numerators:
            signed_n = bits - 0x10000 if bits >= 0x8000 else bits
            d = signed_d[signed_d != -1] if signed_n == -0x8000 else signed_d
            n = np.full_like(d, signed_n)
            for f, (q, r) in signed_roundings(n, d).items():
                sums[("int16", f)] += digest(n, d, q, r)
            n = np.full_like(unsigned_d, bits)
            for f, (q, r) in unsigned_roundings(n, unsigned_d).items():
                sums[("uint16", f)] += digest(n, unsigned_d, q, r)
    return sums


def digests():
    """The eight digests, one per type and rounding, over every pair in the functions' domain, the rows across processes."""
    workers = os.cpu_count() or 1
    with multiprocessing.Pool(workers) as pool:
        parts = pool.map(rows, [range(w, 0x10000, workers) for w in range(workers)])
    return {key: sum(part[key] for part in parts) % 2**64 for key in parts[0]}


def render(table):
    return "".join(f"{t} {f} {value:x}\n" for (t, f), value in table.items())


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument("--write", metavar="FILE")
    action.add_argument("--check", metavar="FILE")
    args = parser.parse_args()
    text = render(digests())
    if args.write:
        with open(args.write, "w", encoding="ascii") as out:
            out.write(text)
        return 0
    with open(args.check, encoding="ascii") as checked_in:
        if checked_in.read() != text:
            sys.stderr.write(f"{args.check} is not what numpy computes:\n{text}")
            return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
