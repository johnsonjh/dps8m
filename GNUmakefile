# DPS/8M simulator: GNUmakefile
# vim: filetype=make:tabstop=4:tw=78

##############################################################################
#
# Copyright (c) 2016 Charles Anthony
# Copyright (c) 2021 Jeffrey H. Johnson <trnsz@pobox.com>
# Copyright (c) 2021-2022 The DPS8M Development Team
#
# All rights reserved.
#
# This software is made available under the terms of the ICU
# License, version 1.8.1 or later.  For more details, see the
# LICENSE.md file at the top-level directory of this distribution.
#
##############################################################################

##############################################################################
# Build flags:
#
#   <------------------ Maintain spacing and formatting ------------------->
#
#     Build flag (ex: make V=1)             Description of build flag
#   #############################    #######################################
#
#       ATOMICS=AIX|BSD|GNU|SYNC        Define specific atomic operations
#         CROSS=MINGW64                 Enable MinGW-64 cross-compilation
#           L68=1                       Build as H6180/Level-68 simulator
#      NEED_128=1                       Enable provided 128-bit int types
#        NATIVE=1                       Enable "-march=native" via CFLAGS
#   NO_LOCKLESS=1                       Enable legacy (non-lockless) code
#        NO_LTO=1                       Disables the use of LTO for build
#   ROUND_ROBIN=1                       Enable non-threaded multiple CPUs
#     OS_PRINTF=1                       Enable standard *printf functions
#       TESTING=1                       Enable developmental testing mode
#          DUMA=1                       Enable the libDUMA malloc library
#             V=1                       Enable verbose compilation output
#             W=1                       Enable extra compilation warnings
#
#   <------------------ Maintain spacing and formatting ------------------->
#
##############################################################################

.DEFAULT_GOAL := all

##############################################################################
# Pre-build setup

MAKE_TOPLEVEL = 1

ifneq (,$(wildcard src/Makefile.pre))
  include src/Makefile.pre
  ifdef OS_IBMAIX
    export OS_IBMAIX
  endif
endif

ifneq (,$(wildcard src/Makefile.mk))
  include src/Makefile.mk
endif

export MAKE

export MAKE_TOPLEVEL

##############################################################################
# DPS8M Simulator
    # XXXX:    # ---------------------- DPS8/M Simulator --------------------
##############################################################################

##############################################################################
# Builds everything

.PHONY: build default all dps8 .rebuild.env
build default all dps8: .rebuild.env                                         \
    # build:    # Builds the DPS8/M simulator and tools
	-@$(PRINTF) '%s\n' "BUILD: Starting simulator and tools build"           \
      2> /dev/null || $(TRUE)
	@$(SETV); $(MAKE) -s -C "." ".rebuild.env";                              \
      $(TEST) -f ".needrebuild" && $(MAKE) -C "." "clean" || $(TRUE);        \
        $(MAKE) -C "src/dpsprintf"; $(MAKE) -C "src/dps8" "all" &&           \
          $(PRINTF) '%s\n' "BUILD: Successful simulator and tools build"     \
            2> /dev/null || $(TRUE)

##############################################################################
# Print build flags help

.PHONY: flags options .rebuild.env
options:                                                                     \
    # options:    # Display help for optional build flags
	-@$(PRINTF) '%s\n' "Optional build flags:" 2> /dev/null || $(TRUE)
	-@$(PRINTF) '\n     %s\n' "Usage: $(MAKE) { FLAG=VALUE ... }"            \
      2> /dev/null || $(TRUE)
	-@$(PRINTF) '%s\n' "      e.g. \"$(MAKE) TESTING=1 V=1 W=1\""            \
      2> /dev/null || $(TRUE)
	-@$(PRINTF) '\n%s\n'                                                     \
      "   Build flag (ex: make V=1)             Description of build flag"   \
        2> /dev/null || $(TRUE)
	-@$(PRINTF) ' %s '                                                       \
      "-----------------------------"                                        \
        " -----------------------------------------"                         \
          2> /dev/null || $(TRUE)
	-@$(PRINTF) '\n%s\n' "" 2> /dev/null || $(TRUE)
	@$(GREP) '^#.*=' "GNUmakefile" 2> /dev/null |                            \
      $(GREP) -v 'vim:' 2> /dev/null | $(GREP) -v 'Build flag'               \
       2> /dev/null | $(SED) 's/^#  //' 2> /dev/null ||                      \
		$(PRINTF) '%s\n' "Error: Unable to display help." || $(TRUE)
	-@$(PRINTF) '%s\n' "" 2> /dev/null || $(TRUE)

##############################################################################
# Builds punutil tool

