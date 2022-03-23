/****************************************************************************
 *
 * Function:	mask_functions
 *
 * Description:	Routines to send data to various mask shops.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <time.h>
#include <unistd.h>
#include "global_client_errs.h"

#include "rtn.h"
#include "server_params.h"
#include "strcpy.h"
#include "trans_hash.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"

#include "global_debug.h"

#define DBPUT_TAR_FILE           6
#define DIRECTORY               'd'
#define UNSPECIFIED             'e'
#define DBMASK_FILELIST_CHECK   79
#define DBPUT_VERIFY_FILES      79
#define DBMASK_FILELIST_SEND    80
#define gz_sockopen_err         "socket error"
#define GZ_SOCKOPEN_ERR         -355

void trans_maskshop_stats_log(int index, int ret, int file_cnt, MASKSHOP *ms, TICKLER *tk, char **files);
int trans_intercom_send(MASKSHOP *ms, TICKLER *tk, char *files[]);

/* Build statistics for MDS database and Intercom */
void trans_maskshop_stats_log(int index, int ret, int file_cnt, MASKSHOP *ms, TICKLER *tk, char **files)
{
    int fi;
    FILE *fo;
    char *plate_list[100];
    char *processstep_list[100];
    int plate_cnt = 0;
    int processstep_cnt = 0;
    int cnt;
    char subject[256];
    char buffer[BLOCK_SIZE];

    DBG();

    /* ret holds value returned from transferring the data */
    if (ret == SUCCESS)
        strcpy5(subject, "Successful transfer of mask set '", tk->mask, "' for '", ms->company, "'");
    else
        strcpy5(subject, "Error during transfer of mask set '", tk->mask, "' to '", ms->company, "'");

    /* First rename the existing file so new file can be built */
    if (access("send2maskshop_log", 0) == 0)
        system("mv send2maskshop_log mds.send2maskshop_log");

    /* Read in the file names and sizes and times from the existing file */
    /* do this only on Solaris */
    /* Build file to append to Mask Prep log */
    if ((fo = fopen("send2maskshop_log", "w")) == NULL)
        strcpy5(subject, "Error during transfer of mask set '", tk->mask, "' to '", ms->company, "'");

    /* Build the first part of file to send to MDS server */
    trans_maskshop_header_build(index, buffer, plate_cnt, processstep_cnt, file_cnt);
    fprintf(fo, "%s\n", buffer);

    /* Now add the plate and layer names */
    if (plate_cnt > 0)
    {
	    fprintf(fo, "Plate names:\n\n");

	    for (cnt = 0; cnt <plate_cnt; ++cnt)
	        fprintf(fo, "%s\n", plate_list[plate_cnt]);
    } /* end if for plates present */

    if (processstep_cnt > 0)
    {
	    fprintf(fo, "Process steps:\n\n");

	    for (cnt = 0; cnt < processstep_cnt; ++cnt)
	        fprintf(fo, "%s\n", processstep_list[plate_cnt]);
    } /* End if for process steps present */

    /* Print the file names present */
    if (file_cnt > 0)
    {
        fprintf(fo, "File names:\n\n");
        if ((fi = open("mds.maskshop_log", O_RDONLY)) > 0)
	    {
	        while ((ret = read(fi, buffer, BLOCK_SIZE)) > 0)
                fprintf(fo, "%s\n", buffer);

	        close(fi);
	    } /* end if for able to open file */
	else
	{
	    for (cnt = 0; cnt < file_cnt; ++cnt)
	        fprintf(fo, "%s\n", files[cnt]);
	} /* end else */
    } /* End if for files present */

    /* Close the output file */
    fclose(fo);

    /* If this is Solaris, save file in MDS database */
#ifdef SOLARIS
    int len;
    /* Save return message so it is not overwritten */
    len = strcpy1(buffer, rtn.msg);

    mask_log_append(tk->mask, "send2maskshop_log", "send2maskshop_log", NONE);
    strcpy2(buffer+len, "\n\n", rtn.msg);

    /* Restore complete rtn.msg */
    strcpy(rtn.msg, buffer);
#endif
} /* end trans_maskshop_stats_log */


int trans_intercom_send(MASKSHOP *ms, TICKLER *tk, char *files[])
{
    DBG();

#ifdef SOLARIS
    char tmp_buffer[64];
    char fab[32];
#endif
    char tmp_dir[FILE_SIZE];
    int file_cnt;
     int ret;
    int index;
    char description[80];

    /* build name for temporary directory */
    strcpy2(tmp_dir, tk->mask, tk->stamp);
	/* Initialize the from company */
    strcpy(server.site, ms->company);
    /* CAll set option to set this field */
    strcpy(user.email, tk->user_email);
    /* Initialize user options */
    /* this includes login name, machine, IP node address, etc. */
    /* It also includes pointers to the global trans structure */
    trans_user_params_init(NULL);
    /* Get an index to a transaction object */
    /* Parameters passed in include */
    /* Overwrite existing files */
    /* Perform background transfer */
    /* Suppress prompting and displaying output */
    /* suppress transfer status */
    index = trans_init(DBNO, DBNO,  DBNO, DBNO);
    /* Set tool type, default repository and read configuration file */
    trans_tool_set(index, INTERNAL_INTERCOM);
    /* Initialize the server machine, address and socket */
    if ((ret = trans_site_find(index, ms->tla)) != SUCCESS)
    return(ret);
	    /* Set the request type */
	    trans_put_request_set(index, DBPUT_VERIFY_FILES, ' ');
    /* Set the symbolic directory */
    if ((ret = trans_symbolic_dir_set(index, tmp_dir)) < 0)
    return(ret);
    /* Initialize names of files in the directory */
    if ((file_cnt = trans_dir_files_link(index, files)) < 0)
    return(file_cnt);
    /* Set e-mail addresses for secure keywords being sent */
    if ((trans[index]->email_list = malloc(strlen(trans[index]->par1) +10)) == NULL)
    {
	strcpy(rtn.msg, malloc_err);
	return(MALLOC_ERR);
    } /* end if */
    else strcpy(trans[index]->email_list, trans[index]->par1);
    /* Build description */
#ifdef SOLARIS
    if (create_file_read(fab, tmp_buffer, tmp_buffer, tmp_buffer, tmp_buffer) == SUCCESS)
    {
	int len = strcpy5(description, "Device=", tk->mask, " Fab=", fab, " Program=MDS Sender=");
	strcpy5(description+len, tk->user_name, ", ", tk->user_email, ", ", tk->user_phone);
    } /* end if */
    else
#endif
    strcpy3(description, "Transfer of mask set '", tk->mask, "'");
    trans_description_set(index, description);
	    ret = trans_perform(index);
    trans_maskshop_stats_log(index, ret, file_cnt, ms, tk, files);
    return(ret);
} /* end trans_intercom_send */

/* Get user's name and phone number from database  using DBRTV command */
short name_user_phone_rtv(TICKLER *tk)
{
    DBG();

    int index;
     char fnd = DBNO;
    short ret;
    char first_name[16];
    char last_name[16];

    /* Initialize the user name, IP node address, machine, etc */
    trans_user_params_init(NULL);
    index = trans_init(DBNO, DBNO, DBNO, DBNO);
    /* Initialize to connect to the default server */
    trans_default_repository_set();
	    trans_simple_basic_request_set(index, DBRTV_NAME_PHONE, tk->user_email);
	    ret = trans_perform(index);
    trans_cleanup(index);
    if (ret == SUCCESS)
    {
	    if (sscanf(rtn.msg, "%s %s %s", first_name, last_name, tk->user_phone) == 3)
	    {
		strcpy3(tk->user_name, first_name, " ", last_name);
    strexc(tk->user_name, '_', ' ');
		fnd = DBYES;
	} /* End if for scan successful */
    } /* end if for transaction successful */
    if (fnd == DBNO)
    {
	strcpy(tk->user_name, "Unknown");
	strcpy(tk->user_phone, "Unknown");
     } /* End if */
    return(SUCCESS);
} /* end name_phone_rtv */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
