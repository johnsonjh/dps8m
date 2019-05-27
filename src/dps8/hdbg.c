/*
 Copyright 2016 by Charles Anthony

 All rights reserved.

 This software is made available under the terms of the
 ICU License -- ICU 1.8.1 and later.
 See the LICENSE file at the top-level directory of this distribution and
 at https://sourceforge.net/p/dps8m/code/ci/master/tree/LICENSE
 */

// history debugging
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dps8.h"
#include "dps8_sys.h"
#include "dps8_cpu.h"
#include "dps8_utils.h"
#include "hdbg.h"


#ifdef HDBG
#include "dps8_faults.h"

enum hevtType { hevtEmpty = 0, hevtTrace, hevtMRead, hevtMWrite, hevtIWBUpdate, hevtRegs, hevtFault, hevtIntrSet, hevtIntr, hevtReg, hevtPAReg };

struct hevt
  {
    enum hevtType type;
    uint64 time;
    uint cpu_idx;
    union
      {
        struct
          {
            addr_modes_e addrMode;
            word15 segno;
            word18 ic;
            word3 ring;
            word36 inst;
          } trace;

        struct
          {
            word24 addr;
            word36 data;
          } memref;

        struct
          {
            _fault faultNumber;
            _fault_subtype subFault;
            char faultMsg[64];
          } fault;

        struct
          {
            uint inum;
            uint cpuUnitIdx;
            uint scuUnitIdx;
          } intrSet;

        struct
          {
            uint intr_pair_addr;
          } intr;

        struct
          {
            enum hregs_t type;
            word36 data;
          } reg;

        struct
          {
            enum hregs_t type;
            struct par_s data;
          } par;
      };
  };

static struct hevt * hevents = NULL;
static unsigned long hdbgSize = 0;
static unsigned long hevtPtr = 0;
static unsigned long hevtMark = 0;

//#ifdef LOCKLESS
//static pthread_mutex_t hdbg_lock = PTHREAD_MUTEX_INITIALIZER;
//#endif

#if 1
#define CPUN if (current_running_cpu_idx < 1 || current_running_cpu_idx > 3) return
#else
#define CPUN
#endif

static void createBuffer (void)
  {
    if (hevents)
      {
        free (hevents);
        hevents = NULL;
      }
    if (! hdbgSize)
      return;
    hevents = malloc (sizeof (struct hevt) * hdbgSize);
    if (! hevents)
      {
        sim_printf ("hdbg createBuffer failed\n");
        return;
      }
    memset (hevents, 0, sizeof (struct hevt) * hdbgSize);

    hevtPtr = 0;
  }

#if 0
static void hdbg_inc (void)
  {
    hevtPtr = (hevtPtr + 1) % hdbgSize;
    if (hevtMark)
      {
        hevtMark --;
        if (hevtMark == 0)
          hdbgPrint ();
      }
  }
#endif

static unsigned long hdbg_inc (void)
  {
    return __sync_fetch_and_add (& hevtPtr, 1l) % hdbgSize;
  }

void hdbgTrace (void)
  {
CPUN;
//#ifdef LOCKLESS
//    pthread_mutex_lock (& hdbg_lock);
//#endif
    if (! hevents)
      goto done;
#ifdef ISOLTS
if (current_running_cpu_idx == 0)
  goto done;
#endif
    unsigned long p = hdbg_inc ();
    hevents[p].type = hevtTrace;
    hevents[p].cpu_idx = current_running_cpu_idx;
    hevents[p].time = cpu.cycleCnt;
    hevents[p].trace.addrMode = get_addr_mode ();
    hevents[p].trace.segno = cpu.PPR.PSR;
    hevents[p].trace.ic = cpu.PPR.IC;
    hevents[p].trace.ring = cpu.PPR.PRR;
    hevents[p].trace.inst = cpu.cu.IWB;
    //hdbg_inc ();
done: ;
//#ifdef LOCKLESS
//    pthread_mutex_unlock (& hdbg_lock);
//#endif
  }

void hdbgMRead (word24 addr, word36 data)
  {
CPUN;
//#ifdef LOCKLESS
//    pthread_mutex_lock (& hdbg_lock);
//#endif
    if (! hevents)
      goto done;
#ifdef ISOLTS
if (current_running_cpu_idx == 0)
  goto done;
#endif
    unsigned long p = hdbg_inc ();
    hevents[p].type = hevtMRead;
    hevents[p].cpu_idx = current_running_cpu_idx;
    hevents[p].time = cpu.cycleCnt;
    hevents[p].memref.addr = addr;
    hevents[p].memref.data = data;
    //hdbg_inc ();
done: ;
//#ifdef LOCKLESS
//    pthread_mutex_unlock (& hdbg_lock);
//#endif
  }