.PHONY: punutil .rebuild.env
punutil: .rebuild.env                                                        \
    # punutil:    # Builds the punch card conversion tool
	-@$(PRINTF) '%s\n' "BUILD: Starting punutil build" 2> /dev/null ||       \
        $(TRUE)
	@$(MAKE) -s -C "." ".rebuild.env";                                       \
      $(TEST) -f ".needrebuild" && $(MAKE) -C "." "clean" || $(TRUE);        \
        $(MAKE) -C "src/punutil" "all" &&                                    \
          $(PRINTF) '%s\n' "BUILD: Successful punutil build" 2> /dev/null || \
            $(TRUE)

##############################################################################
# Builds prt2pdf tool

.PHONY: prt2pdf .rebuild.env
prt2pdf: .rebuild.env                                                        \
    # prt2pdf:    # Builds the prt2pdf PDF rendering tool
	-@$(PRINTF) '%s\n' "BUILD: Starting prt2pdf build" 2> /dev/null ||       \
        $(TRUE)
	@$(MAKE) -s -C "." ".rebuild.env";                                       \
      $(TEST) -f ".needrebuild" && $(MAKE) -C "." "clean" || $(TRUE);        \
        $(MAKE) -C "src/prt2pdf" "all" &&                                    \
          $(PRINTF) '%s\n' "BUILD: Successful prt2pdf build" 2> /dev/null || \
            $(TRUE)

##############################################################################
# Builds unifdef tool

.PHONY: unifdef .rebuild.env
unifdef: .rebuild.env                                                        \
    # unifdef:    # Builds the unifdef pre-processor tool
	-@$(PRINTF) '%s\n' "BUILD: Starting unifdef build" 2> /dev/null ||       \
        $(TRUE)
	@$(MAKE) -s -C "." ".rebuild.env";                                       \
      $(TEST) -f ".needrebuild" && $(MAKE) -C "." "clean" || $(TRUE);        \
        $(MAKE) -C "src/unifdef" "all" &&                                    \
          $(PRINTF) '%s\n' "BUILD: successful unifdef build" 2> /dev/null || \
            $(TRUE)

##############################################################################
# Builds mcmb tool

.PHONY: mcmb .rebuild.env
mcmb: .rebuild.env                                                           \
    # mcmb:    # Builds the minicmb combinatorics tool
	-@$(PRINTF) '%s\n' "BUILD: Starting mcmb build" 2> /dev/null || $(TRUE)
	-@$(MAKE) -s -C "." ".rebuild.env";                                      \
      $(TEST) -f ".needrebuild" && $(MAKE) -C "." "clean" || $(TRUE);        \
        $(MAKE) -C "src/mcmb" "all" &&                                       \
          $(PRINTF) '%s\n' "BUILD: Successful mcmb build" 2> /dev/null ||    \
            $(TRUE)

##############################################################################
# Builds vmpctool memory cache tool

.PHONY: vmpctool .rebuild.env
vmpctool: .rebuild.env                                                       \
    # vmpctool:    # Builds the vmpctool memory cache tool
	-@$(PRINTF) '%s\n' "BUILD: Starting vmpctool build" 2> /dev/null ||      \
        $(TRUE)
	-@$(MAKE) -s -C "." ".rebuild.env";                                      \
      $(TEST) -f ".needrebuild" && $(MAKE) -C "." "clean" || $(TRUE);        \
        $(MAKE) -C "src/vmpctool" "all" &&                                   \
          $(PRINTF) '%s\n' "BUILD: Successful vmpctool build"                \
            2> /dev/null || $(TRUE)

##############################################################################
# Builds blinkenLights2

.PHONY: blinkenLights2 .rebuild.env
blinkenLights2: .rebuild.env                                                 \
    # blinkenLights2:    # Builds the blinkenLights2 front panel
	-@$(PRINTF) '%s\n' "BUILD: Starting blinkenLights2 build"                \
        2> /dev/null || $(TRUE)
ifeq ($(V),1)
	@$(SETV); $(MAKE) -s -C "." ".rebuild.env";                              \
      $(TEST) -f ".needrebuild" && $(RMF) blinkenLights2 || $(TRUE);         \
        ( cd src/blinkenLights2 && $(ENV) VERBOSE=1                          \
          $(SHELL) ./blinkenLights2.build.sh ) &&                            \
            $(PRINTF) '%s\n' "BUILD: Successful blinkenLights2 build"        \
              2> /dev/null || $(TRUE)
else
	@$(MAKE) -s -C "." ".rebuild.env";                                       \
      $(TEST) -f ".needrebuild" && $(RMF) blinkenLights2 || $(TRUE);         \
        ( cd src/blinkenLights2 && $(SHELL) ./blinkenLights2.build.sh ) &&   \
          $(PRINTF) '%s\n' "BUILD: Successful blinkenLights2 build"          \
            2> /dev/null || $(TRUE)
