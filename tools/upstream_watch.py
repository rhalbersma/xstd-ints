#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""Reports what has changed upstream since xstd-ints last reviewed it, and opens or updates one issue saying so.

Four kinds of change are watched, each against a reviewed state recorded in this repository:

- Papers: a revision on wg21.link newer than the one CONTRIBUTING.md's tracking table records as reviewed, or a
  change of state, labels or milestone on the paper's cplusplus/papers tracking issue since upstream-watch.json.
- The working draft: a change to a watched clause in cplusplus/draft since the commit upstream-watch.json records.
- Standard libraries: a value of __cpp_lib_bitops in libstdc++ or libc++ trunk other than the one last reviewed,
  with the forwarding overloads it makes live, read from xstd's own headers, and the tests that then exercise them.
- LWG: an issue against a watched clause that is new since the recorded snapshot, or whose status has changed.

    upstream_watch.py                print the report
    upstream_watch.py --issue        and open or update the issue in $GITHUB_REPOSITORY when something changed
    upstream_watch.py --record       after a review: write upstream's current state into upstream-watch.json

The tracking table stays a human record of reviews: --record never touches it, so a new paper revision is reported
until its row is updated. Reading tracking issues and writing the issue need $GITHUB_TOKEN.
"""

import argparse
import html
import json
import os
import pathlib
import re
import subprocess
import sys
import tempfile
import urllib.error
import urllib.request

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parent
STATE = HERE / "upstream-watch.json"
TITLE = "Upstream changes to review"
MARKER = "<!-- upstream-watch -->"

DRAFT = "https://github.com/cplusplus/draft"
LIBSTDCXX = ("gcc-mirror/gcc", "master", "libstdc++-v3/include/bits/version.def")
LIBCXX = ("llvm/llvm-project", "main", "libcxx/include/version")
LWG_TOC = "https://cplusplus.github.io/LWG/lwg-toc.html"


def fetch(url, token=None, data=None, method=None):
    request = urllib.request.Request(url, data=data, method=method, headers={"User-Agent": "xstd-ints-upstream-watch"})
    if token:
        request.add_header("Authorization", f"Bearer {token}")
        request.add_header("Accept", "application/vnd.github+json")
    with urllib.request.urlopen(request, timeout=60) as response:
        return response.read().decode(), response.geturl()


def git(*args, cwd=None):
    return subprocess.run(["git", *args], cwd=cwd, check=True, capture_output=True, text=True).stdout


def head_commit(repository, branch):
    return git("ls-remote", f"https://github.com/{repository}", f"refs/heads/{branch}").split()[0]


# Papers.

def reviewed_papers(contributing):
    """Each row of the tracking table: paper, reviewed revision and tracking issue number."""
    rows = {}
    for line in contributing.splitlines():
        row = re.match(r"\|\s*\[?(P\d{4})R(\d+)\]?(?:\([^)]*\))?\s*\|.*cplusplus/papers#(\d+)", line)
        if row:
            rows[row.group(1)] = {"revision": int(row.group(2)), "issue": int(row.group(3))}
    return rows


def latest_revision(paper):
    """The revision wg21.link's unversioned link resolves to."""
    _, url = fetch(f"https://wg21.link/{paper}")
    found = re.search(rf"{paper.lower()}r(\d+)", url.lower())
    return int(found.group(1)) if found else None, url


def tracking(issue, token):
    body, _ = fetch(f"https://api.github.com/repos/cplusplus/papers/issues/{issue}", token)
    data = json.loads(body)
    return {"state": data["state"], "labels": sorted(label["name"] for label in data["labels"]),
            "milestone": data["milestone"]["title"] if data.get("milestone") else None}


# The working draft.

def clause(source, label):
    """A clause's LaTeX from its \\rSec heading to the next heading at its level or above, or None."""
    heading = re.search(rf"\\rSec(\d)\[{re.escape(label)}\]", source)
    if not heading:
        return None
    level = int(heading.group(1))
    following = re.compile(rf"\\rSec[0-{level}]\[")
    end = following.search(source, heading.end())
    return source[heading.start():end.start() if end else len(source)]


def draft_changes(reviewed, clauses):
    """For each watched clause, its diff from the reviewed commit to the head of cplusplus/draft."""
    with tempfile.TemporaryDirectory() as scratch:
        git("clone", "--quiet", "--filter=blob:none", "--no-checkout", DRAFT, scratch)
        head = git("rev-parse", "HEAD", cwd=scratch).strip()
        changes = {}
        for label in clauses:
            texts = []
            for commit in (reviewed, head):
                found = git("grep", "-l", f"\\\\rSec[0-9]\\[{label}\\]", commit, "--", "source", cwd=scratch).split()
                text = None
                if found:
                    path = found[0].split(":", 1)[1]
                    text = clause(git("show", f"{commit}:{path}", cwd=scratch), label)
                texts.append(text)
            if texts[0] != texts[1]:
                changes[label] = diff(texts[0] or "", texts[1] or "", label)
    return head, changes


