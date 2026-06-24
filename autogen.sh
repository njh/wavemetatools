#!/bin/sh
# Run this to set up the build system: configure, makefiles, etc.

package="Wave Meta Tools"

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

cd "$srcdir"

(autoreconf --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have autoreconf (autoconf/automake toolchain) installed to compile $package."
    echo "Download the appropriate package for your distribution,"
    echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
    exit 1
}

echo "Generating configuration files for $package, please wait...."
if [ ! -d "$srcdir/build-scripts" ]; then
  mkdir "$srcdir/build-scripts"
fi

echo "  running autoreconf -fvi ..."
if ! autoreconf -fvi; then
  echo failed!
  exit 1
fi

$srcdir/configure && echo