void hdbgMWrite (word24 addr, word36 data)
  {
CPUN;
//#ifdef LOCKLESS
//    pthread_mutex_lock (& hdbg_lock);
//#endif
    if (! hevents)
      goto done;
#ifdef ISOLTS
if (current_running_cpu_idx == 0)
  goto done;
#endif
    unsigned long p = hdbg_inc ();
    hevents[p].type = hevtMWrite;
    hevents[p].cpu_idx = current_running_cpu_idx;
    hevents[p].time = cpu.cycleCnt;
    hevents[p].memref.addr = addr;
    hevents[p].memref.data = data;
    //hdbg_inc ();
done: ;
//#ifdef LOCKLESS
//    pthread_mutex_unlock (& hdbg_lock);
//#endif
  }

void hdbgFault (_fault faultNumber, _fault_subtype subFault,
                const char * faultMsg)
  {
CPUN;
//#ifdef LOCKLESS
//    pthread_mutex_lock (& hdbg_lock);
//#endif
    if (! hevents)
      goto done;
#ifdef ISOLTS
if (current_running_cpu_idx == 0)
  goto done;
#endif
    unsigned long p = hdbg_inc ();
    hevents[p].type = hevtFault;
    hevents[p].cpu_idx = current_running_cpu_idx;
    hevents[p].time = cpu.cycleCnt;
    hevents[p].fault.faultNumber = faultNumber;
    hevents[p].fault.subFault = subFault;
    strncpy (hevents[p].fault.faultMsg, faultMsg, 63);
    hevents[p].fault.faultMsg[63] = 0;
    //hdbg_inc ();
done: ;
//#ifdef LOCKLESS
//    pthread_mutex_unlock (& hdbg_lock);
//#endif
  }

void hdbgIntrSet (uint inum, uint cpuUnitIdx, uint scuUnitIdx)
  {
CPUN;
//#ifdef LOCKLESS
//    pthread_mutex_lock (& hdbg_lock);
//#endif
    if (! hevents)
      goto done;
#ifdef ISOLTS
if (current_running_cpu_idx == 0)
  goto done;
#endif
    unsigned long p = hdbg_inc ();
    hevents[p].type = hevtIntrSet;
    hevents[p].cpu_idx = current_running_cpu_idx;
    hevents[p].time = cpu.cycleCnt;
    hevents[p].intrSet.inum = inum;
    hevents[p].intrSet.cpuUnitIdx = cpuUnitIdx;
    hevents[p].intrSet.scuUnitIdx = scuUnitIdx;
    //hdbg_inc ();
done: ;
//#ifdef LOCKLESS
//    pthread_mutex_unlock (& hdbg_lock);
//#endif
  }

void hdbgIntr (uint intr_pair_addr)
  {
CPUN;
//#ifdef LOCKLESS
//    pthread_mutex_lock (& hdbg_lock);
//#endif
    if (! hevents)
      goto done;
#ifdef ISOLTS
if (current_running_cpu_idx == 0)
  goto done;
#endif
    unsigned long p = hdbg_inc ();
    hevents[p].type = hevtIntr;
    hevents[p].cpu_idx = current_running_cpu_idx;
    hevents[p].time = cpu.cycleCnt;
    hevents[p].intr.intr_pair_addr = intr_pair_addr;
    //hdbg_inc ();
done: ;
//#ifdef LOCKLESS
//    pthread_mutex_unlock (& hdbg_lock);
//#endif
  }

void hdbgReg (enum hregs_t type, word36 data)
  {
CPUN;
//#ifdef LOCKLESS
//    pthread_mutex_lock (& hdbg_lock);
//#endif
    if (! hevents)
      goto done;
#ifdef ISOLTS
if (current_running_cpu_idx == 0)
  goto done;
#endif
    unsigned long p = hdbg_inc ();
    hevents[p].type = hevtReg;
    hevents[p].cpu_idx = current_running_cpu_idx;
    hevents[p].time = cpu.cycleCnt;
    hevents[p].reg.type = type;
    hevents[p].reg.data = data;
    //hdbg_inc ();
done: ;
//#ifdef LOCKLESS
//    pthread_mutex_unlock (& hdbg_lock);
//#endif
  }