endif

##############################################################################
# Initial GCC 10+ based PGO (profile-guided optimization) build

.PHONY: pgobuild
pgobuild:                                                                    \
    # pgobuild:    # Profile Guided Optimization (GCC 10+)
	-@$(PRINTF) '%s\n' "BUILD: Starting PGO build"   2> /dev/null || $(TRUE)
	@$(SETV); $(CC) --version 2>&1 | $(GREP) -wi "gcc" 2> /dev/null |        \
      $(GREP) -q '1[0-9]\.[1-9]' 2> /dev/null ||                             \
       { $(PRINTF) %s\\n "ERROR: Version check failed, need GCC 10+, have:"; \
            $(CC) --version 2>&1 | $(GREP) -wi "gcc"; exit 1; }
	@$(SETV); $(PRINTF) %s\\n "$(CC)" 2> /dev/null |                         \
        $(GREP) -q "gcc" 2> /dev/null ||                                     \
            { $(PRINTF) %s\\n                                                \
                "ERROR: \"CC=$(CC)\", failed substring match for \"gcc\"."   \
                ""                                                           \
             " *********************************************************"    \
             " ***  The \"CC\" environment variable MUST be explicity  ***"  \
             " *** specified when using Profile Guided Optimization! ***"    \
             " ***        Example: \"env CC=gcc make pgobuild\"        ***"  \
             " *********************************************************"    \
                ""; $(SLEEP) 1;                                              \
                 exit 1; }
	@($(SETV); $(SH) -c '$(SETV); $(PRINTF) "$(SETV)" | $(GREP) -q true ||   \
     { export VERBOSE=1; $(PRINTF) %s\\n "PGO: Verbose mode enabled."; } &&  \
         export XXCFLAGS="-Wl,--stats $(CFLAGS)" &&                          \
           export XXLDFLAGS="-Wl,--stats $(LDFLAGS)" &&                      \
       $(PRINTF) %s\\n "PGO: Cleaning up" &&                                 \
         export PMODE="generate" &&                                          \
         $(MAKE) "superclean" &&                                             \
         export LDFLAGS="$${XXLDFLAGS:?} -fprofile-partial-training          \
                         -fprofile-correction -fprofile-$${PMODE:?}          \
                         --param hot-bb-count-ws-permille=1000" &&           \
         export CFLAGS="$${XXCFLAGS:?} -fprofile-partial-training            \
                        -Wno-error=coverage-mismatch                         \
                        -fprofile-correction -fprofile-$${PMODE:?}           \
                        --param hot-bb-count-ws-permille=1000" &&            \
        $(PRINTF) %s\\n "PGO: Building instrumentation build stage" &&       \
        $(MAKE) "build" &&                                                   \
        $(PRINTF) %s\\n "PGO: Profiling in progress - use ^E to abort" &&    \
        $(PRINTF) %s\\n ""                                                   \
                        " *************************************************" \
                        " * Patience! Profiling can take 5 to 75 minutes! *" \
                        " *************************************************" \
                        "" ; $(SLEEP) 1 ;                                    \
       ( $(CD) ./src/perf_test &&                                            \
         ( ../dps8/dps8 -vr nqueensx.ini 2>&1 | $(GREP) -w MIPS ) &&         \
           ( ../mcmb/mcmb -v > /dev/null 2>&1 || $(TRUE) );                  \
             ( ../punutil/punutil -h > /dev/null 2>&1 || $(TRUE) );          \
               ( ../prt2pdf/prt2pdf -v > /dev/null 2>&1 || $(TRUE) );        \
                 ( ../unifdef/unifdef -v > /dev/null 2>&1 || $(TRUE) );      \
                   ( ../vmpctool/vmpctool -q ../vmpctool/vmpctool            \
                      > /dev/null 2>&1 || $(TRUE) ) );                       \
         export PMODE="use" &&                                               \
         export LDFLAGS="$${XXLDFLAGS:?} -fprofile-partial-training          \
                         -fprofile-correction -fprofile-$${PMODE:?}          \
                         --param hot-bb-count-ws-permille=1000" &&           \
         export CFLAGS="$${XXCFLAGS:?} -fprofile-partial-training            \
                        -Wno-error=coverage-mismatch                         \
                        -fprofile-correction -fprofile-$${PMODE:?}           \
                        --param hot-bb-count-ws-permille=1000" &&            \
         $(PRINTF) %s\\n "PGO: Cleaning up ..." &&                           \
         $(MAKE) "clean" &&                                                  \
         $(PRINTF) %s\\n "PGO: Starting profile guided build stage" &&       \
         $(MAKE) "build";                                                    \
    ')
	-@$(PRINTF) '%s\n' "BUILD: Successful PGO build" 2> /dev/null || $(TRUE)

