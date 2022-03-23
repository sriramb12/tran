/****************************************************************************
 *
 * Function:	check_disk_space
 *
 * Description:	Checks amount of space left on specified partition.
 *		If amount of space left is greater than file size
 *		returns DBYES.
 *
 *Returns:	Returns DBYES if finds disk space. Returns 
 *		DBNO if no disk space is found.
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include "global_defines.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "trans_lib_prototypes.h"
#include "trans_server_globals.h"
#include "global_debug.h"

int check_disk_space(
    int disk_index,
    unsigned long long file_size)
{
    int percent;
    char command[48];
    unsigned long long avail;
    FILE *po;

    DBG();

    /* See how much space is available on specified disk partition */
    /* Build the command to execute */
#ifdef SOLARIS
    strcpy2(command, "df -k ", disk.path[disk_index]);
#endif
/* This used when compiling on Linux */
#ifdef LINUX64
    strcpy2(command, "df ", disk.path[disk_index]);
#endif
#ifdef LINUX32
    strcpy2(command, "df ", disk.path[disk_index]);
#endif
    if ((po = popen (command, "r")) == NULL)
    return(DBNO);
    if (fscanf(po, "%*s %*s %*s %*s %*s %*s %*s %*s %*d %*d %llu %d",
    &avail, &percent)!=2)
    return(DBNO);
    pclose(po);
    /* Deny request if more than 95% disk used */
    if (percent > 95) return(DBNO);
avail *= 1024;
    if (avail < (file_size-10000))
    return(DBNO);
    else return(DBYES);
} /* End check_disk_space */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
