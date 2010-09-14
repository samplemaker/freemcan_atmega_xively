#!/bin/sh

oldcwd="$(pwd)"
topdir="$(cd "$(dirname "$0")" && pwd)"
gitdir="${topdir}/.git"

version_h="$1"
test "x$version_h" = "x" && exit

cd "$topdir"

git_version="unknown-version"

if test -d "${gitdir}" && test "x$(git rev-parse HEAD)" != "x"; then
    if git_version="$(git describe)"; then
	if git diff-files --quiet && git diff-index --cached --quiet HEAD; then
	    :
	else
	    git_version="${git_version}-dirty"
	fi
    fi
fi

cd "$oldcwd"

cat>"${version_h}.tmp"<<EOF
/* This file was automatically generated by "$0" */
#ifndef GIT_VERSION_H
#define GIT_VERSION_H
#define GIT_VERSION "${git_version}"
#endif /* GIT_VERSION_H */
EOF

if test -f "${version_h}" && cmp "${version_h}.tmp" "${version_h}"; then
    rm -f "${version_h}.tmp"
else
    mv -f "${version_h}.tmp" "${version_h}"
fi