##############################################################################
# Install

.PHONY: install
install:                                                                     \
    # install:    # Builds and installs the sim and tools
	-@$(PRINTF) '%s\n' "BUILD: Starting install" || $(TRUE)
	@$(MAKE) -C "src/dps8" "install" &&                                      \
      $(PRINTF) '%s\n' "BUILD: Successful install" || $(TRUE)

##############################################################################
# Clean up compiled objects and executables

.PHONY: clean
ifneq (,$(findstring clean,$(MAKECMDGOALS)))
.NOTPARALLEL: clean
endif
clean:                                                                       \
    # clean:    # Cleans up executable and object files
	-@$(PRINTF) '%s\n' "BUILD: Starting clean" 2> /dev/null || $(TRUE)
	@$(RMF) ".needrebuild" || $(TRUE)
	@$(RMF) ".rebuild.vne" || $(TRUE)
	@$(MAKE) -C "src/dps8" "clean" &&                                        \
      $(PRINTF) '%s\n' "BUILD: Successful clean" 2> /dev/null || $(TRUE)

##############################################################################
# Cleans everything `clean` does, plus version info, logs, and state files

.PHONY: distclean
ifneq (,$(findstring clean,$(MAKECMDGOALS)))
.NOTPARALLEL: distclean
endif
distclean: clean                                                             \
    # distclean:    # Cleans up tree to pristine conditions
	-@$(PRINTF) '%s\n' "BUILD: Starting distclean" 2> /dev/null || $(TRUE)
	@$(RMF) ".needrebuild" || $(TRUE)
	@$(RMF) ".rebuild.env" || $(TRUE)
	@$(RMF) ".rebuild.vne" || $(TRUE)
	@$(MAKE) -C "src/dps8" "distclean" &&                                    \
      $(PRINTF) '%s\n' "BUILD: Successful distclean" 2> /dev/null || $(TRUE)

##############################################################################
# Cleans everything `distclean` does, plus attempts to flush compiler caches

.PHONY: superclean realclean reallyclean
ifneq (,$(findstring clean,$(MAKECMDGOALS)))
.NOTPARALLEL: superclean realclean reallyclean
endif
superclean realclean reallyclean: distclean                                  \
    # superclean:    # Cleans up tree fully and flush ccache
	-@$(PRINTF) '%s\n' "BUILD: Starting superclean" 2> /dev/null || $(TRUE)
	@$(MAKE) -C "src/dps8" "superclean" &&                                   \
      $(PRINTF) '%s\n' "BUILD: Successful superclean" 2> /dev/null || $(TRUE)

##############################################################################

ifneq (,$(wildcard src/Makefile.env))
  include src/Makefile.env
endif

###############################################################################
# Help and Debugging Targets                                                  \
    # XXXX:    # --------------------- Help and Debugging -------------------
###############################################################################

ifneq (,$(wildcard src/Makefile.scc))
  include src/Makefile.scc
endif

##############################################################################

ifneq (,$(wildcard src/Makefile.loc))
  include src/Makefile.loc
endif

##############################################################################

ifneq (,$(wildcard src/Makefile.dev))
  include src/Makefile.dev
endif

##############################################################################

ifneq (,$(wildcard src/Makefile.dep))
  include src/Makefile.dep
endif

##############################################################################
# Print help output

.PHONY: help info
help info:                                                                   \
    # help:    # Display this list of Makefile targets
	@$(GREP) -E '^.* # .*:    # .*$$' $(MAKEFILE_LIST) 2> /dev/null        | \
        $(AWK) 'BEGIN { FS = "    # " };                                     \
          { printf "%s%-18s %-40s (%-19s)\n", $$1, $$2, $$3, $$1 }'          \
            2> /dev/null | $(CUT) -d ':' -f 2- 2> /dev/null                | \
              $(SED) -e 's/:\ \+)$$/)/' -e 's/XXXX:/\n/g' 2> /dev/null     | \
                $(SED) -e 's/---- (.*)$$/------------\n/'                    \
                 -e 's/:  )/)/g' -e 's/:       )/)/g'                        \
                  -e 's/              ---/-------------/' 2> /dev/null     | \
                    $(GREP) -v 'GREP' 2> /dev/null                         | \
                      $(GREP) -v '_LIST' 2> /dev/null | $(SED) 's/^n//g'   | \
                       $(SED) 's/n$$//g'                                  || \
                       { $(PRINTF) '%s\n' "Error: Unable to display help.";  \
                          $(TRUE) > /dev/null 2>&1; }                     || \
                            $(TRUE) > /dev/null 2>&1
	@$(PRINTF) '%s\n' "" 2> /dev/null || $(TRUE) > /dev/null 2>&1

##############################################################################

# Local Variables:
# mode: make
# tab-width: 4
# End:
