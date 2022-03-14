/****************************************************************************
 *
 * Function:	strdlt_quotes
 *
 * Description: Deletes leading quotes from a string
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include "global_debug.h" // Mazieri's debug facilities

void strdlt_quotes(char *string)
{
    register short cnt;

    DBG("string='%s'", string);

    if ((string[0] == '\'') || (string[0] == '"'))
    {
	    for (cnt = 1; string[cnt] != '\0'; ++cnt)
	        string[cnt-1] = string[cnt];

	    /* Put null at end of new string */
	    string[cnt-1] = '\0';
    } /* End if */
} /* End strdlt_quotes */
