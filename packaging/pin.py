#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""Pin the vcpkg port and the Conan recipe to one GitHub archive, the way each registry will.

Both registries fetch a release by its GitHub archive and check it against a hash. Given a ref, a tag or a commit
already on GitHub, this downloads that archive, hashes it, and writes a copy of each recipe pinned to it: an overlay
port whose REF and SHA512 name it, and a conandata.yml whose url and sha256 do. With --version it also writes the
version, so a pull request can test the recipes against its own head before any tag exists. Given packaging/ itself
as the output, it pins the recipes in place, which is how a release tag gets its hashes.
"""

import argparse
import hashlib
import pathlib
import re
import shutil
import sys
import urllib.request

HERE = pathlib.Path(__file__).resolve().parent
REPO = "rhalbersma/xstd-ints"


def archive(ref):
    url = f"https://github.com/{REPO}/archive/{ref}.tar.gz"
    with urllib.request.urlopen(url, timeout=120) as response:
        return url, response.read()


def pin_vcpkg(out, ref, data, version):
    port = out / "vcpkg" / "xstd-ints"
    if port.resolve() != (HERE / "vcpkg" / "xstd-ints"):
        shutil.copytree(HERE / "vcpkg" / "xstd-ints", port, dirs_exist_ok=True)
    portfile = port / "portfile.cmake"
    text = portfile.read_text()
    # A tag the portfile spells from the version stays as it is; anything else replaces it.
    if ref != f"v{version}":
        text = re.sub(r'REF "[^"]*"', f'REF "{ref}"', text)
    text = re.sub(r"SHA512 \S+", f"SHA512 {hashlib.sha512(data).hexdigest()}", text)
    portfile.write_text(text)
    manifest = port / "vcpkg.json"
    manifest.write_text(re.sub(r'"version-semver": "[^"]*"', f'"version-semver": "{version}"', manifest.read_text()))
    return port.parent


def pin_conan(out, url, data, version):
    recipe = out / "conan"
    if recipe.resolve() != HERE / "conan":
        shutil.copytree(HERE / "conan", recipe, dirs_exist_ok=True)
    sha256 = hashlib.sha256(data).hexdigest()
    (recipe / "conandata.yml").write_text(f'sources:\n  "{version}":\n    url: "{url}"\n    sha256: "{sha256}"\n')
    return recipe


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("ref", help="a tag or commit on GitHub, such as v0.1.0 or a full SHA")
    parser.add_argument("out", type=pathlib.Path, help="where to write the pinned copies")
    parser.add_argument("--version", default="0.1.0", help="the version both recipes report")
    args = parser.parse_args()
    url, data = archive(args.ref)
    ports = pin_vcpkg(args.out, args.ref, data, args.version)
    recipe = pin_conan(args.out, url, data, args.version)
    print(f"{args.ref}: {len(data)} bytes; overlay ports in {ports}, Conan recipe in {recipe}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
