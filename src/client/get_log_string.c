#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	get_log_string
 *
 * Description:	Retrieves a string and logs it in the trans.log or icom.log file.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "global_defines.h"
#include "global_params.h"
#include "ssl_client_lib.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "transrc.h"
#include "rtn.h"
#include "strcpy.h"
#include "transapi_defines.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "trans_prototypes.h"
#include "client_lib.h"
#include "trans_defines.h"

#include "global_debug.h" // Mazieri's debug facilities

/* Gets return code, return message, warning flag & warning messages from DB */
short get_log_string(void)
{
    char file_name[FILE_SIZE];
    short ret;
    FILE *fp;
    char log_string[1024];

    DBG();

    /* Get the return code from the server */
    ret = get_ssl_short();

    /* Get the return string from the server */
    get_ssl_string(rtn.msg);

    /* Get the string for the log file */
    if (ret == SUCCESS)
    {
        DBG("got SUCCESS");
        if (get_ssl_string(log_string))
        {
            DBG("got from get_ssl_string()=%s transrc.logDir=%s", log_string, transrc.logDir);
            //PRINTF("%s\n", log_string);

            /* If set for logging log the message */
            if (transrc.logDir[0] != '\0')
            {
                /* Build name for log file */
                DBG("TRANS_LOG='%s'", TRANS_LOG);
                strcpy2(file_name, transrc.logDir, TRANS_LOG);
                DBG("File log name will be file_name='%s'", file_name);

                if ((fp = fopen(file_name, "a")) != NULL)
                {
                    fprintf(fp, "%s\n", log_string);
                    fclose(fp);
                    /* Change mode to read/write for user only */
                    chmod(file_name, 0660);
                } /* End if for successful open of file */
            } /* End if for wanting to log the string */
        } /* End if for got the string */
    } /* End if for successful return code */

    DBG("will return");
    return(ret);
} /* End get_log_string */
#endif
#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	get_log_string
 *
 * Description:	Retrieves a string and logs it in the trans.log or icom.log file.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "global_defines.h"
#include "global_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "transrc.h"
#include "rtn.h"
#include "strcpy.h"
#include "transapi_defines.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "trans_prototypes.h"
#include "client_lib.h"
#include "trans_defines.h"

#include "global_debug.h" // Mazieri's debug facilities

/* Gets return code, return message, warning flag & warning messages from DB */
short get_log_string(void)
{
    DBG();

    char file_name[FILE_SIZE];
    short ret;
    FILE *fp;
    char log_string[4096];


    /* Get the return code from the server */
    ret = get_short();

    /* Get the return string from the server */
    get_string(rtn.msg);

    /* Get the string for the log file */
    if (ret == SUCCESS)
    {
	    if (get_string(log_string))
	    {
	        /* If set for logging log the message */
	        if (transrc.logDir[0] != '\0')
	        {
	    	    /* Build name for log file */
	    	    strcpy2(file_name, transrc.logDir, TRANS_LOG);

	    	    if ((fp = fopen(file_name, "a")) != NULL)
	    	    {
	    	        fprintf(fp, "%s\n", log_string);
	    	        fclose(fp);

	    	        /* Change mode to read/write for user only */
	    	        chmod(file_name, 0660);
	    	    } /* End if for successful open of file */
	        } /* End if for wanting to log the string */
	    } /* End if for got the string */
    } /* End if for successful return code */

    return(ret);
} /* End get_log_string */
#endif
