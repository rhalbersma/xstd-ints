#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""The word operations the kernels and their specifications are written over, in two interpretations.

Z3Ops builds Z3 bit-vector terms, for the proofs; IntOps evaluates on Python integers reduced to the word width, as
the C++ does, for the drift check against the compiled kernels. An obligation is a condition the C++ needs on the path
that reaches it: Z3Ops records it as an implication for a proof to discharge, IntOps raises where it fails.
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
