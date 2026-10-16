#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""Run mull-runner over each test executable and score the mutants across all of them.

A header's mutant is compiled into every executable that instantiates the code it sits in, and each executable runs
only its own tests against it. mull-runner scores one executable at a time, so a mutant one test file leaves alive can
show as a survivor there although another file's tests kill it. A mutant here survives only if it survives in every
executable that ran it.
"""

import argparse
import os
import sqlite3
import subprocess
import sys
from pathlib import Path

# mull-runner's ExecutionStatus, as its SQLite reporter stores it.
PASSED = 2
NOT_COVERED = 8


def run(runner: str, config: str, executable: Path, report_dir: Path, timeout_ms: int, workers: int) -> Path:
    name = executable.name
    command = [
        runner,
        '--reporters', 'SQLite',
        '--report-dir', str(report_dir),
        '--report-name', name,
        '--timeout', str(timeout_ms),
        '--workers', str(workers),
        '--include-not-covered',
        '--allow-surviving',
        '--no-output',
        str(executable),
    ]
    print(f'mull-runner {name}', flush=True)
    result = subprocess.run(command, env=os.environ | {'MULL_CONFIG': config}, capture_output=True, text=True)
    if result.returncode != 0:
        sys.exit(f'mull-runner failed on {name} with exit status {result.returncode}:\n{result.stdout}{result.stderr}')
    return report_dir / f'{name}.sqlite'


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--runner', required=True, help='the mull-runner of the LLVM the executables were built with')
    parser.add_argument('--config', required=True, help='the mull.yml the executables were built with')
    parser.add_argument('--report-dir', required=True, type=Path, help='where the per-executable SQLite reports go')
    parser.add_argument('--timeout', type=int, default=60_000, help='milliseconds a mutant run may take')
    parser.add_argument('--workers', type=int, default=os.cpu_count() or 1, help='mutants run in parallel')
    parser.add_argument('executables', nargs='+', type=Path)
    args = parser.parse_args()

    args.report_dir.mkdir(parents=True, exist_ok=True)
    for stale in args.report_dir.glob('*.sqlite'):
        stale.unlink()

    statuses: dict[tuple[str, int, int, str], set[int]] = {}
    for executable in args.executables:
        report = run(args.runner, args.config, executable, args.report_dir, args.timeout, args.workers)
        # An executable that compiled in none of the mutated code writes no report.
        if not report.exists():
            continue
        with sqlite3.connect(report) as db:
            rows = db.execute('SELECT filename, line_number, column_number, mutator, execution_status FROM mutant')
            for filename, line, column, mutator, status in rows:
                statuses.setdefault((filename, line, column, mutator), set()).add(status)

    ran = {key: seen - {NOT_COVERED} for key, seen in statuses.items()}
    uncovered = sorted(key for key, seen in ran.items() if not seen)
    survivors = sorted(key for key, seen in ran.items() if seen == {PASSED})
    killed = len(ran) - len(uncovered) - len(survivors)

    for filename, line, column, mutator in survivors:
        print(f'{filename}:{line}:{column}: survived: {mutator}')
    for filename, line, column, mutator in uncovered:
        print(f'{filename}:{line}:{column}: not covered: {mutator}')

    total = len(ran)
    score = 100 * killed // total if total else 100
    summary = f'Mutation score: {score}% ({killed} of {total} mutants killed, {len(survivors)} survived, {len(uncovered)} not covered)'
    print(summary)

    if step_summary := os.environ.get('GITHUB_STEP_SUMMARY'):
        with open(step_summary, 'a', encoding='utf-8') as out:
            out.write(f'### {summary}\n')
            for filename, line, column, mutator in survivors + uncovered:
                out.write(f'- `{filename}:{line}:{column}` {mutator}\n')

    return 1 if survivors or uncovered or not total else 0


if __name__ == '__main__':
    sys.exit(main())
