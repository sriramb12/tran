/****************************************************************************
 *
 * Function:	set_trans_disk_usage
 *
 * Description:  Records available space in each disk partition 
 *		in file db_admin/disk_usage.
 *		Transcend and InterCom servers use this file to determine 
 *		if a file can be transcended.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <sys/file.h>
#include "strcpy.h"
#include "trans_server_paths.h"
#include "global_defines.h"
#include "trans_server_errs.h"
#include "trans_defines.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "trans_server_defines.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"


void set_trans_disk_usage(void)
{
    int cnt;
    int disk_cnt;
    int err_cnt = 0;
    char command[24];
    char msg[256];
    TRANS_USAGE_DISK disk[10];
    FILE *po;
    FILE *fp;

    DBG();

    /* Open the disk usage file for read/write */
    if ((fp = fopen(TRANS_DISK_FILE, "r+")) == NULL)
    {
	strcpy3(msg, wr_open_err, TRANS_DISK_FILE, "'");
	/* Send database administrator a mail message  */
	email_send(admin.err_email, admin.err_email,
	"File open error", msg);
	return;
    }
    disk_cnt = 0;
    while (fscanf(fp, "%s %d %ld %ld", 
        disk[disk_cnt].path,
    &disk[disk_cnt].percent, &disk[disk_cnt].avail, &disk[disk_cnt].increment) == 4)
    ++disk_cnt;
    /* Do the DF for each partition capturing the new percent used */
    for (cnt = 0; cnt <disk_cnt; ++cnt)
    {
	/* Build the command */
#ifdef SOLARIS
	strcpy2(command, "df -k ", disk[cnt].path);
#else
	strcpy2(command, "df -k ", disk[cnt].path);
#endif
     po = popen(command, "r");
    if (fscanf(po, "%*s %*s %*s %*s %*s %*s %*s %*s %*d %*d %ld %d ",
    &disk[cnt].avail, &disk[cnt].percent)!=2)
	{
	    ++err_cnt;
	    continue;
	} /* End if */
	/* Close the output stream */
	pclose(po);
    } /* End for */
    /* If any errors were found, send system administrator message */
    if (err_cnt > 0) 
    {
	strcpy3(msg, read_err, command, "'");
	email_send(admin.err_email, admin.err_email,
	"Command read error", msg);
    } /* End if */
    /* Go to the beginning of the disk usage file */
    if (fseek(fp, 0l, 0) != SUCCESS)
    {
	/* Send database administrator mail message and exit */
	strcpy3(msg, seek_err, TRANS_DISK_FILE, "'");
	email_send("", admin.err_email,
	"File seek error", msg);
	return;
    } /* End if */
    /* Write out new values for  disk available */
    for (cnt = 0; cnt <disk_cnt; ++cnt)
    {
	fprintf(fp, "%s %d %ld %ld\n", 
	disk[cnt].path, 
     disk[cnt].percent, disk[cnt].avail, disk[cnt].increment);
    } /* End for */
    /* Close this file */
    fclose(fp);
}

