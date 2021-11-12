all : 
	@printf '%s\n' "s1  Pull and build"
	@printf '%s\n' "s2  Build working directory"
	@printf '%s\n' "s3  Test MR12.7_install.ini"
	@printf '%s\n' "s4  Setup Yoyodyne"
	@printf '%s\n' "s5  Run ci_t1.expect"
	@printf '%s\n' "s6  Run isolts.expect"


.PHONY : s1 s2 s3 s4 s5 s6

s1:
	@printf '%s\n' "Start Stage 1: Build simulator"
ifndef NOREBUILD
	(cd ../.. && $(MAKE) distclean > /dev/null 2>&1 && $(MAKE))
endif
	@printf '%s\n' "End Stage 1"

s2:
	@printf '%s\n' "Start Stage 2: Build working directory"
	@rm -Rf ./run
	@mkdir -p ./run
	@mkdir -p ./run/tapes
	@mkdir -p ./run/tapes/12.7
	@mkdir -p ./run/tapes/general
	@mkdir -p ./run/disks
	@cp -p ../dps8/dps8 ./run
	@cp -p ./tapes/12.7* ./run/tapes/12.7
	@cp -p ./tapes/foo.tap ./run/tapes/general
	@cp -p ./ini/* ./run
	@cp -p ./ec/* ./run
	@printf '%s\n' "End Stage 2"

s3:
	@printf '%s\n' "Start Stage 3: Test MR12.7_install.ini"
	cd ./run && CPUPROFILE=install.prof.out ./dps8 -v MR12.7_install.ini
	@printf '%s\n' "End Stage 3"

s4:
	@printf '%s\n' "Start Stage 4: Setup Yoyodyne"
	cd ./run && CPUPROFILE=yoyodyne.prof.out ./dps8 -v yoyodyne.ini
	@printf '%s\n' "End Stage 4"

s5:
	@printf '%s\n' "Start Stage 5: Run ci_t1.expect"
	env CPUPROFILE=run.prof.out ./ci_t1.sh 0
	@printf '%s\n' "End Stage 5"

s6:
	@printf '%s\n' "Start Stage 6: Run isolts.expect"
	env CPUPROFILE=isolts.prof.out ./isolts.sh 0
	@printf '%s\n' "End Stage 5"

.PHONY : diff

diff :
	@printf '%s\n' "=============================" >ci_full.log
	@printf '%s\n' "=         ci.log            =" >>ci_full.log
	@printf '%s\n' "=============================" >>ci_full.log
	cat ci.log >>ci_full.log
	@printf '%s\n' "=============================" >>ci_full.log
	@printf '%s\n' "=        ci_t2.log          =" >>ci_full.log
	@printf '%s\n' "=============================" >>ci_full.log
	cat ci_t2.log >>ci_full.log
	@printf '%s\n' "=============================" >>ci_full.log
	@printf '%s\n' "=        ci_t3.log          =" >>ci_full.log
	@printf '%s\n' "=============================" >>ci_full.log
	cat ci_t3.log >>ci_full.log
	@printf '%s\n' "=============================" >>ci_full.log
	@printf '%s\n' "=        isolts.log         =" >>ci_full.log
	@printf '%s\n' "=============================" >>ci_full.log
	cat isolts.log >>ci_full.log
	dos2unix -f < ci_full.log | ./tidy > new.log
	dos2unix -f < ci_full.log.ref | ./tidy > old.log
	@printf '%s\n' "Done; compare new.log and old.log"

.PHONY : diff_files

diff_files :
	dos2unix -f < ci.log | ./tidy > new.log
	dos2unix -f < ci.log.ref | ./tidy > old.log
	@printf '%s\n' "Done; compare new.log and old.log"

kit:
	tar cvf - ci.makefile ci ci_full.log.ref *.expect init tidy README.txt tapes/* ec/* ini/* | lzip -9 > kit.tar.lz

