/****************************************************************************
 *
 * Function:	password_name_check
 *
 * Description:	Puts name from password file in more readable form
 *
 ***************************************************************************/

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "strcpy.h"
#include "trans_defines.h"
#include "trans_prototypes.h"

#include "global_debug.h" // Mazieri's debug facilities

void password_name_check(char *user_name)
{
    DBG("char* username before '%s'", user_name);

    short len;
    char buffer[64];
    char *ptr;

    /* If no name was found, put in Unknown and return */
    if (user_name[0] == '\0')
    {
	    strcpy(user_name, "Unknown name");
    }

    /* Take extra characters out of the name */
    /* These usually follow a comma in the user's name */
    if ((ptr = strchr(user_name, ',')) != NULL)
        ptr[0] = '\0';

        /* Make sure there is a first and last name */
        if ((ptr = strchr(user_name, ' ')) == NULL)
        {
	        /* Check for a hyphen */
	        /* Sometimes Japanese names will be combined with a hyphen */
	        ptr = strchr(user_name, '-');
	        if (ptr != NULL)
	        {
	            /* Split the name in half */
	            ptr[0] = '\0';

	            /* Copy the first part of the name to a temporary buffer */
	            strcpy(buffer, user_name);

	            /* Copy the second part of the name into the first name */
	            len = strcpy1(user_name, ptr+1);

	            /* Now put the rest of the name back into its proper location */
	            strcpy2(user_name+len, " ", buffer);
	        } /* End if for hyphen found */
            /* Otherwise add a second name */
            else
                strcpy1(user_name+strlen(user_name), " name");
        } /* End if for only one name present */

    DBG("char* username after '%s'", user_name);
} /* End password_name_check */


/* Allows passing in a directory name and alternate file name */
void alternate_file_name_bld(char *full_file_name, char *file_name, char *file_dir, char *alternate_file)
{
    DBG();

    if (file_dir != NULL)
	{
    if (alternate_file != NULL)
	    strcpy3(full_file_name, file_dir, "/", alternate_file);
	    else strcpy3(full_file_name, file_dir, "/", file_name);
    }
    else strcpy(full_file_name, file_name);
} /* end alternate_file_name_bld */

/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
