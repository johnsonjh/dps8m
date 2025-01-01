#!/usr/bin/env sh
# vim: filetype=sh:tabstop=4:ai:expandtab
# SPDX-License-Identifier: MIT-0
# scspell-id: 462dc4c9-f62c-11ec-ae81-80ee73e9b8e7
# Copyright (c) 2021-2025 The DPS8M Development Team

##############################################################################

export SHELL=/bin/sh

##############################################################################

command -v stdbuf > /dev/null 2>&1 && STDBUF="stdbuf -o L" || STDBUF="exec"

##############################################################################

exec ${STDBUF:?} expect -f isolts.expect 2>&1

##############################################################################