def diff(before, after, label):
    with tempfile.TemporaryDirectory() as scratch:
        old, new = pathlib.Path(scratch) / "reviewed", pathlib.Path(scratch) / "head"
        old.write_text(before)
        new.write_text(after)
        result = subprocess.run(["git", "diff", "--no-index", "--unified=2", "--no-color", str(old), str(new)],
                                capture_output=True, text=True)
    lines = result.stdout.splitlines()[4:]
    return "\n".join(lines) if lines else f"[{label}] appeared or disappeared"


# Standard libraries.

def libstdcxx_value(text):
    """The newest value version.def gives __cpp_lib_bitops, with the language mode it needs."""
    block = re.search(r"name = bitops;(.*?)\n};", text, re.S)
    values = re.findall(r"v = (\d+);\s*cxxmin = (\d+);", block.group(1))
    value, mode = max((int(v), int(m)) for v, m in values)
    return value, f"C++{mode}"


def libcxx_value(text):
    """The newest value <version> gives __cpp_lib_bitops, with the language mode it needs."""
    best = (0, "")
    mode = ""
    for line in text.splitlines():
        guard = re.match(r"#if _LIBCPP_STD_VER >= (\d+)", line)
        if guard:
            mode = f"C++{guard.group(1)}"
        define = re.match(r"#\s*define __cpp_lib_bitops\s+(\d+)L", line)
        if define:
            best = max(best, (int(define.group(1)), mode))
    return best


def forwarding_overloads():
    """Each __cpp_lib_bitops threshold in xstd's detail headers, the macro it sets, and the public headers using it."""
    overloads = []
    for header in sorted((ROOT / "include/xstd/ints/bit/detail").glob("std_*.hpp")):
        text = header.read_text()
        threshold = re.search(r"__cpp_lib_bitops >= (\d+)L", text)
        # The first macro is the threshold's own; any after it narrow it, and their functions still go live with it.
        macros = re.findall(r"#define (XSTD_\w+) 1", text)
        if not (threshold and macros):
            continue
        functions = sorted(path.stem for path in (ROOT / "include/xstd/ints/bit").glob("*.hpp")
                           if any(f"#ifdef {macro}" in path.read_text() for macro in macros))
        tests = [f"test/src/ints/bit/{name}.cpp" for name in functions
                 if (ROOT / "test/src/ints/bit" / f"{name}.cpp").exists()]
        overloads.append({"threshold": int(threshold.group(1)), "macro": macros[0], "functions": functions,
                          "tests": tests})
    return overloads


def libraries():
    current = {}
    for name, (repository, branch, path), parse in (("libstdc++", LIBSTDCXX, libstdcxx_value),
                                                    ("libc++", LIBCXX, libcxx_value)):
        commit = head_commit(repository, branch)
        text, _ = fetch(f"https://raw.githubusercontent.com/{repository}/{commit}/{path}")
        value, mode = parse(text)
        current[name] = {"bitops": value, "mode": mode, "commit": commit, "repository": repository, "path": path}
    return current


# LWG.

def lwg_issues(text, clauses):
    """Each LWG issue whose section is a watched clause or lies within one, with its status and title."""
    issues = {}
    for row in re.findall(r"<tr>(.*?)</tr>", text, re.S):
        cells = re.findall(r"<td[^>]*>(.*?)</td>", row, re.S)
        if len(cells) < 4:
            continue
        number = re.sub(r"<[^>]+>|\(i\)", "", cells[0]).strip()
        section = re.search(r"\[([\w.]+)\]", cells[2])
        if not (number.isdigit() and section):
            continue
        label = section.group(1)
        if any(label == watched or label.startswith(watched + ".") for watched in clauses):
            issues[number] = {"status": html.unescape(re.sub(r"<[^>]+>", "", cells[1])).strip(), "section": label,
                              "title": html.unescape(re.sub(r"<[^>]+>", "", cells[3])).strip()}
    return issues


# The report.

