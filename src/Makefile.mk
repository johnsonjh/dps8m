# DPS/8M simulator: src/Makefile.mk
# vim: filetype=make:tabstop=4:tw=76
#
###############################################################################
#
# Copyright (c) 2014-2016 Charles Anthony
# Copyright (c) 2016 Michal Tomek
# Copyright (c) 2021 Jeffrey H. Johnson <trnsz@pobox.com>
# Copyright (c) 2021 The DPS8M Development Team
#
# All rights reserved.
#
# This software is made available under the terms of the ICU
# License, version 1.8.1 or later.  For more details, see the
# LICENSE.md file at the top-level directory of this distribution.
#
###############################################################################
# src/Makefile.mk: Macro definitions and operating system detection defaults
###############################################################################
# Default configuration

TRUE       ?= true
SET        ?= set
ifdef V
    VERBOSE = 1
endif
ifdef VERBOSE
       ZCTV = cvf
       SETV = $(SET) -x
else
       ZCTV = cf
       SETV = $(TRUE)
endif
ENV        ?= env
CCACHE     ?= ccache
SHELL      ?= sh
SH         ?= $(SHELL)
UNAME      ?= uname
COMM       ?= comm
CPPCPP     ?= $(CC) -E
PREFIX     ?= /usr/local
CSCOPE     ?= cscope
MKDIR      ?= mkdir -p
NDKBUILD   ?= ndk-build
PKGCONFIG  ?= pkg-config
GIT        ?= git
GREP       ?= grep
SORT       ?= sort
CUT        ?= cut
SED        ?= sed
AWK        ?= awk
WEBDL      ?= wget
CD         ?= cd
TR         ?= tr
CAT        ?= cat
CMAKE      ?= cmake
RMNF       ?= rm
RMF        ?= $(RMNF) -f
CTAGS      ?= ctags
ETAGS      ?= etags
FIND       ?= find
CP         ?= cp -f
TOUCH      ?= touch
TEST       ?= test
PRINTF     ?= printf
TAR        ?= tar
MAKETAR    ?= $(TAR) --owner=dps8m --group=dps8m --posix -c                   \
                     --transform 's/^/.\/dps8\//g' -$(ZCTV)
TARXT      ?= $(TAR)
COMPRESS   ?= gzip -f -9
GUNZIP     ?= gzip -d
COMPRESSXT ?= gz
KITNAME    ?= sources

###############################################################################

ifdef MAKETRACE
  _SHELL := $(SHELL)
  SHELL = $(info [TRACE] src/Makefile.mk: [$@])$(_SHELL)
endif

###############################################################################

SIMHx=../simh

###############################################################################

ifneq ($(OS),Windows_NT)
  UNAME_S := $(shell  $(UNAME) -s)
  ifeq ($(UNAME_S),Darwin)
    OS = OSX
  endif
endif

###############################################################################

ifeq ($(OS), OSX)
  msys_version = 0
else
  msys_version :=                                                             \
    $(if $(findstring Msys, $(shell                                           \
        $(UNAME) -o)),$(word 1,                                               \
            $(subst ., ,$(shell $(UNAME) -r))),0)
endif

###############################################################################

ifeq ($(CROSS),MINGW64)
  CC = x86_64-w64-mingw32-gcc
ifeq ($(msys_version),0)
  AR = x86_64-w64-mingw32-ar
else
  AR = ar
endif
  EXE = .exe
else
CC ?= clang
endif

###############################################################################

# Default FLAGS
CFLAGS  += -g -O3 -fno-strict-aliasing
CFLAGS  += $(X_FLAGS)
LDFLAGS += $(X_FLAGS)

###############################################################################
# Windows MINGW

ifeq ($(OS),Windows_NT)
ifeq ($(CROSS),MINGW64)
    CFLAGS  += -I../mingw_include
    LDFLAGS += -L../mingw_lib
endif

###############################################################################
# macOS

else
    UNAME_S := $(shell $(UNAME) -s 2> /dev/null)
    ifeq ($(UNAME_S),Darwin)
      CFLAGS += -I/usr/local/include
      LDFLAGS += -L/usr/local/lib
endif

###############################################################################
# FreeBSD

    ifeq ($(UNAME_S),FreeBSD)
      CFLAGS += -I/usr/local/include -pthread
      LDFLAGS += -L/usr/local/lib
    endif

###############################################################################
# IBM AIX

