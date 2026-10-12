#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Pinned by digest, as the workflows pin their actions: the builder's clang and libc++ are what the targets compile with.
FROM gcr.io/oss-fuzz-base/base-builder@sha256:e67da2b0f6156b29b62734305a8c74db61aaf3158c7ab96386a07974fb33cdc8

# The builder's CMake is 3.29, and CMakeLists.txt requires 3.30; the wheel tools/cmake-requirements.txt pins by hash.
COPY tools/cmake-requirements.txt /tmp/cmake-requirements.txt
RUN unset CFLAGS CXXFLAGS && pip3 install --no-cache-dir --require-hashes -r /tmp/cmake-requirements.txt && rm /tmp/cmake-requirements.txt

COPY . $SRC/xstd-ints
WORKDIR $SRC/xstd-ints
COPY .clusterfuzzlite/build.sh $SRC/
