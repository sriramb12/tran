/****************************************************************************
 *
 * Name:	password.c
 *
 * Description:	     Builds encrypted password
 *
 ***************************************************************************/


#include <stdio.h>
#include <string.h>
#include "global_debug.h"


void password_encrypt( char *password, char *pwd, int len);
float get_random( int *ix );
void  user_password_encrypt(char *crypt_pwd, char *pwd);
void encrypt_ascii_pwd(char *int_pwd, char *x500pwd);


/*!  int create_icomrc(char *user_name, char *passwrd)
 *
   \param directory  if null creates in the current directory
   \return           0 on some error,   1 on success
 */
int create_icomrc(char *directory, char *user_name, char *password)
{
     char crypted_password[128] = {0};
     char pathname[1024] = {0};
     FILE *icomrc = NULL;
     int ret = 0;

     encrypt_ascii_pwd(crypted_password, password);
     if (directory != NULL && directory[0] != '\0')
     {
          strcpy(pathname, directory);
          strcat(pathname, "/");
     }
     strcat(pathname, ".icomrc");
     if ((icomrc = fopen(pathname, "w")) != NULL)
     {
          if (    fprintf(icomrc, "User e-mail: %s\n", user_name) > 0
               && fprintf(icomrc, "User password: %s\n", crypted_password) > 0)
          {
               ret = 1;
          }
          fclose(icomrc);
     }
     return ret;
}


/* Adapted from a function written by Howard Anderson */
/* This function does not perform any network functions */
void password_encrypt( char *password, char *pwd, int len)
{
    DBG("password = '%x' pwd = '%s', len='%d'", password, pwd, len);

    int i;
    int iran;
    int iseed;

    pwd[len] = 0;
    iseed = 318057462;

    /* Now encrypt the data one character at a time. */
    /* This is done by exclusive or'ing a random     */
    /* number with each character and                */
    /* outputting the resultant character            */
    for ( i = 0; i < len; ++i)
    {
        iran = 127.0 * get_random(&iseed);
        password[i] = (pwd[i] ^ iran);
    }

    password[len] = '\0';
    DBG("password encrypted");

} /* end password_encrypt */


void user_password_encrypt(char *crypt_pwd, char *pwd)
{
    DBG();

    int cnt;

    for (cnt = 0; pwd[cnt] != '\0'; ++cnt)
    crypt_pwd[cnt] =  pwd[cnt];
    crypt_pwd[cnt] = '\0';
}



/* This routine adapted from page 227 of Simulation and */
/* Modeling Analysis, Law and Kelton, 1982.             */
float get_random( int *ix )
{
    DBG("ix = '%d'", ix);

    float rand;
    int xhi,xalo,leftlo,fhi,k;

    int a   = 16807;
    int b15 = 32768;
    int b16 = 65536;
    int p   = 2147483647;

    xhi  = *ix/b16;
    xalo = (*ix-xhi*b16)*a;
    leftlo = xalo/b16;
    fhi = xhi*a+leftlo;
    k = fhi/b15;
    *ix = (((xalo-leftlo*b16)-p)+(fhi-k*b15)*b16)+k;

    if( *ix < 0 )
        *ix = *ix + p;

    rand = *ix * 4.656612875e-10;
    DBG("will return rand='%.6f'", rand);
    return(rand);
}

/*! void encrypt_ascii_pwd(char *int_pwd, char *x500pwd)
*  \brief generates a password in ascii format such as '83.126.118.65.30.59.37.81.'
*/
void encrypt_ascii_pwd(char *int_pwd, char *x500pwd)
{
     DBG();

     char crypt_pwd[33];
     int len = 0;
     int total_len;
     int cnt;

     /* Get length of password */
     total_len = strlen(x500pwd);

     /* Encrypt the password */
     password_encrypt(crypt_pwd, x500pwd, total_len);

     /* Build integer representation of password */
     for (cnt = 0; cnt <total_len; ++cnt)
     {
          sprintf(int_pwd+len, "%d.", crypt_pwd[cnt]);
          len += strlen(int_pwd+len);
     } /* end for */
} /* end encrypt_ascii_pwd */


#if defined(STANDALONE)
// exaple of compilation: gcc -o generate_icomrc.exe -DSTANDALONE ./client/password.c -Iglobal_include
void help()
{
     printf("   [-i] user password\n");
     printf("   [-d] password\n");
     printf("      -i generates .icomrc in the current directory\n");
     printf("      -d decrypts the password and prints it\n");
}

int password_decrypt(char *decrypted_password, const char *ascii_crypted_password)
{
    unsigned int characters[32];
    char         crypted_password[32];
    int counter = 0;
    int length = sscanf(ascii_crypted_password, "%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.",  // 20 times
                                   &characters[0],
                                   &characters[1],
                                   &characters[2],
                                   &characters[3],
                                   &characters[4],
                                   &characters[5],
                                   &characters[6],
                                   &characters[7],
                                   &characters[8],
                                   &characters[9],
                                   &characters[10],
                                   &characters[11],
                                   &characters[12],
                                   &characters[13],
                                   &characters[14],
                                   &characters[15],
                                   &characters[16],
                                   &characters[17],
                                   &characters[18],
                                   &characters[19]
                                 );
    for (counter = 0; counter < length; ++counter)
    {
        crypted_password[counter] = (char) characters[counter];
     //   printf ("%d ", crypted_password[counter]);
    }
    crypted_password[counter] = 0;
   // printf("\nfiels=%d\n", length);
    password_encrypt(decrypted_password, crypted_password, length);
    decrypted_password[length] = 0;
   // printf("passed=%s\n", decrypted_password);
    return length;
}
#endif
