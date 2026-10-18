---
name: steward
description: Drive an xstd-ints pull request from opened to merged. Covers what to do with a red leg, a merge conflict, a review comment and a green head, and what stays with the maintainer. Use it for any pull request an agent opened here or drives for its author.
---

# Stewarding a pull request

[CLAUDE.md](../../../CLAUDE.md) says when a pull request opens ("Pull requests and CI") and what passes before a
push ("Checking your work"). [CONTRIBUTING.md](../../../CONTRIBUTING.md) lists the checks that gate the merge. This
file covers the rest: what happens between opening a pull request and merging it.

## The pull request itself

- **Title:** a sentence in the imperative saying what changes, as the log on `main` reads. It becomes the squash
  commit's title, with ` (#N)` appended.
- **Body:** `Closes #N.` first, then `## Why`, `## What` and `## Validation`. A bug found along the way gets its own
  heading, with a **Bug:** paragraph and a **Fix:** paragraph.
- **Validation states what ran:**
  - which `tools/ci-local.sh` legs ran, and on which commit;
  - which `main` was merged in;
  - what local CI cannot cover (MSVC, clang-cl, MinGW, Apple Clang, Emscripten, CodeQL), named as CI's alone.
- **Keep the body current.** A fix round that changes what the pull request does updates the body in the same round.
- **Open it as a draft.** It leaves draft only to merge.

## Red CI

Every check that runs on a pull request is fixed before it merges, and none is advisory. That includes the
development rungs (`17-SVN`, `24-SVN`, `2026-Preview`), libc++, clang-cl on VS 2022, ClusterFuzzLite and
Codecov's `patch`. CONTRIBUTING.md's table lists the ones branch protection requires.

1. **Read the leg, not the gate.** `<job> / all` only says that some leg failed. Fetch the failing leg's log and find
   the first error. A compiler crash prints a stack dump that names the declaration it was generating.
2. **Look for the workaround the tree already has.** grep `include/` and `test/` for the toolchain's name, such as
   `Clang 19`, `x86 MSVC`, `Apple Clang` or `libc++`. Every workaround the tree carries says what it works around.
3. **Reproduce before fixing.**
   - **A Linux leg:** run its `ci-local.sh` leg.
   - **A leg `ci-local.sh` cannot run:** reproduce it on Compiler Explorer. Amalgamate the test file and the xstd
     headers it reaches into one source, with `#line` directives so the diagnostics name the real files.
   - **Compiler Explorer's compilers:**
     - `clang1910`: the Clang inside VS 2022's clang-cl;
     - `clang-cl2216` and the `x64 msvc` compilers;
     - `clang_trunk` with `-stdlib=libc++`: what ClusterFuzzLite builds with;
     - `gsnapshot`: GCC trunk.
   - Show the failure, then show the fix passing.
4. **Fix where users would hit it.** A toolchain's bug on a supported rung is still this pull request's failure.
   - When library code trips it, the workaround goes in the library, in the idiom the tree uses for that
     toolchain, since a user instantiating the same code hits the same bug.
   - Guard a test only for a limit of the platform, such as a width the platform lacks. Never guard one to dodge a
     bug in a toolchain the library supports.
5. **Validate, then push once.** The full `tools/ci-local.sh` run passes before every push. Then update the body.

**Never get to green by losing a check.** That rules out:
- skipping, disabling or loosening a test;
- lowering a coverage target;
- a `NOLINT` or `GCOVR_EXCL_LINE` without the reason CONTRIBUTING.md asks for;
- re-running a leg instead of finding its cause.

A re-run is only for a leg that died before any test ran, such as a lost runner or a failed install, and only once.

### Traps the tree already works around

| Symptom | Toolchain | Idiom |
| :-- | :-- | :-- |
| A crash in `EmitAutoVarInit` on a `_BitInt` over 128 bits | Clang 19, which VS 2022's clang-cl bundles | Bind the local by reference: `auto const&` |
| `bit_int<N>` for `N` over 128 does not exist | Apple Clang, whose `__BITINT_MAXWIDTH__` is 128 | Guard on `__BITINT_MAXWIDTH__ >= N` |
| Unresolved `__udivti3` or `__umodti3` | clang-cl, which links no compiler-rt | The test lists stop bit-precise types at 64 bits under `_MSC_VER`. Divisions over 128 bits expand inline and link |
| A 128-bit division writes through a named operand | x86 MSVC's optimizer, with the STL's 128-bit classes | Prvalue operands: `I{a} / I{b}` |
| A structured binding of an over-aligned type reads garbage | x86 MSVC, which passes the binding's bytes rather than its address | Name the members, as `step.quotient`, rather than bind them |
| `_BitInt` counts as `std::integral` | libc++ trunk | Delegate to `std::` only where the width, sign bit included, fits the widest standard type |
| `-Wpadded` on a class deriving from a standard formatter | Clang with libstdc++ | A `#pragma clang diagnostic` push and pop around that one specialization |

## Merge conflicts

- Merge `origin/main` into the branch and resolve the conflict.
- Never rebase, amend or force-push a branch that has a pull request.
- Run the full `ci-local.sh` run again, then push.

## Review comments

Codecov posts statuses, and no bot comments on pull requests here.
- **A reviewer's small, local ask** (a nit, a rename, an added test): implement it in the next round.
- **A larger ask** (an API change, a multi-file refactor, open-ended design): reply with a proposal and leave the
  decision to the maintainer.
- **Comments on GitHub are few:** an answer to a review thread, or one comment on the pull request when a failure is
  not this pull request's. Progress belongs in the body and the diff.

## Green

**Done** means every required check is green on the current head, both Codecov statuses pass, and there is no
merge conflict.

**Merge it at once:**
1. Mark it ready for review.
2. Squash-merge it with the title `<pull request title> (#N)`, and with `expectedHeadSha` set to the head CI passed
   on.

The next queued branch's turn then starts, as CLAUDE.md describes.

## What stays with the maintainer

- **Reports to other projects,** such as a compiler, a standard library, Boost or Abseil:
  - draft each one in an issue labelled `upstream report`, inside a `<details>` block;
  - the maintainer checks for duplicates and files it.
- **Tags and releases,** here and in `cpp-ci`.
- **A finding whose fix costs more than it prevents:** answer its thread with the path you traced, and put the
  decision to the maintainer.
- **Work that pulls in another repository,** and any WG21 paper: these start only on the maintainer's word.
