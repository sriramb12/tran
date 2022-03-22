#ifndef TRANSAPI
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "global_defines.h"
#include "password.h"
#include "rtn.h"
#include "transapi_defines.h"

#include "global_debug.h" // Mazieri's debug facilities


/* Encrypt user's PIN or password */
/* Encrypted password placed in new_pwd */
int password_crypt(char *new_pwd, char *old_pwd)
{
    DBG();
    char fnd;
    int cnt;
    int cnt1;

    for (cnt = 0; old_pwd[cnt] != '\0'; ++cnt)
    {
	for (cnt1 = 0, fnd = DBNO; cnt1 < 90; ++cnt1)
	{
	if (old_pwd[cnt] == password_array[cnt1][0])
	    {
		new_pwd[cnt] = password_array[cnt1][1];
	fnd = DBYES;
		break;
	    } /* End if for match found */
	} /* End for */
	/* If reach this point match not found */
	if (fnd == DBNO)
	{
	    strcpy(rtn.msg, "Invalid character found.");
	    return(FAILURE);
	} /* End if for character not found */
    } /* End big for */
    new_pwd[cnt] = '\0';
    return(SUCCESS);
} /* End password_crypt */
#endif /* For not TRANSAPI */
