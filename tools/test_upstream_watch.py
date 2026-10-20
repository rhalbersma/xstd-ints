#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""Checks upstream_watch.py's parsers on upstream's own formats, and its report on a seeded change, offline.

    python3 -m unittest tools/test_upstream_watch.py
"""

import pathlib
import sys
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import upstream_watch  # noqa: E402

VERSION_DEF = """
ftms = {
  name = bitops;
  // 202606 P3793R2 Better shifting
  // 202607 P3104R6 Bit permutations
  values = {
    v = 202607;
    cxxmin = 29;
  };
  // 201907 P0553R4 Bit operations
  values = {
    v = 201907;
    cxxmin = 20;
  };
};
"""

VERSION = """
#if _LIBCPP_STD_VER >= 20
# define __cpp_lib_bitops                               201907L
#endif

#if _LIBCPP_STD_VER >= 29
# undef  __cpp_lib_bitops
# define __cpp_lib_bitops                               202606L
#endif
"""

TOC = """
<tr>
<td id="4583"><a href="lwg-active.html#4583" title="t">4583</a><sup><a href="issue4583">(i)</a></sup></td>
<td><a href="lwg-active.html#NAD">NAD</a></td>
<td>22.11.4 [bit.byteswap]</td>
<td>std::byteswap can make sense for some types with padding bytes</td>
</tr>
<tr>
<td id="10"><a href="lwg-closed.html#10" title="t">10</a></td>
<td><a href="lwg-active.html#CD1">CD1</a></td>
<td>22.9.2 [bitset.cons]</td>
<td>Not a bit operation</td>
</tr>
"""

DRAFT = r"""
\rSec1[bit]{Bit manipulation}
\rSec2[bit.general]{General}
The header provides components.
\rSec2[bit.byteswap]{byteswap}
Mandates: no padding bits.
\rSec1[stdbit.h.syn]{Header synopsis}
"""

TABLE = ("| Paper | Reviewed wording | Tracking issue | Last reviewed | Relationship |\n|---|---|---|---|---|\n"
         "| P3104R{revision} | [P3104R6](https://wg21.link/P3104R6) | "
         "[cplusplus/papers#1768](https://github.com/cplusplus/papers/issues/1768) | 2026-10-10 | x |\n")


class Parsers(unittest.TestCase):
    def test_libstdcxx_takes_the_newest_value_with_its_mode(self):
        self.assertEqual(upstream_watch.libstdcxx_value(VERSION_DEF), (202607, "C++29"))

    def test_libcxx_takes_the_newest_value_with_its_mode(self):
        self.assertEqual(upstream_watch.libcxx_value(VERSION), (202606, "C++29"))

    def test_lwg_keeps_watched_clauses_and_their_subclauses_only(self):
        issues = upstream_watch.lwg_issues(TOC, ["bit"])
        self.assertEqual(list(issues), ["4583"])
        self.assertEqual(issues["4583"]["status"], "NAD")
        self.assertEqual(issues["4583"]["section"], "bit.byteswap")

    def test_a_clause_runs_to_the_next_heading_at_its_level(self):
        self.assertIn("no padding bits", upstream_watch.clause(DRAFT, "bit"))
        self.assertNotIn("stdbit", upstream_watch.clause(DRAFT, "bit"))
        self.assertNotIn("General", upstream_watch.clause(DRAFT, "bit.byteswap"))
        self.assertIsNone(upstream_watch.clause(DRAFT, "charconv.to.chars"))

    def test_the_table_gives_paper_revision_and_tracking_issue(self):
        self.assertEqual(upstream_watch.reviewed_papers(TABLE.format(revision=5)),
                         {"P3104": {"revision": 5, "issue": 1768}})

    def test_the_overloads_come_from_the_headers(self):
        overloads = {overload["macro"]: overload for overload in upstream_watch.forwarding_overloads()}
        self.assertEqual(overloads["XSTD_HAS_STD_SHIFTS"]["threshold"], 202606)
        self.assertEqual(overloads["XSTD_HAS_STD_SHIFTS"]["functions"], ["shl", "shr"])
        self.assertEqual(overloads["XSTD_HAS_STD_BIT_PERMUTATIONS"]["threshold"], 202607)
        self.assertEqual(overloads["XSTD_HAS_STD_BIT_PERMUTATIONS"]["functions"],
                         ["bit_compress", "bit_expand", "bit_repeat", "bit_reverse"])


class SeededReport(unittest.TestCase):
    """The network replaced by the upstream state below, and the reviewed state seeded around it."""

    upstream = {"libstdc++": {"bitops": 202607, "mode": "C++29", "commit": "a", "repository": "r", "path": "p"},
                "libc++": {"bitops": 202606, "mode": "C++29", "commit": "b", "repository": "r", "path": "p"}}

    def run_report(self, revision, libraries, lwg):
        state = {"draft": {"commit": "old", "clauses": ["bit"]}, "libraries": libraries, "lwg": lwg, "tracking": {}}
        with mock.patch.object(upstream_watch, "latest_revision", return_value=(6, "https://wg21.link/P3104R6")), \
             mock.patch.object(upstream_watch, "draft_changes", return_value=("old", {})), \
             mock.patch.object(upstream_watch, "libraries", return_value=self.upstream), \
             mock.patch.object(upstream_watch, "fetch", return_value=(TOC, upstream_watch.LWG_TOC)):
            return upstream_watch.report(state, TABLE.format(revision=revision), token=None)[0]

    def test_an_older_reviewed_revision_is_reported(self):
        body = self.run_report(5, {"libstdc++": {"bitops": 202607}, "libc++": {"bitops": 202606}}, {"4583": "NAD"})
        self.assertIn("**P3104R6** is out", body)
        self.assertNotIn("Standard libraries", body)

    def test_a_library_crossing_a_threshold_names_the_overloads_it_makes_live(self):
        body = self.run_report(6, {"libstdc++": {"bitops": 201907}, "libc++": {"bitops": 202606}}, {"4583": "NAD"})
        self.assertIn("`XSTD_HAS_STD_BIT_PERMUTATIONS` (at `202607L`) goes live", body)
        self.assertIn("test/src/ints/bit/shl.cpp", body)
        self.assertNotIn("**libc++**", body)

    def test_a_new_lwg_issue_is_reported(self):
        body = self.run_report(6, {"libstdc++": {"bitops": 202607}, "libc++": {"bitops": 202606}}, {})
        self.assertIn("[LWG 4583]", body)

    def test_nothing_changed_reports_nothing(self):
        body = self.run_report(6, {"libstdc++": {"bitops": 202607}, "libc++": {"bitops": 202606}}, {"4583": "NAD"})
        self.assertIsNone(body)


if __name__ == "__main__":
    unittest.main()
