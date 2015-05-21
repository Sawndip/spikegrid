#include "paramheader.h"
#include "mymath.h"
Compute_float* CreatePhiMatrix(void)
{
    Compute_float* PhiMat = calloc(sizeof(Compute_float),grid_size*grid_size);
    Compute_float ThisPhi = 0;
    for (unsigned int x = 0;x<grid_size;x++)
    { 
        for (unsigned int y = 0;y<grid_size;y++)
        {
            PhiMat[x*grid_size+y] = ThisPhi;
            ThisPhi += 2*M_PI/(grid_size*grid_size);
        }
    }
    return PhiMat;
}