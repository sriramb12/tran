/****************************************************************************
 *
 * Function:	strcmp1err
 *
 * Description: Checks to see if names match except two transposed characters.
 *		If a match is found, this function returns SUCCESS (0).
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <string.h>
#include "reg_globals.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "common_names.h"

#include "global_debug.h" // Mazieri's debug facilities

int strcmp1err(char *name1, char *name2)
{
    int mismatch = 1;
    int mismatch1 = 0;
    int mismatch2 = 0;
    register int cnt;
    int len;
    int len2;

    DBG("name1='%s' and name2='%s'", name1, name2);
    
    /* Get the length of the two names */
    len = strlen(name1);
    len2 = strlen(name2);
    if (len2 < len) len = len2;

    /* If less than 8 characters, than abort */
    if (len < 8) return(FAILURE);

     /* Compare characters in name in a for loop */
    for (cnt = 0; cnt < len; ++cnt)
    {
	    /* If mismatch characters found, do further checking */
        if (name1[cnt] != name2[cnt])
	    {
	        /* If this is the first mismatch, point to the position in string */
	        if (mismatch == 1) mismatch1 = cnt;
	        /* If this is the second mismatch, point to the second position in the string */
	        else if (mismatch == 2) mismatch2 = cnt;
	        /* If two mismatches already found, return failure */
	        else if (mismatch > 2) return(FAILURE);

	        ++mismatch;
	    } /* End if for mismatch found */
    } /* End for */

    /* If no mismatches found, return success */
    if (mismatch == 0) return(SUCCESS);

    /* If there was only one mismatch and the string is at least 3 characters in length, return success */
    if ((mismatch == 1) && (len >= 3)) return(SUCCESS);

    /* If the mismatches are transposed characters, return success */
    if (((mismatch1+1) == mismatch2) && (name1[mismatch1] == name2[mismatch2]) && (name1[mismatch2] == name2[mismatch1]))
        return(SUCCESS);
    else 
        return(FAILURE);
} /* End strcmp1err */