def report(state, contributing, token):
    """The Markdown body naming every change, or None when nothing has changed; and upstream's current state."""
    sections = []
    current = {"tracking": {}}

    rows = reviewed_papers(contributing)
    if not rows:
        raise SystemExit("CONTRIBUTING.md's tracking table has no row this script can read; fix the table or the parser")
    papers = []
    for paper, row in rows.items():
        revision, url = latest_revision(paper)
        if revision is not None and revision > row["revision"]:
            papers.append(f"- **{paper}R{revision}** is out ({url}); the tracking table records "
                          f"{paper}R{row['revision']} as reviewed.")
        if token:
            snapshot = tracking(row["issue"], token)
            current["tracking"][paper] = snapshot
            recorded = state.get("tracking", {}).get(paper)
            if recorded != snapshot:
                was = "nothing recorded" if recorded is None else describe(recorded)
                papers.append(f"- [cplusplus/papers#{row['issue']}](https://github.com/cplusplus/papers/issues/"
                              f"{row['issue']}) ({paper}) is now {describe(snapshot)}; recorded: {was}.")
        else:
            current["tracking"] = state.get("tracking", {})
    if papers:
        sections.append("## Papers\n\n" + "\n".join(papers))

    head, changes = draft_changes(state["draft"]["commit"], state["draft"]["clauses"])
    current["draft"] = {"commit": head, "clauses": state["draft"]["clauses"]}
    if changes:
        reviewed = state["draft"]["commit"]
        text = [f"cplusplus/draft has moved from the reviewed {reviewed[:12]} to {head[:12]} "
                f"([compare]({DRAFT}/compare/{reviewed}...{head})), changing:"]
        for label, change in changes.items():
            text.append(f"\n<details><summary><code>[{label}]</code></summary>\n\n```diff\n{change}\n```\n\n</details>")
        sections.append("## Working draft\n\n" + "\n".join(text))

    found = libraries()
    current["libraries"] = {name: {"bitops": value["bitops"]} for name, value in found.items()}
    overloads = forwarding_overloads()
    libs = []
    for name, value in found.items():
        recorded = state["libraries"].get(name, {}).get("bitops")
        if value["bitops"] == recorded:
            continue
        source = f"https://github.com/{value['repository']}/blob/{value['commit']}/{value['path']}"
        libs.append(f"- **{name}** trunk defines `__cpp_lib_bitops` as `{value['bitops']}L` in {value['mode']} mode "
                    f"([source]({source})); reviewed: `{recorded}L`.")
        for overload in overloads:
            if value["bitops"] >= overload["threshold"] > (recorded or 0):
                functions = ", ".join(f"`xstd::{function}`" for function in overload["functions"])
                tests = ", ".join(f"`{test}`" for test in overload["tests"])
                libs.append(f"  - `{overload['macro']}` (at `{overload['threshold']}L`) goes live: {functions} "
                            f"forward to `std::` there, exercised by {tests} on a leg that builds against {name} "
                            f"trunk in {value['mode']} mode.")
    if libs:
        sections.append("## Standard libraries\n\n" + "\n".join(libs))

    toc, _ = fetch(LWG_TOC)
    issues = lwg_issues(toc, state["draft"]["clauses"])
    current["lwg"] = {number: issue["status"] for number, issue in issues.items()}
    lwg = []
    for number, issue in sorted(issues.items(), key=lambda item: int(item[0])):
        recorded = state["lwg"].get(number)
        if recorded == issue["status"]:
            continue
        change = "new" if recorded is None else f"was {recorded}"
        lwg.append(f"- [LWG {number}](https://cplusplus.github.io/LWG/issue{number}) [{issue['section']}] "
                   f"{issue['title']}: {issue['status']} ({change}).")
    if lwg:
        sections.append("## LWG\n\n" + "\n".join(lwg))

    if not sections:
        return None, current
    footer = ("After a review, update the paper's row in CONTRIBUTING.md's tracking table, run "
              "`tools/upstream_watch.py --record` for the rest, and close this issue.")
    return "\n\n".join([MARKER, *sections, footer]), current


def describe(snapshot):
    labels = ", ".join(snapshot["labels"]) or "no labels"
    milestone = f", milestone {snapshot['milestone']}" if snapshot["milestone"] else ""
    return f"{snapshot['state']} ({labels}{milestone})"


def publish(body, repository, token):
    """Opens the issue, or updates the open one and comments that it changed; leaves it alone when nothing did."""
    api = f"https://api.github.com/repos/{repository}/issues"
    listing, _ = fetch(f"{api}?state=open&per_page=100", token)
    existing = [issue for issue in json.loads(listing)
                if issue["title"] == TITLE and MARKER in (issue["body"] or "") and "pull_request" not in issue]
    if not existing:
        created, _ = fetch(api, token, json.dumps({"title": TITLE, "body": body}).encode(), "POST")
        print(f"Opened {json.loads(created)['html_url']}")
    elif existing[0]["body"] != body:
        number = existing[0]["number"]
        fetch(f"{api}/{number}", token, json.dumps({"body": body}).encode(), "PATCH")
        fetch(f"{api}/{number}/comments", token, json.dumps({"body": "Upstream changed again; the description above "
                                                                     "is current."}).encode(), "POST")
        print(f"Updated {existing[0]['html_url']}")
    else:
        print(f"{existing[0]['html_url']} already says this")


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    action = parser.add_mutually_exclusive_group()
    action.add_argument("--issue", action="store_true", help="open or update the issue in $GITHUB_REPOSITORY")
    action.add_argument("--record", action="store_true", help="write upstream's current state into the state file")
    args = parser.parse_args()

    token = os.environ.get("GITHUB_TOKEN")
    state = json.loads(STATE.read_text())
    body, current = report(state, (ROOT / "CONTRIBUTING.md").read_text(), token)

    if args.record:
        if not token:
            print("warning: no GITHUB_TOKEN, so the tracking issues keep their recorded state", file=sys.stderr)
        STATE.write_text(json.dumps(current, indent=2, sort_keys=True) + "\n")
        print(f"Recorded upstream's current state in {STATE.relative_to(ROOT)}")
        return
    print(body or "Nothing upstream has changed since the last review.")
    if args.issue and body:
        try:
            publish(body, os.environ["GITHUB_REPOSITORY"], token)
        except urllib.error.HTTPError as error:
            sys.exit(f"could not open or update the issue: {error}")


if __name__ == "__main__":
    main()
