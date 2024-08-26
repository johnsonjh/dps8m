#!/usr/bin/env bash
# scspell-id: f585612c-63e8-11ef-9fc2-80ee73e9b8e7
# shellcheck disable=SC2312

set -eu

# Compiler
test -z "${CC:-}" && CC="clang"
export CC

# Test
"${CC:?}" --version

# Setup
RUNS=5
printf '\n%s\n' "Setting up PGO build ..."
PROFILE_PATH="$(pwd -P)/.profile_data"
export PROFILE_PATH
test -d "${PROFILE_PATH}" && rm -rf "${PROFILE_PATH}"
mkdir -p "${PROFILE_PATH}"
export BASE_LDFLAGS="${LDFLAGS:-}"
export BASE_CFLAGS="-Dftello64=ftello -Doff64_t=off_t -Dfseeko64=fseeko -Dfopen64=fopen ${CFLAGS:-}"
export LIBUVVER="libuvrel"
export STDERR="/dev/stderr"
export LLVM_PROFILE_FILE="${PROFILE_PATH:?}/profile.%p.profraw"

# Base
printf '\n%s\n' "Generating baseline build ..."
export CFLAGS="${BASE_CFLAGS:?}"
export LDFLAGS="${BASE_LDFLAGS:-} ${CFLAGS:?} -fuse-ld=lld"
${MAKE:-make} distclean
${MAKE:-make} "${LIBUVVER:?}"
${MAKE:-make}
printf '\n%s\n' "Running baseline benchmark ..."
SMIPS=$(cd src/perf_test && for i in $(seq 1 "${RUNS}"); do printf '%s' "(${i:?}/${RUNS:?}) " >&2; ../dps8/dps8 -r ./nqueensx.ini | grep MIPS | tee "${STDERR}"; done | tr -cd '[\n.0-9]' | awk '{for (i=1;i<=NF;++i) {sum+=$i; ++n}} END {printf "%.4f\n", sum/n}')

# Profile
printf '\n%s\n' "Generating profile build ..."
export CFLAGS="-fprofile-generate=${PROFILE_PATH:?}/profile.%p.profraw ${BASE_CFLAGS:?}"
export LDFLAGS="${BASE_LDFLAGS:-} ${CFLAGS:?} -fuse-ld=lld"
${MAKE:-make} distclean
${MAKE:-make} "${LIBUVVER:?}"
${MAKE:-make}
printf '\n%s\n' "Generating profile ..."
(cd src/perf_test && ../dps8/dps8 -r ./nqueensx.ini)
llvm-profdata merge -output="${PROFILE_PATH:?}/final.profdata" "${PROFILE_PATH:?}"/profile.*.profraw

# Build
printf '\n%s\n' "Generating final build ..."
export CFLAGS="-fprofile-use=${PROFILE_PATH:?}/final.profdata ${BASE_CFLAGS:?}"
export LDFLAGS="${BASE_LDFLAGS:-} ${CFLAGS:?} -fuse-ld=lld"
${MAKE:-make} distclean
${MAKE:-make} "${LIBUVVER:?}"
${MAKE:-make}

# Final
printf '\n%s\n' "Running final benchmark ..."
EMIPS=$(cd src/perf_test && for i in $(seq 1 "${RUNS}"); do printf '%s' "(${i:?}/${RUNS:?}) " >&2; ../dps8/dps8 -r ./nqueensx.ini | grep MIPS | tee "${STDERR}"; done | tr -cd '[\n.0-9]' | awk '{for (i=1;i<=NF;++i) {sum+=$i; ++n}} END {printf "%.4f\n", sum/n}')
printf '\nBefore : %s\nAfter  : %s\n' "${SMIPS:?}" "${EMIPS:?}"
# shellcheck disable=SC2046
printf 'Change : %s%%\n' $(printf '%s\n' "scale=6;((${EMIPS:?}-${SMIPS:?})/${EMIPS:?})*100" | bc -l | dd bs=1 count=6 2> /dev/null)
