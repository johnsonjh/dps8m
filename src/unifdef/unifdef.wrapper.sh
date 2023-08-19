#!/usr/bin/env sh
# vim: filetype=sh:tabstop=4:ai:expandtab
# SPDX-License-Identifier: MIT-0
# scspell-id: 9482306e-f631-11ec-a6ed-80ee73e9b8e7
# Copyright (c) 2021-2023 The DPS8M Development Team

############################################################################

IFS=

############################################################################

# shellcheck disable=SC2086
UNIFDEF="$(printf '%s' ${0} | sed 's/unifdef\.wrapper\.sh/unifdef/')"

############################################################################

command -v timeout > /dev/null 2>&1 && HT=1

############################################################################

test -z "${HT:-}" || \
  { timeout --preserve-status 20 "${UNIFDEF:?}" "${@}"; exit "${?}"; };

############################################################################

test -z "${HT:-}" && \
  { "${UNIFDEF:?}" "${@}"; exit "${?}"; };

############################################################################

exit 1

############################################################################
