//This file uses a very large amount of ram.  Useful for stress testing a PC
#warning "using canonical parameters"
#include <stddef.h> //offsetof
//these first few parameters actually escape into the paramheader file through magic
#define grid_size 80
///Total size of the grid
///Coupling range
#define couplerange 15
#ifndef PARAMETERS  //DO NOT REMOVE
///include guard
#define PARAMETERS  //DO NOT REMOVE
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#endif
//the following typedef must be before the include to get the right compute types
//#include "../paramheader.h"
///Whether we are using the single or double layer model
static const LayerNumbers ModelType = DUALLAYER;

//Fun note - with the right optimisations GCC actually will pull these constants inline (for example disassemble evolvept_STDP with STDP off)
///Parameters for the single layer model
static const parameters OneLayerModel =
{
    .couple =
    {
        .Layertype = SINGLELAYER,
        .Layer_parameters =
        {
            .single =
            {
                .WE     = 0.41,                 //excitatory coupling strength
                .sigE   = 14,                   //char. length for Ex symapses (int / float?)
                .WI     = 0.24,                 //Inhib coupling strength
                .sigI   = 42,                   //char. length for In synapses (int / float?)
                .Ex = {.R=0.5,.D=2.0},          //excitatory rise / decay time
                .In = {.R=0.5,.D=2.0},          //inhibitory rise / decay time
            }
        },
        .tref   = 5,
        .norm_type = None,
    },
    .potential =
    {
        .type    =
        {
            .type = LIF,
            .extra =
            {
                .EIF={.Vth=-55,.Dpk=1}
            }
        },
        .Vrt     = -70,                  //reset potential
        .Vpk    = -55,                   //peak potential (at which membrane potential is reset -- must match Vth for LIF neurons)
        .Vlk     = -70,                  //leak reversal potential
        .Vex     = 0,                    //Ex reversal potential
        .Vin     = -80,                  //In reversal potential
        .glk     = 0.05,                 //leak current
        .rate = 0,
    },
    .skip=1,
};
///parameters for the inhibitory layer of the double layer model
static const parameters DualLayerModelIn =
{
    .couple =
    {
        .Layertype = DUALLAYER,
        .Layer_parameters =
        {
            .dual =
            {
                .connectivity = HOMOGENEOUS,   //EXPONENTIAL or HOMOGENEOUS
                .W            = -0.30,
                .sigma        = 60,
                .synapse      = {.R=0.5,.D=7.0},
            }
        },
        .norm_type = None,
        .tref       = 5,
    },
    .potential =
    {
        .type    =
        {
            .type = LIF,
            .extra =
            {
                .EIF={.Vth=-55,.Dpk=1}
            }
        },
        .Vrt     = -70,                  //reset potential
        .Vpk    = -55,                   //peak potential (at which membrane potential is reset -- must match Vth for LIF neurons)
        .Vlk     = -70,                  //leak reversal potential
        .Vex     = 0,                    //Ex reversal potential
        .Vin     = -80,                  //In reversal potential
        .glk     = 0.05,                 //leak current
        .rate = 0,
    },
    .skip = 2,
};
///parameters for the excitatory layer of the double layer model
static const parameters DualLayerModelEx =
{
    .couple =
    {
        .Layertype = DUALLAYER,
        .Layer_parameters =
        {
            .dual =
            {
                .connectivity = EXPONENTIAL,
                .W            = 0.23,
                .sigma        = 12,
                .synapse      = {.R=0.5,.D=2.0},
            }
        },
        .tref       = 5,
        .norm_type = None,
    },
    .potential =
    {
        .type    =
        {
            .type = LIF,
            .extra =
            {
                .EIF={.Vth=-55,.Dpk=1}
            }
        },
        .Vrt     = -70,                  //reset potential
        .Vpk    = -55,                   //peak potential (at which membrane potential is reset -- must match Vth for LIF neurons)
        .Vlk     = -70,                  //leak reversal potential
        .Vex     = 0,                    //Ex reversal potential
        .Vin     = -80,                  //In reversal potential
        .glk     = 0.05,                 //leak current
        .rate = 0,
    },
    .skip = 1,

};
///Constant external input to conductances
static const extinput Extinput =
{
    .gE0 = 0.015,
    .gI0 = 0,
};
///Some global features that can be turned on and off
static const model_features Features =
{
    .Recovery   = OFF,
    .STDP		= OFF, //Question - some of these do actually make more sense as a per-layer feature - just about everything that isn't the timestep -
    .STD        = OFF,  //if we need any of these features we can make the changes then.
    .Theta      = OFF,
    .Timestep   = 0.1, // Works in like with 0.1 for midpoint. But if gE too small should addition be smaller too???
    .Simlength  = 2e3,
    .job=
    {
        .initcond=SINGLE_SPIKE,
        .Voltage_or_count = -50, //superthreshold
        .next =
        &(Job){
            .initcond=SINGLE_SPIKE,
            .Voltage_or_count=-55.1, //subthreshold
            .next =
            &(Job){
                .initcond=RAND_JOB,  //random - run a few times to be sure
                .Voltage_or_count = 4,
            }
        }
    },
    .output=
    {
        {
            .method=PICTURE,
            .Output=0, //gE
            .Delay=5
        },
        {
            .method=PICTURE,
            .Output=1,//gI
            .Delay=5
        },
        {
            .method=TEXT,
            .Output=2,//coupling1
            .Delay=1000
        },
        {
            .method=TEXT,
            .Output=3,//coupling2
            .Delay=1000
        },
        {
            .method=PICTURE,
            .Output=4,//V1
            .Delay=5
        },
        {
            .method=PICTURE,
            .Output=5,//V2
            .Delay=5
        },
    }
};
///Parameters for conducting a parameter sweep.
static const sweepable Sweep =
{
    .offset=offsetof(parameters,couple)+offsetof(couple_parameters,Layer_parameters) +0+ /*offset in the union is always 0*/  + offsetof(duallayer_parameters,W),
    .minval = 0.01,
    .maxval = 1,
    .count = 10
};

#ifdef __clang__
#pragma clang diagnostic pop
#else
#pragma GCC diagnostic pop
#endif
#endif //DO NOT REMOVE
