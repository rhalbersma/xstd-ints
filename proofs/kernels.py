#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""The portable bit kernels of include/xstd/ints/bit/detail/portable/, transcribed statement by statement.

Each kernel is written over the word operations of operations.py, at the width of std::size_t. A loop is given as its
parts, the state before it, the condition that keeps it going and one pass of its body, so a proof can reason about
one iteration and induct, and run() executes the loop for the drift check. The obligations a kernel records are what
the C++ needs to be defined there: a shift within the width, a precondition met, a table index in range.
"""


def ones(o):
    """~0UZ at the word width, from which each kernel derives its constants."""
    return (1 << o.width) - 1


def run(o, state, going, step):
    """The loop executed on integers: its body while its condition holds."""
    while going(o, state):
        state = step(o, state, o.true)
    return state


def block_reverse_by_swaps(o, x):
    nibbles = ones(o) // 0x11
    pairs = ones(o) // 0x05
    singles = ones(o) // 0x03
    x = o.byteswap(x)
    x = (o.lshr(x, 4) & nibbles) | o.shl(x & nibbles, 4)
    x = (o.lshr(x, 2) & pairs) | o.shl(x & pairs, 2)
    x = (o.lshr(x, 1) & singles) | o.shl(x & singles, 1)
    return x


# block_compress_by_rank: the state is (selected, dst), the loop runs while selected != 0.
def compress_start(o, src, mask):
    return (src & mask, o.word(0))


def compress_going(o, state):
    selected, _ = state
    return selected != 0


def compress_step(o, mask, state, path):
    selected, dst = state
    below_lowest = (selected - 1) & ~selected
    amount = o.popcount(below_lowest & mask)
    o.require(path, o.ult(amount, o.width), "block_compress_by_rank: 1UZ << popcount within the width")
    return (selected & (selected - 1), dst | o.shl(1, amount))


def block_compress_by_rank(o, src, mask):
    _, dst = run(o, compress_start(o, src, mask), compress_going, lambda o, s, p: compress_step(o, mask, s, p))
    return dst


# block_expand_by_walk: the state is (src, mask, dst), the loop runs while mask != 0.
def walk_start(o, src, mask):
    return (src, mask, o.word(0))


def walk_going(o, state):
    _, mask, _ = state
    return mask != 0


def walk_step(o, state, path):
    src, mask, dst = state
    lowest = mask & ~(mask - 1)
    return (o.lshr(src, 1), mask & (mask - 1), dst | (lowest & (0 - (src & 1))))


def block_expand_by_walk(o, src, mask):
    _, _, dst = run(o, walk_start(o, src, mask), walk_going, walk_step)
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
SELECT_INDEX_BITS = 11


# block_select_by_broadword, in two parts that meet at its counts: the ones of each byte and every byte below it.
def select_counts(o, x):
    byte_mask = BYTE_VALUES - 1
    ones_per_byte = ones(o) // byte_mask
    pair_lows = ones(o) // 0x03
    nibble_lows = ones(o) // 0x05
    byte_lows = ones(o) // 0x11
    counts = x - (o.lshr(x, 1) & pair_lows)
    counts = (counts & nibble_lows) + (o.lshr(counts, 2) & nibble_lows)
    return ((counts + o.lshr(counts, 4)) & byte_lows) * ones_per_byte


def select_from_counts(o, x, k, counts, path, table=SELECT_IN_BYTE):
    """The rest of the kernel; besides the position, the byte it found and the table's answer within it."""
    byte_mask = BYTE_VALUES - 1
    ones_per_byte = ones(o) // byte_mask
    byte_tops = (ones_per_byte << (BYTE_DIGITS - 1)) % (1 << o.width)
    bytes_below = o.popcount((((o.word(k) * ones_per_byte) | byte_tops) - counts) & byte_tops)
    offset = o.word(BYTE_DIGITS) * bytes_below
    o.require(path, o.ult(offset, o.width), "block_select_by_broadword: shifts by offset within the width")
    rank_in_byte = o.word(k) - (o.lshr(o.shl(counts, BYTE_DIGITS), offset) & byte_mask)
    # Below 8 in the word, so it never wrapped, so the C++ unsigned arithmetic gives the same small value.
    o.require(path, o.ult(rank_in_byte, BYTE_DIGITS), "block_select_by_broadword: rank within the byte below 8")
    index = (o.lshr(x, offset) & byte_mask) | o.shl(rank_in_byte, BYTE_DIGITS)
    o.require(path, o.ult(index, len(table)), "block_select_by_broadword: index inside select_in_byte")
    answer = o.table(table, index, SELECT_INDEX_BITS)
    return offset + answer, bytes_below, answer


def block_select_by_broadword(o, x, k, path=None, table=SELECT_IN_BYTE):
    path = o.true if path is None else path
    o.require(path, o.ult(k, o.popcount(x)), "block_select_by_broadword: precondition k < popcount(x)")
    position, _, _ = select_from_counts(o, x, k, select_counts(o, x), path, table)
    return position


# block_expand_by_select: the state is (src, dst), the loop runs while src != 0, src cut to the mask's population first.
def select_expand_start(o, src, mask):
    mask_ones = o.popcount(mask)
    return (o.ite(o.ult(mask_ones, o.width), src & (o.shl(1, mask_ones) - 1), src), o.word(0))


def select_expand_going(o, state):
    src, _ = state
    return src != 0


def select_expand_step(o, mask, state, path, select=block_select_by_broadword):
    src, dst = state
    position = select(o, mask, o.countr_zero(src), path)
    o.require(path, o.ult(position, o.width), "block_expand_by_select: 1UZ << select within the width")
    return (src & (src - 1), dst | o.shl(1, position))


def block_expand_by_select(o, src, mask):
    _, dst = run(o, select_expand_start(o, src, mask), select_expand_going,
                 lambda o, s, p: select_expand_step(o, mask, s, p))
    return dst
