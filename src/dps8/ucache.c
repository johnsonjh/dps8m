#include <string.h>

#include "dps8.h"
#include "dps8_cpu.h"

void ucInvalidate (void) {
  memset (cpu.uCache.caches, 0, sizeof (cpu.uCache.caches));
  memset (cpu.uCache.operandReadBigCache, 0, sizeof (cpu.uCache.operandReadBigCache));
}

void ucCacheSave (uint ucNum, word15 segno, word18 offset, word14 bound, word1 p, word24 address, word3 r1, bool paged) {
  ucache_t * ep;
  if (ucNum == UC_OPERAND_READ) {
    if (segno >= UC_BIG_CACHE_SZ)
      return;
    ep = cpu.uCache.operandReadBigCache + segno;
  } else {
    ep = cpu.uCache.caches + ucNum;
  }
  ep->valid = true;
  ep->segno = segno;
  ep->offset = offset;
  ep->bound = bound;
  ep->address = address;
  ep->r1 = r1;
  ep->p = p;
  ep->paged = paged;
#ifdef HDBG
  hdbgNote ("ucache", "save %u %05o:%06o %05o %o %08o %o %o", ucNum, segno, offset, bound, p, address, r1, paged);
#endif
}

bool ucCacheCheck (uint ucNum, word15 segno, word18 offset, word14 * bound, word1 * p, word24 * address, word3 * r1, bool * paged) {
  ucache_t * ep;
  if (ucNum == UC_OPERAND_READ) {
    if (segno >= UC_BIG_CACHE_SZ)
      return false;
    ep = cpu.uCache.operandReadBigCache + segno;
  } else {
    ep = cpu.uCache.caches + ucNum;
  }
  // Is cache entry valid?
  if (! ep->valid) {
    //sim_printf ("!valid\r\n");
#ifdef HDBG
    hdbgNote ("ucache", "check not valid");
#endif
    goto miss;
  }
  // Same segment?
  if (ep->segno != segno) {
    //sim_printf ("segno %o != %o\r\n", ep->segno, segno);
#ifdef HDBG
    hdbgNote ("ucache", "segno %o != %o\r\n", ep->segno, segno);
#endif
    goto miss;
  }
  // Same page?
  if (ep->paged && ((ep->offset & PG18MASK) != (offset & PG18MASK))) {
    //sim_printf ("pgno %o != %o\r\n", (ep->offset & PG18MASK), (offset & PG18MASK));
#ifdef HDBG
    hdbgNote ("ucache", "pgno %o != %o\r\n", (ep->offset & PG18MASK), (offset & PG18MASK));
#endif
    goto miss;
  }
  // In bounds?
  if (((offset >> 4) & 037777) > ep->bound) {
    //sim_printf ("bound %o != %o\r\n", ((offset >> 4) & 037777), ep->bound);
#ifdef HDBG
    hdbgNote ("ucache", "bound %o != %o\r\n", ((offset >> 4) & 037777), ep->bound);
#endif
    goto miss;
  }
#ifdef HDBG
  hdbgNote ("ucache", "hit %u %05o:%06o %05o %o %08o %o %o", ucNum, segno, offset, ep->bound, ep->p, ep->address, ep->r1, ep->paged);
#endif
  * bound = ep->bound;
#if 0
  if (ep->paged) {
    word18 pgoffset = offset & OS18MASK;
    * address = (ep->address & PG24MASK) + pgoffset;
# ifdef HDBG
    hdbgNote ("ucache", "  FAP pgoffset %06o address %08o", pgoffset, * address);
# endif
  } else {
    * address = (ep->address & 077777760) + offset;
# ifdef HDBG
    hdbgNote ("ucache", "  FANP pgoffset %06o address %08o", pgoffset, * address);
# endif
  }
#else
  * address = ep->address;
#endif
  * r1 = ep->r1;
  * p = ep->p;
  * paged = ep->paged;
#ifdef UCACHE_STATS
  cpu.uCache.hits[ucNum] ++;
#endif
  return true;
miss:;
#ifdef UCACHE_STATS
  cpu.uCache.misses[ucNum] ++;
#endif
  return false;
}


