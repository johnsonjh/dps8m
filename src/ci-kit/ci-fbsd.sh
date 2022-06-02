#!/usr/bin/env sh

export SHELL=/bin/sh > /dev/null 2>&1

command -v "gawk" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No GNU awk command available - try: pkg install gawk."
    exit 1
  }

command -v "gsed" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No GNU sed command available - try: pkg install gsed."
    exit 1
  }

command -v "dos2unix" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No dos2unix command available - try: pkg install unix2dos."
    exit 1
  }

command -v "tmux" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No tmux command available - try: pkg install tmux."
    exit 1
  }

command -v "expect" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No expect command available - try: pkg install expect."
    exit 1
  }

command -v "moreutils-ts" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No moreutils-ts command available - try: pkg install moreutils."
    exit 1
  }

command -v "ansifilter" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No ansifilter command available - try: pkg install ansifilter."
    exit 1
  }

command -v "gmake" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No GNU make command available - try: pkg install gmake."
    exit 1
  }

command -v "gtail" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No gtail command available - try: pkg install coreutils."
    exit 1
  }

command -v "gdate" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No gdate command available - try: pkg install coreutils."
    exit 1
  }

command -v "mksh" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No mksh command available - try: pkg install mksh."
    exit 1
  }

command -v "ncat" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No ncat command available - try: pkg install nmap."
    exit 1
  }

command -v "faketime" > /dev/null 2>&1 ||
  {
    printf '%s\n'  \
      "*** Error: No faketime command available - try: pkg install libfaketime."
    exit 1
  }

# NOTE: faketime disabled until FreeBSD package fixed.
${SHELL:?} -c 'export SHELL=/bin/sh ; FAKETIME="env TZ=UTC" TAIL=gtail MAKE=gmake DATE=gdate mksh ./ci.sh'
