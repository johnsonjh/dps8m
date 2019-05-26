#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>

#include "dps8.h"
#include "dps8_cpu.h"
#include "dps8_iom.h"
#include "dps8_cable.h"
#include "dps8_state.h"
#include "shm.h"
//#include "threadz.h"

struct system_state_s * system_state;
vol word36 * M = NULL;                                          // memory
cpu_state_t * cpus;
vol cpu_state_t * cpup_;
#undef cpu
#define cpu (* cpup_)

#ifdef CTRACE
static word36 append (uint cpun, uint segno, uint offset)
  {
    // prds is segment 072
    //uint segno = 072;
    word24 dsbr_addr = cpus[cpun].DSBR.ADDR;
//printf ("dsbr %08o\n", dsbr_addr);
    if (! dsbr_addr)
      return 1;
    // punt on unpaged ds
    if (cpus[cpun].DSBR.U)
      return 2;
    word24 x1 = (2u * segno) / 1024u; // floor
    word36 PTWx1 = M[dsbr_addr + x1];
//printf ("PTRx1 %012llo\n", PTWx1);
    if (TSTBIT (PTWx1, 2) == 0) // df - page not in memory
      return 3;
    word24 y1 = (2 * segno) % 1024;
    word36 SDWeven = M [(GETHI (PTWx1) << 6) + y1];
    word36 SDWodd = M [(GETHI (PTWx1) << 6) + y1 + 1];
//printf ("SDWeven %012llo\n", SDWeven);
//printf ("SDWodd %012llo\n", SDWodd);
    if (TSTBIT (SDWeven, 2) == 0)
      return 4;
    if (TSTBIT (SDWodd, 16)) // .U
      return 5;
    word24 sdw_addr = (SDWeven >> 12) & 077777777;
//printf ("sdw_addr %08o\n", sdw_addr);
    word24 x2 = (offset) / 1024; // floor

    word24 y2 = offset % 1024;
    word36 PTWx2 = M[sdw_addr + x2];
    word24 PTW1_addr= GETHI (PTWx2);
    word1 PTW0_U = TSTBIT (PTWx2, 9);
    if (! PTW0_U)
      return 6;
    word24 finalAddress = ((((word24)PTW1_addr & 0777760) << 6) + y2) & PAMASK;
    return M[finalAddress];
    //return (word24) (GETHI (PTWx1) << 6);
  }
#endif

// Sampling loop
//   Sample rate: 1000 Hz
#define USLEEP_1KHz 1000 // 1000 usec is 1 msec
//   Display update rate: 1 Hz
#define UPDATE_RATE 1000  // Every 1000 samples is 1 sec.

