/****************************************************************************
 *
 * Function:	whereis
 *
 * Description:	Looks for gtar or gnutarwhen a directory
 *		or group of files is being transferred.
 *		Gtar or gnutar is used when present.  Otherwise,
 *		tar is used.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "global_defines.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities



#ifdef WINDOWS_NT4
void tar_type_set(int is_gtar, char *zip_char, char *tar_type, char *zip_command)
{
    DBG();
    strcpy(tar_type, "libarchive");
} /* end tar_type_set */

int whereis_tar(char *tar_path)
{
    DBG();
    strcpy1(tar_path, "tar.exe");
    /* Indicate using gtar */
    return(DBYES);
}

int whereis_gzip_tar(char *tar_path)
{
    DBG();
    strcpy1(tar_path, "tar.exe");
    /* Indicate using gtar */
    return(DBYES);
}

int whereis_gunzip_tar(char *tar_path)
{
    DBG();
    return(strcpy1(tar_path, "tar.exe xf"));
}
#else

void tar_type_set(int is_gtar, char *zip_char, char *tar_type, char *zip_command)
{
    DBG();
    if (is_gtar == DBYES)
    {
	strcpy(tar_type, "gtar");
	strcpy(zip_char, " z");
	strcpy(zip_command, "");
    } else {
	strcpy(tar_type, "tar");
	strcpy(zip_char, " ");
	strcpy(zip_command, "");
    } /* End else */
} /* end tar_type_set */

int whereis_tar(char *tar_path)
{
    DBG();
    extern char tar_option;
    int whereis();
    int len;


    /* First look for gtar */
    if ((tar_option == 't') || (tar_option == 'T'))
    return(strcpy1(tar_path, "tar"));
    if ((len = whereis(tar_path, "gtar")) > 0) return(len);
    else if ((len = whereis(tar_path, "gnutar")) > 0) return(len);
    else return(strcpy1(tar_path, "tar"));
} /* End whereis_tar */

int whereis_gzip_tar(char *tar_path)
{
    DBG();
    extern char tar_option;
    int whereis();
    int len;


    /* First look for gtar */
    if ((tar_option == 't') || (tar_option == 'T'))
    return(strcpy1(tar_path, "tar"));
    if ((len = whereis(tar_path, "gtar")) > 0) 
    return(DBYES);
    else if ((len = whereis(tar_path, "gnutar")) > 0) 
    return(DBYES);
    else return(strcpy1(tar_path, "tar"));
} /* End whereis_gzip_tar */

int whereis_gunzip_tar(char *tar_path)
{
    DBG();
    extern char tar_option;
    int whereis();
    int len;


    /* First look for gtar */
    if ((tar_option == 't') || (tar_option == 'T'))
    return(strcpy1(tar_path, "gunzip --stdout | tar xf -"));
    if ((len = whereis(tar_path, "gtar")) > 0) 
    return(len + strcpy1(tar_path+len, " zxf -"));
    else if ((len = whereis(tar_path, "gnutar")) > 0) 
    return(len + strcpy1(tar_path+len, " zxf -"));
    else return(strcpy1(tar_path, "gunzip --stdout | tar xf -"));
} /* End whereis_gunzip_tar */

int whereis(char *tar_path, char *command)
{
    DBG();
    FILE *po;
    char buffer[64];

    strcpy2(buffer, "whereis ", command);
    if ((po = popen(buffer, "r")) == NULL)
     return(FAILURE);
    while (fscanf(po, "%s", tar_path)  != EOF)
    {
    if (strchr(tar_path, '/') != NULL) return(strlen(tar_path));
    } /* End while */
    /* If reach this point, valid path not found */
    return(FAILURE);
} /* End whereis */
#endif
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
