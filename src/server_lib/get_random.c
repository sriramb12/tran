/****************************************************************************
 *
 * Name:	encrypt_file
 *
 * Description:	Encrypt a file 
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/


#include "global_defines.h"
#include "global_params.h"
#include "global_client_errs.h"
#include <stdio.h>
#include "user_params.h"
#include "global_debug.h" // Mazieri's debug facilities


void encrypt_file(char *file1, char *file2)
{
    DBG("file1='%s' and file2='%s'", file1, file2);
    
    FILE *fopen(), *fi, *fo;
    char  c;
    int iran,iseed;
    float get_random();

    fi = fopen(file1, "r");
    if (fi == NULL)
    {
        return;
    }

    fo = fopen(file2, "w");
    if (fo == NULL)
    {
        return;
    }

   iseed = 318057462;

    /* Encrypt the data one character at a time. */
   while ((c = getc(fi)) != EOF)
   {
      iran = 255.0 * get_random(&iseed); 
      putc(c^iran, fo);
   }
}


/* This routine adapted from page 227 of Simulation and */
/* Modeling Analysis, Law and Kelton, 1982.             */
float get_random(int *ix)
{  
    DBG("ix='%d'", ix);

    float rand;
    int xhi,xalo,leftlo,fhi,k;
    int a   = 16807;
    int b15 = 32768;
    int b16 = 65536;
    int p   = 2147483647;

    xhi = *ix/b16;
    xalo = (*ix-xhi*b16)*a;
    leftlo = xalo/b16;
    fhi = xhi*a+leftlo;
    k = fhi/b15;
    *ix = (((xalo-leftlo*b16)-p)+(fhi-k*b15)*b16)+k;
    
    if( *ix < 0 ) *ix = *ix + p;

    rand = *ix * 4.656612875e-10;

    return(rand);
}
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
