#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""The portable bit kernels and their specifications, each written once over an interface of word operations.

Z3Ops builds Z3 bit-vector terms from them, for the proofs; IntOps evaluates them on Python integers reduced to the
word width, for the drift check against the compiled C++. Each kernel follows its C++ statement by statement: a loop
becomes `width` guarded iterations, enough for any word, and a guard that the C++ would have stopped at keeps the
state unchanged. The obligations a kernel records are what the C++ needs to be defined, or to stop: a shift below the
width, a precondition met, an index inside its table, a loop finished within `width` iterations.

The specifications transcribe their primary sources: PEXT and PDEP the Intel SDM's Operation pseudocode, RBIT the Arm
ARM's ASL, and select its defining property, that the k-th one has exactly k ones below it.
"""

import z3


class Z3Ops:
    """Words as Z3 bit-vector terms of one width; obligations as implications from their path."""

    def __init__(self, width):
        self.width = width
        self.true = z3.BoolVal(True)
        self.obligations = []

    def word(self, value):
        return value if isinstance(value, z3.BitVecRef) else z3.BitVecVal(value % (1 << self.width), self.width)

    def lshr(self, x, n):
        return z3.LShR(x, self.word(n))

    def shl(self, x, n):
        return self.word(x) << self.word(n)

    def bit(self, x, i):
        return z3.Extract(i, i, x) == 1

    def bit_at(self, x, n):
        return (self.lshr(x, n) & 1) == 1

    def popcount(self, x):
        return z3.Sum([z3.ZeroExt(self.width - 1, z3.Extract(i, i, x)) for i in range(self.width)])

    def countr_zero(self, x):
        result = self.word(self.width)
        for i in reversed(range(self.width)):
            result = z3.If(self.bit(x, i), self.word(i), result)
        return result

    def byteswap(self, x):
        return z3.Concat(*[z3.Extract(8 * i + 7, 8 * i, x) for i in range(self.width // 8)])

    def ite(self, condition, then, otherwise):
        return z3.If(condition, self.word(then), self.word(otherwise))

    def ult(self, a, b):
        return z3.ULT(self.word(a), self.word(b))

    def and_(self, *conditions):
        return z3.And(*conditions)

    def require(self, path, condition, what):
        self.obligations.append((what, z3.Implies(path, condition)))

    def table(self, entries, index, index_bits):
        array = z3.K(z3.BitVecSort(index_bits), z3.BitVecVal(0, 8))
        for i, value in enumerate(entries):
            if value:
                array = z3.Store(array, z3.BitVecVal(i, index_bits), z3.BitVecVal(value, 8))
        return z3.ZeroExt(self.width - 8, z3.Select(array, z3.Extract(index_bits - 1, 0, index)))


class Word:
    """An unsigned integer of a fixed width, its arithmetic reduced modulo 2^width as the C++ is."""

    __slots__ = ("value", "width")

    def __init__(self, value, width):
        self.value = value % (1 << width)
        self.width = width

    def _of(self, other):
        return other.value if isinstance(other, Word) else other

    def __and__(self, other):
        return Word(self.value & self._of(other), self.width)

    def __or__(self, other):
        return Word(self.value | self._of(other), self.width)

    def __xor__(self, other):
        return Word(self.value ^ self._of(other), self.width)

    def __invert__(self):
        return Word(~self.value, self.width)

    def __add__(self, other):
        return Word(self.value + self._of(other), self.width)

    def __sub__(self, other):
        return Word(self.value - self._of(other), self.width)

    def __rsub__(self, other):
        return Word(self._of(other) - self.value, self.width)

    def __mul__(self, other):
        return Word(self.value * self._of(other), self.width)

    def __eq__(self, other):
        return self.value == self._of(other)

    def __ne__(self, other):
        return self.value != self._of(other)

    def __hash__(self):
        return hash(self.value)


class IntOps:
    """Words as Python integers of one width; an obligation that fails on the path taken is an error."""

    def __init__(self, width):
        self.width = width
        self.true = True

    def word(self, value):
        return value if isinstance(value, Word) else Word(value, self.width)

    def lshr(self, x, n):
        n = self.word(n).value
        return Word(x.value >> n if n < self.width else 0, self.width)

    def shl(self, x, n):
        n = self.word(n).value
        return Word(self.word(x).value << n if n < self.width else 0, self.width)

    def bit(self, x, i):
        return (x.value >> i) & 1 == 1

    def bit_at(self, x, n):
        return (self.lshr(x, n).value & 1) == 1

    def popcount(self, x):
        return Word(x.value.bit_count(), self.width)

    def countr_zero(self, x):
        return Word((x.value & -x.value).bit_length() - 1 if x.value else self.width, self.width)

    def byteswap(self, x):
        return Word(int.from_bytes(x.value.to_bytes(self.width // 8, "little"), "big"), self.width)

    def ite(self, condition, then, otherwise):
        return self.word(then) if condition else self.word(otherwise)

    def ult(self, a, b):
        return self.word(a).value < self.word(b).value

    def and_(self, *conditions):
        return all(conditions)

    def require(self, path, condition, what):
        if path and not condition:
            raise AssertionError(what)

    def table(self, entries, index, index_bits):
        return Word(entries[index.value % (1 << index_bits)], self.width)


# The constants each kernel derives from ~0UZ, at the word width.
def ones(o):
    return (1 << o.width) - 1


def block_reverse_by_swaps(o, x):
    nibbles = ones(o) // 0x11
    pairs = ones(o) // 0x05
    singles = ones(o) // 0x03
    x = o.byteswap(x)
    x = (o.lshr(x, 4) & nibbles) | o.shl(x & nibbles, 4)
    x = (o.lshr(x, 2) & pairs) | o.shl(x & pairs, 2)
    x = (o.lshr(x, 1) & singles) | o.shl(x & singles, 1)
    return x


def block_compress_by_rank(o, src, mask):
    dst = o.word(0)
    selected = src & mask
    for _ in range(o.width):
        active = selected != 0
        below_lowest = (selected - 1) & ~selected
        amount = o.popcount(below_lowest & mask)
        o.require(active, o.ult(amount, o.width), "block_compress_by_rank: 1UZ << popcount within the width")
        dst = o.ite(active, dst | o.shl(1, amount), dst)
        selected = o.ite(active, selected & (selected - 1), selected)
    o.require(o.true, selected == 0, "block_compress_by_rank: finished within the width's iterations")
    return dst


def block_expand_by_walk(o, src, mask):
    dst = o.word(0)
    for _ in range(o.width):
        active = mask != 0
        lowest = mask & ~(mask - 1)
        dst = o.ite(active, dst | (lowest & (0 - (src & 1))), dst)
        mask = o.ite(active, mask & (mask - 1), mask)
        src = o.ite(active, o.lshr(src, 1), src)
    o.require(o.true, mask == 0, "block_expand_by_walk: finished within the width's iterations")
    return dst


BYTE_DIGITS = 8
BYTE_VALUES = 1 << BYTE_DIGITS


def select_in_byte():
    """The position of the k-th one of each byte value, at index byte | (k << 8), as the C++ table builds it."""
    table = [0] * (BYTE_VALUES * BYTE_DIGITS)
    for byte in range(BYTE_VALUES):
        rank = 0
        for bit in range(BYTE_DIGITS):
            if (byte >> bit) & 1:
                table[byte | (rank << BYTE_DIGITS)] = bit
                rank += 1
    return table


SELECT_IN_BYTE = select_in_byte()


def block_select_by_broadword(o, x, k, path=None, table=SELECT_IN_BYTE):
    path = o.true if path is None else path
    o.require(path, o.ult(k, o.popcount(x)), "block_select_by_broadword: precondition k < popcount(x)")
    byte_mask = BYTE_VALUES - 1
    ones_per_byte = ones(o) // byte_mask
    byte_tops = (ones_per_byte << (BYTE_DIGITS - 1)) % (1 << o.width)
    pair_lows = ones(o) // 0x03
    nibble_lows = ones(o) // 0x05
    byte_lows = ones(o) // 0x11
    counts = x - (o.lshr(x, 1) & pair_lows)
    counts = (counts & nibble_lows) + (o.lshr(counts, 2) & nibble_lows)
    counts = ((counts + o.lshr(counts, 4)) & byte_lows) * ones_per_byte
    bytes_below = o.popcount((((o.word(k) * ones_per_byte) | byte_tops) - counts) & byte_tops)
    offset = o.word(BYTE_DIGITS) * bytes_below
    o.require(path, o.ult(offset, o.width), "block_select_by_broadword: shifts by offset within the width")
    rank_in_byte = o.word(k) - (o.lshr(o.shl(counts, BYTE_DIGITS), offset) & byte_mask)
    # Below 8 in the word, so it never wrapped, so the C++ unsigned arithmetic gives the same small value.
    o.require(path, o.ult(rank_in_byte, BYTE_DIGITS), "block_select_by_broadword: rank within the byte below 8")
    index = (o.lshr(x, offset) & byte_mask) | o.shl(rank_in_byte, BYTE_DIGITS)
    o.require(path, o.ult(index, len(table)), "block_select_by_broadword: index inside select_in_byte")
    return offset + o.table(table, index, 11)


def block_expand_by_select(o, src, mask, select=block_select_by_broadword):
    mask_ones = o.popcount(mask)
    src = o.ite(o.ult(mask_ones, o.width), src & (o.shl(1, mask_ones) - 1), src)
    dst = o.word(0)
    for _ in range(o.width):
        active = src != 0
        position = select(o, mask, o.countr_zero(src), active)
        o.require(active, o.ult(position, o.width), "block_expand_by_select: 1UZ << select within the width")
        dst = o.ite(active, dst | o.shl(1, position), dst)
        src = o.ite(active, src & (src - 1), src)
    o.require(o.true, src == 0, "block_expand_by_select: finished within the width's iterations")
    return dst


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
