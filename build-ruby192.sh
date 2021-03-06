#!/bin/sh
set -ue

cd "$(dirname $(readlink -f "$0"))"
cd ruby192
autoconf
./configure \
  --prefix="$(readlink -f ../ruby192-build)" \
  --disable-install-doc \
  --with-ext= \
  --with-static-linked-ext \

echo "option nodynamic" > ext/Setup
# echo "dl" >> ext/Setup
echo "zlib" >> ext/Setup

make
make install
