/****************************************************************************
 *
 * Function:	get_random_index
 *
 * Description:	Returns a random index (0-9)
 *
 ***************************************************************************/

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "global_debug.h"


int get_random_index(void)
{	
	struct timeval time; 
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
	
	int index = rand() % 10;
	DBG("index=%d", index);
	
	return(index);
    
} /* End get_random_index */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
