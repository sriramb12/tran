/****************************************************************************
 *
 * Function:	init_user_globals
 *
 * Description:	Initializes information database needs about user
 *		Data initialized includes: IP node address, 
 *		machine name, login, user name, machine type,
 *		home directory, and current working directory.
 *		Client version is not initialized in this function
 *		because more than one client uses this function.
 *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "strcpy.h"
#include "trans_prototypes.h"
#include <string.h>
#ifdef WINDOWS_NT4
#include <Windows.h>
#endif

#include "global_defines.h"
#include "global_params.h"
#include "global_client_errs.h"
#include "server_params.h"
#include "user_params.h"
#include "global_debug.h" // Mazieri's debug facilities

/* Holds server name, address, and socket */
extern SERVER_PARAMS server;

#ifndef WINDOWS_NT4
#include <pwd.h>
#endif

/* The following function is used only on unix workstations */
#ifndef WINDOWS_NT4
void password_name_check(char *user_name)
{
    short len;
    char buffer[64];
    char *ptr;

    DBG("");

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
    else strcpy1(user_name+strlen(user_name), " name");
    } /* End if for only one name present */
} /* End password_name_check */
#endif

#ifndef TRANSAPI
/* Sets up values for global variables */
void user_params_init(void)
{
    DBG();

    extern USER_PARAMS user;
    struct passwd *pw_entry;
    unsigned long uid;

#ifdef WINDOWS_NT4
    DWORD nameLen;
#endif

    /* Indicate network password encryption not specified */
    /* Overwrite any file creation mode user has set */
    umask(0);

    /* Get the user's machine name */
    gethostname(user.mach, sizeof(user.mach) -1);

    /* Get the user-s login name */
    /* Do not overwrite the login name if this is the GUI */
#ifdef WINDOWS_NT4
    nameLen = sizeof(user.login);
    if (!GetUserName(user.login,&nameLen))
    {
        strcpy(user.login, "unknown");
        strcpy(user.name, "Unknown Name");
    }
    else
    {
	    strcpy(user.name, user.login);
	    strexc(user.login, ' ', '-');
    }
#else
    /* Get the User ID */
    uid = getuid();

    /* Open the password file */
    setpwent();

    /* Retrieve data from the password file */
    pw_entry = getpwuid(uid);
    if (pw_entry != NULL)
    {
	    /* Copy the user's login name into the user_id string */
	    strcpy(user.login, pw_entry->pw_name);

	    /* Initialize the home directory */
	    strcpy(user.home_dir, pw_entry->pw_dir);

	    /* Copy the user's name into correct field */
	    strcpy(user.name, pw_entry->pw_gecos);

	    /* Check for spaces and commas in the user's name */
	    password_name_check(user.name);
    } /* End if */
    else
    {
	    strcpy(user.login, "unknown");
	    strcpy(user.name, "Unknown Name");
    }

    /* Close the password file */
    endpwent();
#endif
    
    /* Get the IP node address */
    ip_find(user.addr, user.mach);

#ifdef WINDOWS_NT4
    /* Exchange any spaces in machine name for '_' */
    strexc(user.mach, ' ', '_');
#endif

    /* Initialize the current working directory */
    getcwd(user.cwd, PATH_SIZE);

    /* If no home directory present, make current directory home directory */
    if (user.home_dir[0] == '\0') strcpy(user.home_dir, user.cwd);

    strcpy(user.machine_type, MACHINE_TYPE);
} /* End user_params_init */
#endif /* For not TRANSAPI */
