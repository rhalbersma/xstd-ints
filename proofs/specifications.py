#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""What the portable bit kernels compute, transcribed from the definitions they answer to.

PEXT and PDEP follow the Operation pseudocode of the Intel 64 and IA-32 Architectures Software Developer's Manual,
RBIT the ASL of the Arm Architecture Reference Manual for A-profile, and select its defining property. Each is written
over the same word operations as the kernels, in operations.py, so it builds Z3 terms and evaluates on integers alike.
"""


def pext(o, src, mask):
    """Intel SDM, PEXT, Operation: each mask bit m that is set copies TEMP[m] to DEST[k], then k = k + 1."""
    dest = o.word(0)
    k = o.word(0)
    for m in range(o.width):
        selected = o.bit(mask, m)
        dest = o.ite(selected, dest | o.shl(o.lshr(src, m) & 1, k), dest)
        k = o.ite(selected, k + 1, k)
    return dest


def pdep(o, src, mask):
    """Intel SDM, PDEP, Operation: each mask bit m that is set copies TEMP[k] to DEST[m], then k = k + 1."""
    dest = o.word(0)
    k = o.word(0)
    for m in range(o.width):
        selected = o.bit(mask, m)
        dest = o.ite(selected, dest | o.shl(o.lshr(src, k) & 1, m), dest)
        k = o.ite(selected, k + 1, k)
    return dest


def rbit(o, x):
    """Arm ARM, RBIT, ASL: for i = 0 to datasize-1, result[datasize-1-i] = operand[i]."""
    result = o.word(0)
    for i in range(o.width):
        result = result | o.shl(o.lshr(x, i) & 1, o.width - 1 - i)
    return result


def selects(o, x, k, position):
    """Select's definition: position names a one of x, and exactly k ones of x lie below it."""
    below = o.shl(1, position) - 1
    return o.and_(o.ult(position, o.width), o.bit_at(x, position), o.popcount(x & below) == o.word(k))
