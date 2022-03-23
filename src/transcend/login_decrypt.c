/****************************************************************************
 *
 * Name:	password_encrypt
 *
 * Description:	Builds encrypted password
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/


#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "password.h"
#include "rtn.h"
#include "strcpy.h"
#include "user_params.h"
#include "global_debug.h"


int time_offset_calc(long time_int)
{
    DBG();

    char time_string[24];
    int offset;

    sprintf(time_string, "%ld", time_int);
    offset = time_string[9]-48;
    offset = 0;

    return(offset);
} /* end time_offset_calc */


/* shift password */
void pwd_shift(char *pwd, int offset)
{
    DBG();

    int cnt;

    for (cnt = 0; cnt < 12; ++cnt)
	    pwd[cnt] += offset;

    pwd[12] = '\0';
} /* end pwd_shift */


/* Decrypt user's login name */
/* Called for all transactions except for authentication */
void login_decrypt(char *pwd)
{
    DBG();

    char *ptr;
    char clear_pwd[24];

    pwd[12] = '\0';
    password_crypt(clear_pwd, pwd);
/*
    pwd_shift(clear_pwd, 0);
*/
    if ((ptr = strchr(clear_pwd, '_')) != NULL)
        ptr[0] = '\0';

    strcpy(pwd, clear_pwd);
} /* end login_decrypt */


/* Called when the user authenticates */
void login_encrypt(char *user_login)
{
    DBG();

    char *ptr;
    char clear_pwd[24];
    char pwd[24];

    strcpy3(clear_pwd, user_login, "_=+^", user.mach);
    clear_pwd[12] = '\0';
/*
    pwd_shift(clear_pwd, 0);
*/
    password_crypt(pwd, clear_pwd);
    if ((ptr = strchr(pwd, '_')) != NULL)
        ptr[0] = '\0';

    strcpy(user.login, pwd);
} /* end login_encrypt */
/*ts.files.fileuse*/
