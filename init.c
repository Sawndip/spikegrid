#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "coupling.h"
#include "STD.h"
static layer_t glayer;
//creates a random initial condition - small fluctuations away from Vrt
void randinit(Compute_float* input)
{
    srandom((unsigned)(time(0)));
    for (int x=0;x<grid_size;x++)
    {
        for (int y=0;y<grid_size;y++)
        {
            input[x*grid_size + y ] = ((Compute_float)random())/((Compute_float)RAND_MAX)/((Compute_float)20.0) + Param.potential.Vrt;
        }
    }
}
//The spikes that we emit have a time course.  This function calculates the timecourse and returns an array of cached values to avoid recalculating at every timestep
Compute_float* __attribute__((const)) Synapse_timecourse_cache (const unsigned int cap, const decay_parameters Decay,const time_parameters t)
{
    Compute_float* ret = calloc(sizeof(Compute_float),cap);
    for (unsigned int i=0;i<cap;i++)
    {
        const Compute_float time = ((Compute_float)i)*t.dt;
        ret[i]=Synapse_timecourse(Decay,time); 
    }
    return ret;
}

//some tests that the setcap function is correct.
//TODO: add more tests
void setcaptests()
{   
    decay_parameters t1 = {.D=1.5,.R=0.5};
    decay_parameters t2 = {.D=2.0,.R=0.5};
    //todo:Get Adam to check these values - also add more tests
    assert (setcap(t1,(Compute_float)1E-6,Param.time.dt)==209);
    assert (setcap(t2,(Compute_float)1E-6,Param.time.dt)==270);
}

//given a parameters object, set up a layer object.
//This function is what theoretically will allow for sweeping through parameter space.
//The only problem is that a parameters object is immutable, so we need some way to do essentially a copy+update in C.
//Essentially, we need to be able to do something like P = {p with A=B} (F# record syntax)
//currently this function is only called from the setup function (but it could be called directly)
layer_t setuplayer(const parameters p)
{
    const Compute_float min_effect = (Compute_float)1E-6;
    const unsigned int cap = max(setcap(p.synapse.Ex,min_effect,Param.time.dt),setcap(p.synapse.In,min_effect,Param.time.dt));
    layer_t layer = 
        {
            .spikes=
            {   
                .count=cap,
                .data=calloc(sizeof(coords*), cap)
            },
            .connections = CreateCouplingMatrix(p.couple),
            .STDP_connections = p.features.STDP==ON?calloc(sizeof(Compute_float),grid_size*grid_size*couple_array_size*couple_array_size):NULL,
            .std                = STD_init(&p.STD), //this is so fast that it doesn't matter to run in init
            .Extimecourse       = Synapse_timecourse_cache(cap,p.synapse.Ex,p.time),
            .Intimecourse       = Synapse_timecourse_cache(cap,p.synapse.In,p.time),
            .P                  = &(p.potential),
            .S                  = &(p.STDP)
        };

    memset(layer.voltages,0,grid_size*grid_size); //probably not required
    memset(layer.voltages_out,0,grid_size*grid_size);//probably not required
    for (unsigned int i=0;i<cap;i++)
    {
        layer.spikes.data[i]=calloc(sizeof(coords),(grid_size*grid_size + 1));//assume worst case - all neurons firing.  Need to leave spae on the end for the -1 which marks the end.
        layer.spikes.data[i][0].x=-1;//need to make sure that we don't start with spikes by ending at 0
    }
    return layer;
}
//The idea here is that "one-off" setup occurs here, whilst per-layer setup occurs in setuplayer
void setup(const parameters p)
{
    char* buffer = malloc(1024);
    gethostname(buffer,1023);
    if (!strcmp(buffer,"headnode.physics.usyd.edu.au")) {printf("DON'T RUN THIS CODE ON HEADNODE\n");exit(EXIT_FAILURE);}
    glayer = setuplayer(p);
}