void hdbgPAReg (enum hregs_t type, struct par_s * data)
  {
CPUN;
//#ifdef LOCKLESS
//    pthread_mutex_lock (& hdbg_lock);
//#endif
    if (! hevents)
      goto done;
#ifdef ISOLTS
if (current_running_cpu_idx == 0)
  goto done;
#endif
    unsigned long p = hdbg_inc ();
    hevents[p].type = hevtPAReg;
    hevents[p].cpu_idx = current_running_cpu_idx;
    hevents[p].time = cpu.cycleCnt;
    hevents[p].par.type = type;
    hevents[p].par.data =  * data;
    //hdbg_inc ();
done: ;
//#ifdef LOCKLESS
//    pthread_mutex_unlock (& hdbg_lock);
//#endif
  }

static FILE * hdbgOut = NULL;

static void printMRead (struct hevt * p)
  {
    fprintf (hdbgOut, "DBG(%"PRId64")> CPU %d FINAL: Read %08o %012"PRIo64"\n",
                p -> time, 
                p -> cpu_idx,
                p -> memref.addr, p -> memref.data);
  }

static void printMWrite (struct hevt * p)
  {
    fprintf (hdbgOut, "DBG(%"PRId64")> CPU %d FINAL: Write %08o %012"PRIo64"\n",
                p -> time, 
                p -> cpu_idx,
                p -> memref.addr, p -> memref.data);
  }

static void printTrace (struct hevt * p)
  {
    char buf[256];
    if (p -> trace.addrMode == ABSOLUTE_mode)
      {
        fprintf (hdbgOut, "DBG(%"PRId64")> CPU %d TRACE: %06o %o %012"PRIo64" (%s)\n",
                    p -> time, 
                    p -> cpu_idx,
                    p -> trace.ic, p -> trace.ring,
                    p -> trace.inst, disassemble (buf, p -> trace.inst));
      }
    else
      {
        fprintf (hdbgOut, "DBG(%"PRId64")> CPU %d TRACE: %05o:%06o %o %012"PRIo64" (%s)\n",
                    p -> time, p -> cpu_idx, p -> trace.segno,
                    p -> trace.ic, p -> trace.ring,
                    p -> trace.inst, disassemble (buf, p -> trace.inst));
      }
  }

static void printFault (struct hevt * p)
  {
    fprintf (hdbgOut, "DBG(%"PRId64")> CPU %d FAULT: Fault %d(0%o), sub %"PRId64"(0%"PRIo64"), '%s'\n",
                p -> time, 
                p -> cpu_idx,
                p -> fault.faultNumber, p -> fault.faultNumber,
                p -> fault.subFault.bits, p -> fault.subFault.bits,
                p -> fault.faultMsg);
  }

static void printIntrSet (struct hevt * p)
  {
    fprintf (hdbgOut, "DBG(%"PRId64")> CPU %d INTR_SET: Number %d(0%o), CPU %u SCU %u\n",
                p -> time, 
                p -> cpu_idx,
                p -> intrSet.inum, p -> intrSet.inum,
                p -> intrSet.cpuUnitIdx,
                p -> intrSet.scuUnitIdx);
  }

static void printIntr (struct hevt * p)
  {
    fprintf (hdbgOut, "DBG(%"PRId64")> CPU %d INTR: Interrupt pair address %o\n",
                p -> time, 
                p -> cpu_idx,
                p -> intr.intr_pair_addr);
  }

static char * regNames[] =
  {
    "A ",
    "Q ",
    "X0", "X1", "X2", "X3", "X4", "X5", "X6", "X7",
    "AR0", "AR1", "AR2", "AR3", "AR4", "AR5", "AR6", "AR7",
    "PR0", "PR1", "PR2", "PR3", "PR4", "PR5", "PR6", "PR7"
  };

static void printReg (struct hevt * p)
  {
    if (p->reg.type >= hreg_X0 && p->reg.type <= hreg_X7)
      fprintf (hdbgOut, "DBG(%"PRId64")> CPU %d REG: %s %06"PRIo64"\n",
                  p->time, 
                  p->cpu_idx,
                  regNames[p->reg.type],
                  p->reg.data);
    else
      fprintf (hdbgOut, "DBG(%"PRId64")> CPU %d REG: %s %012"PRIo64"\n",
                  p->time, 
                  p->cpu_idx,
                  regNames[p->reg.type],
                  p->reg.data);
  }

