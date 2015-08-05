/// \file
#ifndef SIZES
#define SIZES
#ifdef _WIN32
#include "VS\VS\Winheader.h"
#endif
#define PARAMETERS
#include "whichparam.h"
#undef  PARAMETERS
//get some macros for various sizes
///Size of the "large" arrays (notable examples are gE and gI)
#define conductance_array_size (grid_size + 2*couplerange)
///Size of a coupling matrix
#define couple_array_size (2*couplerange + 1)

///Use this macro to change how far we apply STDP.
#define STDP_RANGE couplerange
#define STDP_RANGE_SQUARED (STDP_RANGE*STDP_RANGE)
#define STDP_array_size (2*STDP_RANGE  + 1)
#include "typedefs.h"
static inline size_t Conductance_index(const coords in) {return (size_t)((in.x+couplerange)*conductance_array_size)+in.y+couplerange;}
static inline size_t grid_index(const coords in) {return (size_t)(in.x*grid_size) + in.y;}
#endif
