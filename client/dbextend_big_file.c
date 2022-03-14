#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	db_extend_big_file
 *
 * Extends expiration date of big file
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "global_defines.h"
#include "global_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include <stdlib.h>
#include "trans_prototypes.h"
#include "client_lib.h"

int extend_big_file(char *key, int days)
{
    short ret;

    if ((ret = is_keyword(DBEXTEND_DATA, key)) < 0) return(ret);
    if ((ret = init_trans_socket(DBEXTEND_DATA, key, NONE, NONE, days, 0, 
UNSPECIFIED, NONE_STRING)) < SUCCESS) return(ret);
    return(get_ret_code());
} /* End extend_big_file */

int dbextend_big_file(int argc, char **argv)
{
    int days;

    if (argc < 4)
    {
	printf("You may extend the expiration date up to 3 days. \n");
	printf("Enter the number of days to extend the expiration.  ");
	scanf("%d", &days);
	printf("\n");
    } /* End if for days not entered on command line */
    else days = atoi(argv[3]);
    if ((days < 1) || (days > 3))
    {
	printf("You cannot extend the expiration date more than three days.\n");
	printf("Enter 1, 2, or 3 to extend the expiration date.  Enter\n");
	printf("anything else to exit %s.\n> ", so.program);
	scanf("%d", &days);
	printf("\n");
	if ((days < 1) || (days > 3))
	{
	    return(-1);
	} /* End if for exiting Transcend */
    } /* End if for days greater than 3 */
    return(extend_big_file(argv[2], days));
} /* End dbextend_big_file */

#endif /* For not TRANSAPI */
