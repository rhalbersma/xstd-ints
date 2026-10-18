#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""MkDocs hooks that let the Markdown read the same on GitHub and on the site.

The site serves README.md as its index and the files under docs_dir as its other pages. A relative link is written
for GitHub, against the file it sits in, so each one is resolved against the repository and pointed at the page the
site serves for that file, or at the file on GitHub where the site serves none, such as CONTRIBUTING.md.
"""

import pathlib
import posixpath
import re

from mkdocs.structure.files import File

ROOT = pathlib.Path(__file__).resolve().parent.parent
BLOB = "https://github.com/rhalbersma/xstd-ints/blob/main/"
LINK = re.compile(r"\]\((?![a-z]+:)([^)#\s]+)(#[^)\s]*)?\)")


def on_files(files, config):
    files.append(File.generated(config, "index.md", abs_src_path=str(ROOT / "README.md")))
    return files


def on_page_markdown(markdown, page, config, files):
    docs = pathlib.Path(config["docs_dir"]).resolve().relative_to(ROOT).as_posix()

    def in_repository(uri):
        return "README.md" if uri == "index.md" else f"{docs}/{uri}"

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
