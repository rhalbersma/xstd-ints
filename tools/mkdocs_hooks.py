#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""MkDocs hooks that let the Markdown read the same on GitHub and on the site, and keep its citations linked.

The site serves README.md as its index and the files under docs_dir as its other pages. A relative link is written
for GitHub, against the file it sits in, so each one is resolved against the repository and pointed at the page the
site serves for that file, or at the file on GitHub where the site serves none, such as CONTRIBUTING.md.

A citation of the working draft, such as [iterator.concept.winc]/3, links to its section at eel.is/c++draft, and a WG21
paper or document, such as P3104R6 or N4950, links to its cplusplus/papers issue or to wg21.link. One left unlinked in
prose is reported as a warning, which --strict makes a failure.

Python-Markdown nests a list item, and a paragraph, table or fence that continues one after a blank line, only when it
is indented four spaces past its parent's marker, where GitHub nests it at two. Either indented one to three spaces is
reported as a warning too: on the site it would render outside the list it belongs to.

Each header's page under reference/ shows an example, pulled in from examples/ by pymdownx.snippets with what it prints,
and every example there is shown on a page. A page without an Example section, or an example no page shows, is
reported as a warning too. An example is shown from its first line of code: the licence notice that opens its source
is the repository's, as the site's footer states.
"""

import logging
import pathlib
import posixpath
import re

from mkdocs.structure.files import File

ROOT = pathlib.Path(__file__).resolve().parent.parent
BLOB = "https://github.com/rhalbersma/xstd-ints/blob/main/"
LINK = re.compile(r"\]\((?![a-z]+:)([^)#\s]+)(#[^)\s]*)?\)")

# What a citation check reads past: fenced and inline code, Markdown links with their text, and HTML tags.
NOT_PROSE = re.compile(r"^```.*?^```|`[^`\n]*`|\[(?:[^\[\]]|\[[^\[\]]*\])*\]\([^)]*\)|<[^>]+>", re.M | re.S)
CITATION = re.compile(r"\[[a-z][a-z0-9_]*(?:\.[a-z0-9_]+)+\](?:/\d+)?|\b[PN]\d{4}(?:R\d+)?\b", re.I)

# A list item, or a block after a blank line, that Python-Markdown would not nest under its parent.
UNNESTED_ITEM = re.compile(r"^ {1,3}(?:[-*+]|\d+[.)]) ")
UNNESTED_BLOCK = re.compile(r"^ {1,3}\S")
FENCE = re.compile(r"^ *(?:```|~~~)")

# A snippet that pymdownx.snippets replaces with the file it names, here an example or the output it prints.
SNIPPET = re.compile(r'^--8<-- "([^"]+)"$', re.M)

log = logging.getLogger("mkdocs.hooks.citations")
examples_log = logging.getLogger("mkdocs.hooks.examples")
shown = set()


def on_files(files, config):
    files.append(File.generated(config, "index.md", abs_src_path=str(ROOT / "README.md")))
    return files


def from_first_code_line(snippet):
    path = snippet.group(1)
    if not path.endswith(".cpp"):
        return snippet.group(0)
    lines = (ROOT / path).read_text(encoding="utf-8").splitlines()
    first = next(number for number, line in enumerate(lines, 1) if line.strip() and not line.startswith("//"))
    return f'--8<-- "{path}:{first}:"'


def on_page_markdown(markdown, page, config, files):
    docs = pathlib.Path(config["docs_dir"]).resolve().relative_to(ROOT).as_posix()

    shown.update(SNIPPET.findall(markdown))
    markdown = SNIPPET.sub(from_first_code_line, markdown)
    uri = pathlib.PurePosixPath(page.file.src_uri)
    if uri.parts[0] == "reference" and len(uri.parts) == 3 and uri.name != "index.md" and "### Example" not in markdown:
        examples_log.warning("%s/%s has no Example section", docs, uri)

    def in_repository(uri):
        return "README.md" if uri == "index.md" else f"{docs}/{uri}"

    for citation in CITATION.findall(NOT_PROSE.sub(" ", markdown)):
        log.warning("%s cites %s without a link", in_repository(page.file.src_uri), citation)

    fenced, after_blank = False, False
    for number, line in enumerate(markdown.splitlines(), 1):
        if FENCE.match(line):
            fenced = not fenced
        elif not fenced and (UNNESTED_ITEM.match(line) or (after_blank and UNNESTED_BLOCK.match(line))):
            log.warning("%s:%d indents a list's block by fewer than four spaces", in_repository(page.file.src_uri), number)
        after_blank = not line.strip()

    served = {in_repository(f.src_uri): f.src_uri for f in files.documentation_pages()}
    here = posixpath.dirname(in_repository(page.file.src_uri))

    # A link to no file at all is left as written, for --strict to report.
    def relink(link):
        target = posixpath.normpath(posixpath.join(here, link.group(1)))
        if target in served:
            target = posixpath.relpath(served[target], posixpath.dirname(page.file.src_uri) or ".")
        elif (ROOT / target).exists():
            target = BLOB + target
        else:
            return link.group(0)
        return f"]({target}{link.group(2) or ''})"

    return LINK.sub(relink, markdown)


def on_env(env, config, files):
    for example in sorted((ROOT / "examples").rglob("*")):
        relative = example.relative_to(ROOT).as_posix()
        if example.suffix in (".cpp", ".out") and not relative.startswith("examples/perft/") and relative not in shown:
            examples_log.warning("no reference page shows %s", relative)
    return env
