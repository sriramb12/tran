/****************************************************************************
 *
 * Name:	password_encrypt
 *
 * Description:	Builds encrypted password
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/


#include "global_defines.h"
#include "global_params.h"
#include "global_client_errs.h"
#include "global_server_prototypes.h"
#include <stdio.h>
#include "global_debug.h" // Mazieri's debug facilities

/* Adapted from a function written by Howard Anderson */
void password_encrypt(char *password, char *pwd, int len)
{
    DBG("password='%s', pwd='%s' and len='%d'", password, pwd, len);

    int i;
    int iran;
    int iseed;
    float get_random();

    pwd[len] = 0;
    iseed = 318057462;

    /*  Now encrypt the data one character at a time.  */
    /*  This is done by exclusive or'ing a random      */
    /*  number with each character and                 */
    /*  outputting the resultant character             */
    for ( i = 0; i < len; ++i)
    {
        iran = 127.0 * get_random(&iseed); 
        password[i] = (pwd[i] ^ iran);
    }

    password[len] = '\0';
} /* end password_encrypt */


void decrypt_ascii_pwd(char *user_pwd, char *decrypt_pwd)
{
    DBG("user_pwd='%s' and decrypt_pwd='%d'", user_pwd, decrypt_pwd);

    char ascii_pwd[64];
    int i[32];
    int ret;
    int cnt;

    ret = sscanf(user_pwd, "%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.",
    &i[0], &i[1], &i[2], &i[3], &i[4], &i[5], &i[6], &i[7], &i[8], &i[9], &i[10], &i[11], &i[12], &i[13], &i[14], &i[15],
    &i[16], &i[17], &i[18], &i[19], &i[20], &i[21], &i[22], &i[23], &i[24], &i[25], &i[26],  &i[27], &i[28], &i[29], &i[30], &i[31]);

    /* Build string with ASCII values */
    for (cnt = 0; cnt <ret; ++cnt)
        ascii_pwd[cnt] =  i[cnt];

    /* Terminate string with null character */
    ascii_pwd[ret] = '\0';

    /* Now decrypt the password */
    password_encrypt(decrypt_pwd, ascii_pwd, ret);
} /* end decrypt_ascii_pwd */