static void printPAReg (struct hevt * p)
  {
    if (p->reg.type >= hreg_PR0 && p->reg.type <= hreg_PR7)
      fprintf (hdbgOut, "DBG(%"PRId64")> CPU %d REG: %s "
               "%05o:%06o BIT %2o RNR %o\n",
               p->time, 
               p->cpu_idx,
               regNames[p->reg.type],
               p->par.data.SNR,
               p->par.data.WORDNO,
               p->par.data.PR_BITNO,
               p->par.data.RNR);
    else
      fprintf (hdbgOut, "DBG(%"PRId64")> CPU %d REG: %s "
               "%05o:%06o CHAR %o BIT %2o RNR %o\n",
               p->time, 
               p->cpu_idx,
               regNames[p->reg.type],
               p->par.data.SNR,
               p->par.data.WORDNO,
               p->par.data.AR_CHAR,
               p->par.data.AR_BITNO,
               p->par.data.RNR);
  }

void hdbgPrint (void)
  {
    sim_printf ("hdbg print\n");
//#ifdef LOCKLESS
//    pthread_mutex_lock (& hdbg_lock);
//#endif
    if (! hevents)
      goto done;
    struct hevt * t = hevents;
    hevents = NULL;
    hdbgOut = fopen ("hdbg.list", "w");
    if (! hdbgOut)
      {
        sim_printf ("can't open hdbg.list\n");
        goto done;
      }
    time_t curtime;
    time (& curtime);
    fprintf (hdbgOut, "%s\n", ctime (& curtime));

    for (unsigned long p = 0; p < hdbgSize; p ++)
      {
        unsigned long q = (hevtPtr + p) % hdbgSize;
        struct hevt * evtp = t + q;
        switch (evtp -> type)
          {
            case hevtEmpty:
              break;

            case hevtTrace:
              printTrace (evtp);
              break;
                
            case hevtMRead:
              printMRead (evtp);
              break;
                
            case hevtMWrite:
              printMWrite (evtp);
              break;
                
            case hevtFault:
              printFault (evtp);
              break;
                
            case hevtIntrSet:
              printIntrSet (evtp);
              break;
                
            case hevtIntr:
              printIntr (evtp);
              break;
                
            case hevtReg:
              printReg (evtp);
              break;
                
            case hevtPAReg:
              printPAReg (evtp);
              break;
                
            default:
              fprintf (hdbgOut, "hdbgPrint ? %d\n", evtp -> type);
              break;
          }
      }
    fclose (hdbgOut);
#if 0
    int fd = open ("M.dump", O_WRONLY | O_CREAT, 0660);
    if (fd == -1)
      {
        sim_printf ("can't open M.dump\n");
        goto done;
      }
    // cast discards volatile
    /* ssize_t n = */ write (fd, (const void *) M, MEMSIZE * sizeof (word36));
    close (fd);
#endif
done: ;
//#ifdef LOCKLESS
//    pthread_mutex_unlock (& hdbg_lock);
//#endif
  }

void hdbg_mark (void)
  {
//#ifdef LOCKLESS
//    pthread_mutex_lock (& hdbg_lock);
//#endif
    hevtMark = hdbgSize;
    sim_printf ("hdbg mark set to %ld\n", hevtMark);
//#ifdef LOCKLESS
//    pthread_mutex_unlock (& hdbg_lock);
//#endif
  }

// set buffer size 
t_stat hdbg_size (UNUSED int32 arg, const char * buf)
  {
//#ifdef LOCKLESS
//    pthread_mutex_lock (& hdbg_lock);
//#endif
    hdbgSize = strtoul (buf, NULL, 0);
    sim_printf ("hdbg size set to %ld\n", hdbgSize);
    createBuffer ();
//#ifdef LOCKLESS
//    pthread_mutex_unlock (& hdbg_lock);
//#endif
    return SCPE_OK;
  }

t_stat hdbg_print (UNUSED int32 arg, const char * buf)
  {
    hdbgPrint ();
    return SCPE_OK;
  }
#else
t_stat hdbg_size (UNUSED int32 arg, UNUSED const char * buf)
  {
    sim_printf ("hdbg not enabled; ignoring\n");
    return SCPE_OK;
  }
#endif // HDBG
