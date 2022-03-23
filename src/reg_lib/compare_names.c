/****************************************************************************
 *
 * Function:	compare_names
 *
 * Description: Compares names to see if match is found.
 *		First copies names passed in to local variables.
 *		Converts all names to lower case.
 *		Strips off leading quotes and makes lengths the same.
 *
 * Author:	Renee Carter
 *
 * Parameters:
 *  char *servername1
 *  char *servername2
 *  char *username1
 *  char *username2
 *  char *username3
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "global_reg_prototypes.h"
#include "reg_globals.h"
#include "global_defines.h"
#include "common_names.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

int compare_names(char *servername1, char *servername2, char *username1, char *username2, char *username3)
{
    int server_len;
    int user_len;
    int len;
    register int cnt;
    char *ptr;
    char server_name1[32];
    char server_name2[32];
    char user_name1[32];
    char user_name2[32];
    char user_name3[32];

    DBG("servername1='%s', servername2='%s', username1='%s', username2='%s', username3='%s'", servername1, servername2, username1, username2, username3);

    /* Copy all names to local variables */
    strcpy(server_name1, servername1);
    strcpy(server_name2, servername2);
    strcpy(user_name1, username1);
    strcpy(user_name2, username2);
    strcpy(user_name3, username3);

    /* Put all names in lower case */
    str_lower(server_name1);
    str_lower(server_name2);
    str_lower(user_name1);
    str_lower(user_name2);
    str_lower(user_name3);

	/* Make sure the first name matches */
	/* Get length of first name just in case there are trailing */
	/* characters on the name from the password file */
	server_len = strlen(server_name1);
	user_len = strlen(user_name1);

    /* Make total length of name the least of the two lengths */
    if (user_len < server_len) len = user_len;
    else len = server_len;

    /* If either length less than 3, just use the server length */
    if (len < 3) len = server_len;

    /* If there is only one name, see if first name is actually two */
    /* names separated by a '_' */
    if (user_name2[0] == '\0')
    {
	    for (cnt = 0; user_name1[cnt] != '\0'; ++cnt)
	    {
	        if (user_name1[cnt] == '_')
	        {
	    	    user_name1[cnt] = '\0';
	    	    strcpy(user_name2, user_name1+cnt+1);
	    	    break;
	        } /* End if for '_' found */
	    } /* End for statement */
	
	    /* If the '_' not found, return failure */
	    if (user_name2[0] == '\0') return(DBNO);
    } /* End if */

	if (strncmp(user_name1, server_name1, len) != 0)
	{
	    /* Delete any leading quotes */
	    strdlt_quotes(user_name1);

	    /* If the names still do not match, */
	    /* check for common and formal names */
	    if (strncmp(user_name1, server_name1, len) != 0)
	    {
		    /* Go through the whole list of common and formal names */
		    for (cnt = 0; cnt < TOTAL_NAMES; ++cnt)
		    {
		        if (((strcmp(user_name1, common_names[cnt]) == 0) && (strcmp(server_name1, formal_names[cnt]) == 0))
		            || ((strcmp(user_name1, formal_names[cnt]) == 0) && (strcmp(server_name1, common_names[cnt]) == 0)))
		            break;
		    } /* End for */

		    /* If name still not found, check for misspelling */
		    if (cnt == TOTAL_NAMES) 
		    {
		        if (strcmp1err(user_name1, server_name1) != SUCCESS)
		        {
			        /* If name still not found, check for initials */
			        if ((user_name1[1] != '.') || (user_name1[0] != server_name1[0]))
			            return(DBNO);
		        } /* End if for checking for initial */
		    } /* End if for checking for misspelling */
	    } /* End if for match not found for first name */
	} /* End if for match not found immediately */

	/* Make sure last name is the same */
	/* check second and third string just in case */
	/* user has a middle name or initial */
	/* Get length of second name returned from whois command */
	/* just in case there are trailing characters on the name */
	/* from the password file */
	len = strlen(server_name2);
	if ((strncmp(server_name2, user_name2, len) != 0) && (strncmp(server_name2, user_name3, len) != 0))
	{
	    /* Delete any leading quotes */
	    strdlt_quotes(user_name2);

	    /* Now do the same thing on the third name */
	    /* Only do it if a name is present */
	    if (strlen(user_name3) > 0)
	    {
	        /* Delete any leading quotes */
	        strdlt_quotes(user_name3);
	    } /* End if for length of third name greater than 0 */

	    /* Take dash and trailing characters off of name from user or whois command */
	    ptr = strchr(server_name2, '-');
	    if (ptr != NULL)
	    {
		    ptr[0] = '\0';
		    len = strlen(server_name2);
	    } /* End if for trailing - with characters found */

	    /* Check the names one more time */
	    if ((strncmp(server_name2, user_name2, len) != 0) && (strncmp(server_name2, user_name3, len) != 0))
	    {
	        /* Check for misspellings */
		    if ((strcmp1err(user_name2, server_name2) == SUCCESS) || (strcmp1err(user_name3, server_name2) == SUCCESS))
	            return(DBYES);
		    else return(DBNO);
	    } /* End if for one last check of names */
	} /* End if for match of second name not found immediately */

	return(DBYES);
} /* End compare_names */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
