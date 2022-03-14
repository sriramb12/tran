/****************************************************************************
 *
 * Function:	process_partition
 *
 * Description:	Processes requests with data type of PC *
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

     #include <stdlib.h>
     #include <alloca.h>
#include <errno.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include "dir.h"
#include "close_down.h"
#include "global_defines.h"
#include "list.h"
#include "mask_server_defines.h"
#include "m2audit_prototypes.h"
#include "m2files.h"
#include "m2server_globals.h"
#include "m2state_table.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2server_errs.h"
#include "m2server_paths.h"
#include "m2req_types.h"
#include "m2server_prototypes.h"
#include "mti.h"
#include "pre_suffix.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "m2msg.h"


void process_partition(void)
{

    /* Make sure user is retrieving data from wacc site */
    if (strcmp(ip.master_site, "wacc") != 0)
    m2server_send("wacc");
    /* Initialize paths needed for PC transactions */
    partition_paths_init();
    switch(mask_msg.req_type)
{
	/* List an existing path */
	case M2LIST:  partition_list(); break;
	case CREATE:  
	{
	    /* Verify user can create partitions */
	    pc_build_check();
	    partition_create(); 
	}
	break;
	/* Delete n existing partition */
	case DELETE:
	{
	    /* Verify user can delete partitions */
	    pc_build_check();
	    partition_delete(); 
	}
	break;
	default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* End switch */
} /* End process_partition */
/* Initialize paths needed to perform PC transactions */
void partition_paths_init(void)
{
	    strcpy(key, mask_msg.key1);
    strcpy2(mti.data_dir, PC_DATA, mask_msg.key1);
    strcpy3(mti.prefix, "partition '", mask_msg.key1, "'");
    strcpy3(mti.suffix, "partition '", mask_msg.key1, "'");
} /* End partition_paths_init */

/* LIst everything in specified partition */
int partition_list(void)
{
    char *names[1024];
    char *tmp_ptr;
    int num;
    int cnt;

    /* Build name of directory to list */
    if (strcmp(mask_msg.key1, "/") == 0) strcpy(mti.data_dir, PC_DATA);
    else strcpy2(mti.data_dir, PC_DATA, mask_msg.key1);
    /* Verify the directory is present */
    if (access(mti.data_dir, 0) != 0)
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* Read list of names from file */
    num = dir_list(mti.data_dir, names);
    /* Check for libraries */
    for (cnt = 0; names[cnt] != NULL; ++cnt)
    {
	/* Build state file name */
	strcpy4(mti.state_file, mti.data_dir, "/", names[cnt], "/state_file");
	if (access(mti.state_file, 0) == 0)
	{
		tmp_ptr = names[cnt];
		names[cnt] = malloc(strlen(tmp_ptr) + 4);
	state_file_read();
	if (st.version_type[0] == '+')
		strcpy2(names[cnt], "+  ", tmp_ptr);
		else strcpy2(names[cnt], "-  ", tmp_ptr);
	    free(tmp_ptr);
	} /* End if for library name */
    } /* End for */
    /* Put the list in alphabetical order */
    /* Ignore upper and lower case while doing the sort */
    files_case_sort(names);
    /* Tell client ready to send names */
    put_short(cnt);
    list_get(names);
    /* Send return code */
    return_success("Partition list retrieved.", DBNO);
} /* End partition_list */

/* Create specified partition */
int partition_create(void)
{

    /* Build name of directory to create */
    strcpy3(mti.data_dir, PC_DATA, "/", mask_msg.key1);
    if (access(mti.data_dir, 0) == 0)
    close_down1f(crt_err, mti.prefix, CRT_ERR);
    /* Verify the name is not of the form number.number */
    /* This check is made so partitions */
    /* which look like version numbers cannot be made. */
    partition_name_check(mask_msg.key1);
    /* Now we are  ready to create the partition */
    dir_make(mti.data_dir);
    /* Tell client transaction successful */
    put_short(SUCCESS);
    mti.suffix_ptr = CRT;
    put_ret_code(DBNO);
} /* End partition_create */

int partition_delete(void)
{
    int ret;
    char *files[M2TOTAL_FILES];

    /* Build name of partition to delete */
    strcpy2(mti.data_dir, PC_DATA, mask_msg.key1);
    if (access(mti.data_dir, 0) != 0)
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* See if there are any directories or files under this directory */
    if (dir_list(mti.data_dir, files) > 0)
    close_down1f(partition_not_empty, mti.prefix, PARTITION_NOT_EMPTY);
    /* Now we are finally ready to delete the partition */
    if ((ret = rmdir(mti.data_dir)) != 0)
    ret = unlink(mti.data_dir);
    if (ret != 0)
    {
	sprintf(rtn.msg, delete_err, errno, mti.prefix);
	shut_down(rtn.msg, DELETE_ERR);
    }
    /* Tell client transaction successful */
    put_short(SUCCESS);
    mti.suffix_ptr = DLT;
    put_ret_code(DBNO);
} /* End partition_create */

/* Partitions of the form number.number are invalid */
/* This function makes sure the partition name does not look like a version number */
void partition_name_check(char *partition)
{

    /* First isolate only the last portion of the partition name */
    char *partition_ptr = rindex(partition, '/');
    /* If only one level in name, point to beginning of name */
    /* If no '/' present, point to beginning of name */
    if (partition_ptr == NULL) partition_ptr = partition;
    /* Point to character just past the '/' */
    else ++partition_ptr;
    /* if there are non-numeric characters, it is valid */
    if (str_numeric_only(partition_ptr) == DBNO) return;
    /* If there is not a '.' in the name, it is valid */
    if (strchr(partition_ptr, '.') == NULL) return;
    /* If the period is at the beginning or end of the name, it is valid */
  if ((partition_ptr[0] == '.') || ((partition_ptr+strlen(partition_ptr) +1) == '.')) return;
    /* By default we now have a name of the form number.number */
    else close_down1f(invalid_partition_name, partition, INVALID_PARTITION_NAME);
} /* End partition_name_check */

/* Verifies string contains only numeric characters including a '.' */
int str_numeric_only(char *string)
{
    int cnt;

    /* Go through string looking for non numeric characters */
    for (cnt = 0; string[cnt] != '\0'; ++cnt)
    {
	/* If non numeric return */
	if (string[cnt] == '.') continue;
	if ((string[cnt] < '0') 
	|| (string[cnt] > '9'))return(DBNO);
    }
    /* If reached this point string is valid */
    return(DBYES);
}