int main (int argc, char * argv[])
  {
    for (;;)
      {
        system_state = (struct system_state_s *)
          open_shm ("state", sizeof (struct system_state_s));

        if (system_state)
          break;

        printf ("No state file found; retry in 1 second\n");
        sleep (1);
      }

    initscr ();

    M = system_state->M;
    cpus = system_state->cpus;
// We don't have access to the thread state data structure, so we
// can't easily tell how many or which CPUs are running. We can watch
// the cycle count, tho. If it is 0, then the CPU hasn't been started;
// If it isn't changing, then it has probably been stopped.

#ifdef LOCKLESS
//#define for_cpus for (uint cpun = 0; cpun < 2; cpun ++)
#define for_cpus for (uint cpun = 0; cpun < N_CPU_UNITS_MAX; cpun ++)
#else
#define for_cpus for (uint cpun = 0; cpun < 1; cpun ++)
#endif

    unsigned long long last_cycle_cnt[N_CPU_UNITS_MAX] = {0,0,0,0,0,0,0,0};
    int dis_cnt[N_CPU_UNITS_MAX] = {0,0,0,0,0,0,0,0};

    for_cpus
      last_cycle_cnt[cpun] = cpus[cpun].cycleCnt;

    //for_cpus last_cycle_cnt[cpun] = cpus[cpun].cycleCnt;

    long sample_ctr = 0;
    for (;;)
      {
// Once a second, update display
        if (sample_ctr && sample_ctr % UPDATE_RATE == 0)
          {
            clear ();
            unsigned long disk_seeks = __atomic_exchange_n (& system_state->profiler.disk_seeks, 0, __ATOMIC_SEQ_CST);
            unsigned long disk_reads = __atomic_exchange_n (& system_state->profiler.disk_reads, 0, __ATOMIC_SEQ_CST);
            unsigned long disk_writes = __atomic_exchange_n (& system_state->profiler.disk_writes, 0, __ATOMIC_SEQ_CST);
            unsigned long disk_read = __atomic_exchange_n (& system_state->profiler.disk_read, 0, __ATOMIC_SEQ_CST);
            unsigned long disk_written = __atomic_exchange_n (& system_state->profiler.disk_written, 0, __ATOMIC_SEQ_CST);
            printw ("DISK S %06d R %06d W %06d MB/S %4d\n", disk_seeks, disk_reads, disk_writes, (disk_read + disk_written) * 9 / 2 / 1048576);

            for_cpus
              {
                cpup_ = (cpu_state_t *) & cpus[cpun];
#ifdef LOCKLESS
                if (! cpu.run)
                  continue;
#endif
                unsigned long long cnt = __atomic_load_n (& cpu.cycleCnt, __ATOMIC_ACQUIRE);
                float dis_pct = ((float) (dis_cnt[cpun] * 100)) / UPDATE_RATE;
#ifdef CTRACE
                printw ("CPU %c Cycles %10lld %5.1f%% %012llo %012llo\n", 'A' + cpun, cnt - last_cycle_cnt [cpun], 100.0 - dis_pct, append (cpun, 072, 20), append (cpun, 072, 21));
#else
                printw ("CPU %c Cycles %10lld %5.1f%%\n", 'A' + cpun, cnt - last_cycle_cnt [cpun], 100.0 - dis_pct);
#endif
                printw ("%05o:%06o %012llo A: %012llo Q: %012llo",
                        cpu.PPR.PSR, cpu.PPR.IC, IWB_IRODD, cpu.rA, cpu.rQ);
                dis_cnt[cpun] = 0;
                last_cycle_cnt [cpun] = cnt;

                uint cioc_iom = __atomic_exchange_n (& cpu.cioc_iom, 0, __ATOMIC_SEQ_CST);
                uint cioc_cpu = __atomic_exchange_n (& cpu.cioc_cpu, 0, __ATOMIC_SEQ_CST);
                uint intrs = __atomic_exchange_n (& cpu.intrs, 0, __ATOMIC_SEQ_CST);
                printw (" CIOC IOM %4u CPU %4u INTRS %4u\n", cioc_iom, cioc_cpu, intrs);


                for (uint fn = 0; fn < 32; fn ++)
                  {
                    if (fn == 0)
                      printw ("     SDF     STR     MME     F1      TRO     CMD     DRL     LUF     CON     PAR     IPR     ONC     SUF     OFL     DIV     EXF\n");
                    else if (fn == 16)
                      printw ("     DF0     DF1     DF2     DF3     ACV     MME2    MME3    MME4    F2      F3      UN1     UN2     UN3     UN4     UN5     TRB\n");
                    unsigned long fcnt = __atomic_exchange_n (& cpu.faults[fn], 0, __ATOMIC_SEQ_CST);
                    printw ("%8u", fcnt);
                    if (fn % 16 == 15)
                      printw ("\n");
                  }



                printw ("\n");
              }

            refresh ();
          }

        for_cpus
          {
            cpup_ = (cpu_state_t *) & cpus[cpun];
#ifdef LOCKLESS
            if (! cpu.run)
              continue;
#endif
            if (cpu.currentInstruction.opcode10 == 00616)
              dis_cnt[cpun] ++;
          }

        usleep (USLEEP_1KHz); // 1 ms
        sample_ctr ++;
     }

    return 0;
  }