# OS: IBM AIX 7.1-GA, 7.2-GA, 7.2 TL3, and 7.2 TL4 SP3+ on IBM POWER7+ pSeries
# Compiler: IBM AIX Toolbox GCC 8.3.0 (gcc-8-1.ppc -- powerpc-ibm-aix7.2.0.0),
# libuv: IBM AIX Toolbox libuv 1.38.1 (libuv-{devel}-1.38.1-1), or later, and,
# libpopt: IBM AIX Toolbox libpopt 1.18 (libpopt-1.18-1) or later is required.
# 32-bit builds, older OS versions, or older technology levels are not tested.
# Support for IBM XLC C/C++ Compiler on AIX is currently planned. Builds using
# the OSS4AIX/Perzl RPMs or the Bull/Atos toolchains are currently not tested.

    ifeq ($(UNAME_S),AIX)
      KRNBITS=$(shell getconf KERNEL_BITMODE 2> /dev/null || printf '%s' "64")
      CFLAGS += -DHAVE_POPT=1 -maix$(KRNBITS) -Wl,-b$(KRNBITS)
      LDFLAGS += -lm -lpthread -lpopt -maix$(KRNBITS) -Wl,-b$(KRNBITS)
      CC=gcc
      LD=gcc
    endif

###############################################################################
# GNU/Hurd

# Debian GNU/Hurd 2021: GNU-Mach 1.8+git20210821 / Hurd-0.9 / i686-AT386
# GCC: Debian 10.2.1-6+hurd.1 and Debian GCC 11.2.0-2
# Clang: Debian clang version 11.0.1-2 and Debian clang version 12.0.1-1

  ifeq ($(UNAME_S),GNU)
    export NEED_128=1
    export OS=GNU
    CFLAGS +=-UHAVE_DLOPEN
  endif

###############################################################################
# SunOS

# OpenIndiana illumos: GCC 7, GCC 10, Clang 9, all supported.
# Oracle Solaris 11: GCC only. Clang and SunCC need some work.

    ifeq ($(UNAME_S),SunOS)
      ifeq ($(shell $(UNAME) -o 2> /dev/null),illumos)
        ISABITS=$(shell isainfo -b 2> /dev/null || printf '%s' "64")
        CFLAGS  +=-I/usr/local/include -m$(ISABITS)
        LDFLAGS +=-L/usr/local/lib -lsocket -lnsl -lm -lpthread -m$(ISABITS)
      endif
      ifeq ($(shell $(UNAME) -o 2> /dev/null),Solaris)
        ISABITS=$(shell isainfo -b 2> /dev/null || printf '%s' "64")
        CFLAGS  +=-I/usr/local/include -m$(ISABITS)
        LDFLAGS +=-L/usr/local/lib -lsocket -lnsl -lm -lpthread -luv -lkstat  \
                  -ldl -m$(ISABITS)
        CC=gcc
      endif
    endif
endif

###############################################################################

CFLAGS += -I../decNumber -I$(SIMHx) -I../include
CFLAGS += -std=c99
CFLAGS += -U__STRICT_ANSI__
CFLAGS += -D_GNU_SOURCE
CFLAGS += -DUSE_READER_THREAD
CFLAGS += -DUSE_INT64

###############################################################################

ifneq ($(W),)
  CFLAGS +=-Wno-array-bounds
endif

###############################################################################

ifdef OS_IBMAIX
  export OS_IBMAIX
endif

###############################################################################

include ../Makefile.var

###############################################################################

%.o: %.c
	@$(PRINTF) '%s\n' "CC: $<"
	@$(SETV); $(CC) $(CFLAGS) $(CPPFLAGS) $(X_FLAGS) -c "$<" -o "$@"          \
    -DBUILDINFO_"$(shell printf '%s\n' "$@"                               |   \
    $(CUT) -f 1 -d '.'                                      2> /dev/null  |   \
    $(TR) -cd '\ [:alnum:]_'                                2> /dev/null ||   \
    $(PRINTF) '%s\n' "fallback"                             2> /dev/null      \
    )"="\"$(shell printf '%s\n'                                               \
    '$(CC) $(CFLAGS) $(CPPFLAGS) $(X_CFLAGS) $(LDFLAGS) $(LOCALLIBS) $(LIBS)' \
	| $(TR) -d '\\"'                                        2> /dev/null  |   \
    $(SED)  -e 's/ -DVER_CURRENT_TIME=....................... /\ /g'          \
            -e 's/^[ \t]*//;s/[ \t]*$$//'                   2> /dev/null  |   \
     $(AWK) -v RS='[,[:space:]]+' '!a[$$0]++{ printf "%s %s ", $$0, RT }'     \
         2> /dev/null | $(SED) -e 's/-I\.\.\/decNumber/\ /g'                  \
             -e 's/-I\.\.\/simh/\ /' -e 's/-I\.\.\/include/\ /g'              \
                 2> /dev/null | $(TR) -s ' '                             ||   \
                     $(PRINTF) '%s\n' "fallback")\""

###############################################################################

ifneq (,$(wildcard ../GNUmakefile.local))
  include ../GNUmakefile.local
endif

###############################################################################

ifneq (,$(wildcard GNUmakefile.local))
  include GNUmakefile.local
endif

###############################################################################

print-% : ; $(info src/mk: $* is a $(flavor $*) variable set to [$($*)]) \
              @(TRUE)

###############################################################################

# Local Variables:
# mode: make
# tab-width: 4
# End:
