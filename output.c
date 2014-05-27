/// \file
#include <string.h>
#include "output.h"
#include "picture.h"

///Extracts the actual information out of a tagged array and converts it to a simple square matrix
Compute_float* taggedarrayTocomputearray(const tagged_array input)
{
    const unsigned int size = input.size - (2*input.offset);
    Compute_float * ret = calloc(sizeof(*ret),size*size);
    for (unsigned int i=0;i<size;i++)
    {
        for (unsigned int j=0;j<size;j++)
        {
            const Compute_float val =  input.data[(i+input.offset)*input.size + j + input.offset ];
            ret[i*size+j]=val;
        }
    }
    return ret;
}

///simple function to convert comp_float 2d array to a bitmap that you can then do something with (like save)
bitmap_t* FloattoBitmap(const Compute_float* const input,const unsigned int size,const Compute_float minval, const Compute_float maxval)
{
    bitmap_t* bp = (bitmap_t*)malloc(sizeof(bitmap_t));
    bp->pixels = malloc(sizeof(pixel_t)*size*size);
    bp->width=size;
    bp->height=size;

    for (unsigned int i=0;i<size;i++)
    {
        for (unsigned int j=0;j<size;j++)
        {
            Compute_float val =input[i*size+j];

            // Cap values which fall outside the specified range
            if (val < minval)
                val = minval;
            if (val > maxval)
                val = maxval;

            // Total range of values
            const Compute_float dval =  maxval-minval;

            // Scaled value (between 0 and 1)
            const Compute_float sval = (val - minval)/dval;

            // This generates the jet MATLAB colormap. We multiply by 255 because the map distinguishes colors over a range of 0 to 255 integer values inclusive (see http://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale for a full explanation)
            if (val < (minval + 0.125*dval))
            {
                bp->pixels[i*size+j].red = 0;
                bp->pixels[i*size+j].green = 0;
                bp->pixels[i*size+j].blue = (uint8_t)(255*(4*sval + 0.5));
            }
            else if (val < (minval + 0.375*dval))
            {
                bp->pixels[i*size+j].red = 0;
                bp->pixels[i*size+j].green = (uint8_t)(255*(4*sval - 0.5));
                bp->pixels[i*size+j].blue = 255; 
            }
            else if (val < (minval + 0.625*dval))
            {
                bp->pixels[i*size+j].red = (uint8_t)(255*(4*sval - 1.5));
                bp->pixels[i*size+j].green = 255;
                bp->pixels[i*size+j].blue = (uint8_t)(255*(-4*sval + 2.5));
            }
            else if (val < (minval + 0.875*dval))
            {
                bp->pixels[i*size+j].red = 255;
                bp->pixels[i*size+j].green = (uint8_t)(255*(-4*sval + 3.5));
                bp->pixels[i*size+j].blue = 0;
            }
            else
            {
                bp->pixels[i*size+j].red = (uint8_t)(255*(-4*sval + 4.5));
                bp->pixels[i*size+j].green = 0;
                bp->pixels[i*size+j].blue = 0;
            }
        }
    }
    return bp;
}

#ifdef MATLAB
mxClassID __attribute__((pure,const)) MatlabDataType()
{
    return sizeof(Compute_float)==sizeof(float)?mxSINGLE_CLASS:mxDOUBLE_CLASS;
}
//When using matlab, we want to be able to output just about any array of stuff.  This function does the work
mxArray* outputToMxArray (const tagged_array input) 
{
    const unsigned int size = input.size - (2*input.offset);
    mxArray* ret = mxCreateNumericMatrix((int)size,(int)size,MatlabDataType(),mxREAL); //matlab has signed ints for array sizes - really?
    Compute_float* dataptr =  (Compute_float*)mxGetData(ret);
    Compute_float* actualdata = taggedarrayTocomputearray(input);
    memcpy(dataptr,actualdata,sizeof(Compute_float)*size*size);
    return ret;
}
#endif
/// number of PNG's outputted.  Used to keep track of the next filename to use
int printcount=0;
///convert a tagged array to a PNG.  Paths are auto-calculated
void OutputToPng(const tagged_array input,const Compute_float minval,const Compute_float maxval)
{
    char fnamebuffer[30];
    const unsigned int size = input.size - (2*input.offset);
    Compute_float* actualdata=taggedarrayTocomputearray(input);
    bitmap_t* b = FloattoBitmap(actualdata,size,minval,maxval);
    sprintf(fnamebuffer,"pics/%i.png",printcount);
    printcount++;
    save_png_to_file(b,fnamebuffer);
    free(b->pixels);
    free(b);
    free(actualdata);
    
}
///High level function to create a series of PNG images which can be then turned into a movie
void makemovie(const layer l,const unsigned int t)
{
    if (l.P->Movie.MakeMovie==ON && t % l.P->Movie.Delay==0)
    {
        OutputToPng(Outputtable[l.P->Movie.Output].data,Outputtable[l.P->Movie.Output].minval,Outputtable[l.P->Movie.Output].maxval);
    }
}
