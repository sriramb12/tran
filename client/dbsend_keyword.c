#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	dbsend_keyword
 *
 * Description:	Send specified user(s) secure file keyword.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include "global_defines.h"
#include "global_params.h"
#include "list.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "trans_prototypes.h"
#include "trans_prototypes.h"
#include "client_lib.h"
#include "rtn.h"

/* Prompt user for description of data being transferred */
void description_get(char *description)
{
	int cnt = 0; 

    fflush(stdin);
	printf("Enter a one line description for %s data (optional).\n> ", so.program);
	while (((description[cnt] = getchar()) != '\n') 
	&& (description[cnt] != EOF)) ++cnt;
	description[cnt] = '\0';
	/* Put in carriage return for output separation */
	printf("\n");
} /* end description_get */
int dbsend_keyword(
    char *secure_key,
    char *file_key,
#ifdef TRANSCEND_GUI
    char *description,
#endif
    int argc,
    char **argv)
{
    char email_string[BUFFER_SIZE];
#ifndef TRANSCEND_GUI
    char description[160];
#endif
    short ret;
    int email_cnt;

    /* Verify valid keyword passed in */
    if ((ret = is_keyword(DBSEND_KEYWORD, secure_key)) < 0) return(ret);
    /* Connect to the Transcend database */
    if ((ret = init_trans_socket(DBSEND_KEYWORD, secure_key, NONE, NONE, email_cnt,  0,
    UNSPECIFIED, file_key, NONE, NONE, NONE, NONE, NONE)) != SUCCESS)
    return(ret);
#ifndef TRANSCEND_GUI
    description_get(description);
#endif
	put_string(description);
    /* Send email address to server in string format */
	static_list_string_convert(argv, email_string);
    put_string(email_string);
    return(get_ret_code());
} /* end dbsend_keyword */
#endif /* For not TRANSAPI */
