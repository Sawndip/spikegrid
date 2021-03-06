/// \file
#ifndef LAGSTORAGE
#define LAGSTORAGE
#include "sizes.h"
/// TODO: Much of the time in the code appears to be in bracnch mispredicts caused by while loops in lagstorage
/// One solution would be to also add a count of the spikes in here - then we can count down to 0
/// it would also make adding more new spikes simpler.
/// There is a small memory cost so do some benchmarking
typedef struct lagstorage //TODO question - could this be moved out of .h?
{
    int16_t*    lags; //since the maximum time delay should be short, don't bother with a full int, using int16 will halve memory usage.  In the no-STDP case, lagstorage is a pretty significant amount of RAM usage.  Note - with small lags we could even go to int8_t
    uint16_t* counts;
    const int   cap;
    const unsigned int   lagsperpoint;
} lagstorage;
lagstorage* lagstorage_init(const unsigned int flagcount,const int cap);
void lagstorage_dtor(lagstorage* l);
void AddnewSpike(lagstorage* L,const size_t baseidx);
void modifyLags(lagstorage* L,const size_t baseidx,const size_t realidx);
int16_t __attribute__((const,pure,used)) CurrentShortestLag(const lagstorage* const L,const size_t baseidx,const size_t realbase);
///A small helper function to calculate the base idx for the lags at a given x and y coordinate.  Mostly just for convenience
static inline size_t __attribute__((const,pure)) LagIdx(const coords c ,const lagstorage* L) {return (grid_index(c))*L->lagsperpoint;}
#endif
