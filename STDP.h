/// \file
#include "layer.h"
typedef struct STDP_parameters STDP_parameters;
typedef struct STDP_data
{
    lagstorage lags;
    Compute_float* connections;
} STDP_data;

void  DoSTDP(const Compute_float* const const_couples, const Compute_float* const const_couples2,STDP_data* data,const STDP_parameters S,STDP_data* const data2,const STDP_parameters S2);
STDP_data* STDP_init(const STDP_parameters S,const int trefrac_in_ts);
