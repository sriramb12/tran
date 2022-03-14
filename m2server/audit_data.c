/****************************************************************************
 *
 * Function:	audit_data
 *
 * Description:	Builds check sums for the specified mask set, template,
 *		or PC library.  Sends check sums to remote
 *		servers.  If check sums not correct, sends
 *		remote server a new copy of the data.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <strings.h>
#include <fcntl.h>
#include <alloca.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include "close_down.h"
#include "dir.h"
#include "files.h"
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_prototypes.h"
#include "m2audit_prototypes.h"
#include "m2files.h"
#include "m2mask_files.h"
#include "m2req_types.h"
#include "m2server_globals.h"
#include "m2server_paths.h"
#include "m2server_prototypes.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "rmt_server.h"
#include "mti.h"
#include "rmt_server_lib.h"
#include "rtn.h"
#include "user_params.h"
#include "server_lib.h"
#include "strcpy.h"

/* At this point data path, delta path, and key are set */
/* Build check sums for directories passed in */
/* Send these check sums and get results from remote server */
int audit_state(int refresh_cnt[], int start_index, int end_index)
{
    long checksum;
    char checksum_string[FILE_SIZE];
    register short cnt;
    short ret;

    /* Build check sum for state file */
    checksum = bld_checksum(mti.state_file);
    sprintf(checksum_string, "%s %ld", mti.state_file, checksum);
    /* Build header to send to remote server */
    bld_header(bf.header_buffer, RMT_AUDIT_STATE, mask_msg.data_type, st.transaction_num);
    /* Update each server in a for loop */
    for (cnt = start_index; cnt < end_index; ++cnt)
    {
	/* Connect to remote server */
	/* Send the header */
	/* Get back the response */
	ret = init_rmt_socket(bf.header_buffer, &rmt_mpd, cnt);
	/* If ret less than 0, abort transaction */
	if (ret != 0)
    {
	    data_log(mti.audit_summary, rtn.msg);
	    continue;
    }
	/* Now send the check sums */
	put_string(checksum_string);
	ret = get_rmt_short();
	/* Send check sum for state file */
	if (ret == MARK)
	{
	file_put(mti.delta_dir, "state_file");
	    ++refresh_cnt[cnt];
	} else {
	    get_string(rtn.msg);
	    data_log(mti.audit_summary, rtn.msg);
	    continue;
	}
	/* If reached this point, transaction successful */
	ret =  get_rmt_ret_code(DBYES);
    data_log(mti.audit_summary, rtn.msg);
    } /* End for */
    return(ret);
} /* End audit_state */

int audit_data(int refresh_cnt[], int start_index, int end_index)
{
    char *data_list[1000];
    char *delta_list[1000];
    register short cnt;
    short ret;

    /* List the data and delta directories */
    /* Check sums are built at the time the list is made */
    cnt = mask_dir_list(mti.data_dir, data_list, 0);
    /* Put NULL at end of list */
    data_list[cnt] = NULL;
    files_sort(data_list);
    /* List delta dir and build check sums at same time */
    cnt = mask_dir_list(mti.delta_dir, delta_list, 0) ;
    delta_list[cnt] = NULL;
    files_sort(delta_list);
    /* Build header to send to remote server */
    bld_header(bf.header_buffer, RMT_AUDIT, mask_msg.data_type, st.transaction_num);
    /* Update each server in a for loop */
    for (cnt = start_index; cnt < end_index; ++cnt)
    {
	/* Connect to remote server */
	/* Send the header */
	/* Get back the response */
	ret = init_rmt_socket(bf.header_buffer, &rmt_mpd, cnt);
	/* ret will = REFRESH if mask not present remotely */
	/* In this case just go ahead with the refresh */
	if (ret == REFRESH)
	{
	    refresh_data();
	data_log(mti.audit_summary, rtn.msg);
	++refresh_cnt[cnt];
	continue;
	}
	else if (ret < 0)
    {
	    data_log(mti.audit_summary, rtn.msg);
	    continue;
    }
	/* Now send the check sums */
	static_list_get(data_list);
	static_list_get(delta_list);
	ret = get_rmt_short();
	if (ret == REFRESH)
	{
	    refresh_data();
	data_log(mti.audit_summary, rtn.msg);
	    ++refresh_cnt[cnt];
	}
	else if (ret == SUCCESS) ret = get_rmt_ret_code(DBYES);
	data_log(mti.audit_summary, rtn.msg);
    } /* End for */
    return(ret);
} /* End audit_data */

int audit_dir(char *dir, int start_index, int end_index)
{
     int tmp_sock = sock;
    char *data_list[1000];
    register short cnt;
    short ret;

    /* List the directory being audited */
    /* Check sums are built at the time the list is made */
    /* Call special function to delete non registry files if this is a registry request */
    if (mask_msg.data_type == REGISTER)
    cnt = reg_dir_list(dir, data_list);
    else cnt = mask_dir_list(dir, data_list, 0);
    /* Put NULL at end of list */
    data_list[cnt] = NULL;
    files_sort(data_list);
    /* Build header to send to remote server */
    bld_header(bf.header_buffer, RMT_AUDIT_DIR, mask_msg.data_type, 0);
    /* Update each server in a for loop */
    for (cnt = start_index; cnt < end_index; ++cnt)
    {
	/* Connect to remote server */
	/* Send the header */
	/* Get back the response */
	ret = init_rmt_socket(bf.header_buffer, &rmt_mpd, cnt);
	if (ret != 0)
    {
	    data_log(mti.audit_summary, rtn.msg);
	    continue;
    }
	/* Ret will equal 0 if server is alive */
	/* Now send the check sums */
	static_list_get(data_list);
	ret = get_rmt_short();
	ret = get_rmt_ret_code(DBYES);
    data_log(mti.audit_summary, rtn.msg);
    } /* End for */
    sock = tmp_sock;
    put_short(SUCCESS);
    /* Audit status in the audit report file */
    return_success(rtn.msg, DBNO);
    exit(0);
} /* End audit_dir */

int audit_file(char *file, int start_index, int end_index)
{
    char buffer[FILE_SIZE];
    long checksum;
     int tmp_sock = sock;
    register short cnt;
    short ret;

    /* Build check sum for file */
    /* Make sure the file is present */
    if (access(file, 0) != 0)
    close_down1f(non_err, file, NON_ERR);
    checksum = bld_checksum(file);
    sprintf(buffer, "%s %ld", file, checksum);
    /* Build header to send to remote server */
    bld_header(bf.header_buffer, RMT_AUDIT_FILE, mask_msg.data_type, 0);
    /* Update each server in a for loop */
    for (cnt = start_index; cnt < end_index; ++cnt)
    {
	/* Connect to remote server */
	/* Send the header */
	/* Get back the response */
	ret = init_rmt_socket(bf.header_buffer, &rmt_mpd, cnt);
	if (ret != 0)
    {
	    data_log(mti.audit_summary, rtn.msg);
	    continue;
    }
	/* Ret will equal 0 if server is alive */
	/* Now send the check sums */
	put_string(buffer);
	ret = get_rmt_short();
	ret = get_rmt_ret_code(DBYES);
	data_log(mti.audit_summary, rtn.msg);
    } /* End for */
    sock = tmp_sock;
    put_short(SUCCESS);
    /* Audit status in the audit report file */
    return_success(rtn.msg, DBNO);
    exit(0);
} /* End audit_file */

/* Refresh mask table using data from state and create files */

/* Special audit needed for global data because it  a different structure than other data */
void audit_global(char *key, int refresh_cnt[])
{
    int cnt;
    char *delta_files[4];
    char *data_files[2];
    short ret;

    /* Build the global check sums */
    bld_global_checksums(data_files, delta_files);
    /* Build header to send to remote server */
    bld_header(bf.header_buffer, RMT_AUDIT, mask_msg.data_type, st.transaction_num);
    /* Update each server in a for loop */
    for (cnt = 0; cnt < rmt_mpd.server_num; ++cnt)
    {
	/* Connect to remote server */
	/* Send the header */
	/* Get back the response */
	ret = init_rmt_socket(bf.header_buffer, &rmt_mpd, cnt);
	/* If global data not present, ret will equal refresh */
	if (ret == REFRESH)
	{
	    refresh_global();
	    ++refresh_cnt[cnt];
	    continue;
	}
	else if (ret < 0)
    {
	    data_log(mti.audit_summary, rtn.msg);
    }
	/* Now send the check sums */
	static_list_get(data_files);
	static_list_get(delta_files);
	/* Get response from remote server */
	ret = get_rmt_short();
	if (ret == REFRESH)
	{
	    refresh_global();
	    ++refresh_cnt[cnt];
	}
	else if (ret == SUCCESS) ret = get_rmt_ret_code(DBYES);
    } /* End for */
} /* End audit_ global */

/* audit the remote data */
/* Used to audit mask and  template data */
void rmt_audit_state(void)
{
    char file_name[32];
    long checksum;
    char checksum_string[FILE_SIZE];
    int index;

    /* Indicate remote audit being performed */
    rmt_log4audit("Auditing", mask_msg.key1, mask_msg.key2);
    /* Verify state file is present */
    if (access(mti.state_file, 0) != 0)
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* Read owner of this data */
    /* If requesting database does not own this data, give error */
	index = state_file_read4owner();
    /* Indicate ready to get remote check sums */
     put_short(SUCCESS);
    /* Get the remote check sums and compare to local copy */
    get_string(rtn.msg);
    /* Build check sum for state file */
    checksum = bld_checksum(mti.state_file);
    sprintf(checksum_string, "%s %ld", mti.state_file, checksum);
    if (strcmp(rtn.msg, checksum_string) != 0)
    {
	put_short(MARK);
	/* Get new copy of state file */
	file_get(mti.delta_dir, file_name);
	/* Tell remote server transaction successful */
	strcpy5(rtn.msg, "State file for ", mti.suffix, " refreshed on ", ip.master_site, ".");
    }
    else strcpy4(rtn.msg, "Check sums for ", mti.suffix, " correct on ", ip.master_site);
	data_log(mti.rmt_audit_summary, rtn.msg);
    return_success(rtn.msg, DBNO);
    exit(0);
} /* End rmt_audit_state */

void rmt_audit_data(void)
{
    int index;
    int cnt;
    char *local_list[1000];
    char *rmt_list1[1000];
    char *rmt_list2[1000];

    /* Indicate remote audit being performed */
    rmt_log4audit("Auditing", mask_msg.key1, mask_msg.key2);
    /* Verify state file is present */
    if (access(mti.state_file, 0) != 0)
    {
	rmt_refresh_data();
	data_log(mti.rmt_audit_summary, rtn.msg);
    }
    /* Read owner of this data */
    /* If requesting database does not own this data, give error */
	index = state_file_read4owner();
    /* Indicate ready to get remote check sums */
     put_short(SUCCESS);
    /* Get the remote check sums and compare to local copy */
    list_put(rmt_list1);
    list_put(rmt_list2);
    /* Compare check sums received with local check sums */
cnt = mask_dir_list(mti.data_dir, local_list, 0);
    local_list[cnt] = NULL;
    files_sort(local_list);
    if (checksum_compare(rmt_list1, local_list) != 0)
    {
	rmt_refresh_data();
	data_log(mti.rmt_audit_summary, rtn.msg);
    }
    /* Compare check sums for delta directory */
    cnt = mask_dir_list(mti.delta_dir, local_list, 0);
    local_list[cnt] = NULL;
    files_sort(local_list);
    if (checksum_compare(local_list, rmt_list2) != 0)
    {
	rmt_refresh_data();
	data_log(mti.rmt_audit_summary, rtn.msg);
    }
    /* If reach this point, check sums are the same */
    put_short(SUCCESS);
    strcpy4(rtn.msg, "Check sums for ", mti.suffix, " correct on ", ip.master_site);
    data_log(mti.rmt_audit_summary, rtn.msg);
    return_success(rtn.msg, DBNO);
    exit(0);
} /* End rmt_audit_data */

void rmt_audit_dir(char *dir)
{
    int cnt;
    char *local_list[1000];
    char *rmt_list[1000];

    /* Indicate ready to get remote check sums */
     put_short(SUCCESS);
    /* Get the remote check sums and compare to local copy */
    list_put(rmt_list);
    /* Call special function to delete non registry files if this is a registry request */
    if (mask_msg.data_type == REGISTER)
cnt = reg_dir_list(dir, local_list);
else cnt = mask_dir_list(dir, local_list, 0);
    local_list[cnt] = NULL;
    files_sort(local_list);
    /* Compare check sums received with local check sums */
    if (checksum_compare( local_list, rmt_list) != SUCCESS)
    {
	strcpy4(rtn.msg, "Check sums for directory '", mask_msg.key1, "' not correct on ", ip.master_site);
	data_log(mti.rmt_audit_summary, rtn.msg);
	return_err(rtn.msg, -1);
    } else {
	/* If reach this point, check sums are the same */
	put_short(SUCCESS);
	strcpy4(rtn.msg, "Check sums for directory '", mask_msg.key1, "' correct on ", ip.master_site);
	data_log(mti.rmt_audit_summary, rtn.msg);
	return_success(rtn.msg, DBNO);
	exit(0);
    } /* End if for correct check sums */
} /* End rmt_audit_dir */

void rmt_audit_file(char *file)
{
    char local_buffer[FILE_SIZE];
    char rmt_buffer[FILE_SIZE];
    int checksum;

    /* Indicate ready to get remote check sums */
     put_short(SUCCESS);
    /* Get the remote check sums and compare to local copy */
    get_string(rmt_buffer);
    /* Compare check sums received with local check sums */
    checksum = bld_checksum(mask_msg.key1);
    sprintf(local_buffer, "%s %ld", mask_msg.key1, checksum);
    if (strcmp( local_buffer, rmt_buffer) != SUCCESS)
    {
	strcpy4(rtn.msg, "Check sums for file '", mask_msg.key1, "' not correct on ", ip.master_site);
	data_log(mti.rmt_audit_summary, rtn.msg);
	return_err(rtn.msg, -1);
    } else {
	/* If reach this point, check sums are the same */
	put_short(SUCCESS);
	strcpy4(rtn.msg, "Check sums for file '", mask_msg.key1, "' correct on ", ip.master_site);
    data_log(mti.rmt_audit_summary, rtn.msg);
	return_success(rtn.msg, DBNO);
	exit(0);
    } /* End if for correct check sums */
} /* End rmt_audit_file */

/* Special audit needed to audit remote global data */
void rmt_audit_global(void)
{
    int index;
    char *local_data_files[2];
    char *local_delta_files[4];
    char *rmt_data_files[2];
    char *rmt_delta_files[4];
    int ret;

    /* Verify state file is present */
    if (access(mti.state_file, 0) != 0)
    {
	rmt_refresh_global();
	data_log(mti.rmt_audit_summary, rtn.msg);
    }
    /* Read owner of this data */
    /* If requesting database does not own this data, give error */
	index = state_file_read4owner();
    /* Append key before building check sums */
    strcpy2(mti.data_dir + strlen(mti.data_dir), "/", mask_msg.key1);
    strcpy2(mti.delta_dir + strlen(mti.delta_dir), "/", mask_msg.key1);
    /* build the local check sums */
    bld_global_checksums(local_data_files, local_delta_files);
    /* Indicate ready to get remote check sums */
     put_short(SUCCESS);
    /* Get the remote check sums and compare to local copy */
    list_put(rmt_data_files);
    list_put(rmt_delta_files);
    /* Compare check sums received with local check sums */
    if (checksum_compare(rmt_data_files, local_data_files) != SUCCESS)
    {
	m2list_free(rmt_delta_files);
	m2list_free(local_delta_files);
	rmt_refresh_global();
	data_log(mti.rmt_audit_summary, rtn.msg);
    }
    if ((ret = checksum_compare(rmt_delta_files, local_delta_files)) != SUCCESS)
    {
	rmt_refresh_global();
	data_log(mti.rmt_audit_summary, rtn.msg);
    } /* End if */
	/* If they are the same, send SUCCESS */
	put_short(SUCCESS);
	strcpy4(rtn.msg, "Check sums for ", mti.suffix, " correct on ", ip.master_site);
    data_log(mti.rmt_audit_summary, rtn.msg);
	return_success(rtn.msg, DBNO);
	exit(0);
} /* End rmt_audit_global */

long audit_time_set(void)
{

    /* First build name of audit log files */
    if (strcmp(mask_msg.key1, COMPLETE) == 0) return(0);
    else if (strcmp(mask_msg.key1, WEEKLY) == 0) return(tr.upd_time-SECPERDAY*7);
    else if (strcmp(mask_msg.key1, DAILY) == 0) return(tr.upd_time-SECPERDAY);
    else if (strcmp(mask_msg.key1, HOURLY) == 0) return(tr.upd_time-3600);
    else return(-1);
} /* End audit_time_set */

long bld_checksum(char *file_name)
{
    extern TMP_BUFFER bf;
    register int byte_cnt;
    short total_bytes;
    int fd;
    long checksum = 0;


    /* If this is the data log file, skip it */
    if (strncmp("/data_log", file_name+strlen(file_name)-9, 9) == 0)
    return(checksum);
    /* Open the data file */
    if ((fd = open(file_name, O_RDONLY)) == EOF)
    return(0);
    /* Read in data until end of file reached */
    while ((total_bytes = read(fd, bf.buffer, 1024)) > 0)
    {
        /* Add up the byte count */
        for (byte_cnt = 0; byte_cnt < total_bytes; ++byte_cnt)
        checksum += bf.buffer[byte_cnt];
    } /* End for for reading in data */
    /* Close the data file */
    close(fd);
    return(checksum);
} /* End bld_checksum */

/* Build check sums for global data */
/* Used to build both local and remote global check sums */
void bld_global_checksums(char *data_files[], char *delta_files[])
{
    char files[3][64];
    int cnt;
    long checksum;

    /* Initialize the check sums for the data file */
    /* At this point key is part of the data and delta paths */
    checksum = bld_checksum(mti.data_dir);
    sprintf(bf.buffer, "%s %ld", mti.data_dir, checksum);
    data_files[0] = malloc(strlen(bf.buffer) + 1);
    strcpy(data_files[0], bf.buffer);
    data_files[1] = NULL;
    /* Build names for 2 delta files */
    strcpy(files[0], mti.delta_dir);
    strcpy2(files[1], mti.delta_dir, ".state");
    /* Now build check sums and put file name and check sum in list */
    for (cnt = 0; cnt < 2; ++cnt)
    {
	checksum = bld_checksum(files[cnt]);
	sprintf(bf.buffer, "%s %ld", files[cnt], checksum);
	delta_files[cnt] = malloc(strlen(bf.buffer) + 1);
	strcpy(delta_files[cnt], bf.buffer);
    }
    delta_files[2] = NULL;
} /* End bld_global_checksums */

/* Compares two lists of check sums to see if equal */
int checksum_compare(char *list1[], char *list2[])
{
    int ret = SUCCESS;
    int cnt1;
    int cnt2;

    /* First make sure lists have same number of elements */
    for (cnt1 = 0; list1[cnt1] != NULL; ++cnt1);
    for (cnt2 = 0; list2[cnt2] != NULL; ++cnt2);
    if (cnt1 != cnt2) 
    {
	sprintf(bf.buffer, "List length different: list cnt1 = %d, list2 cnt = %d\n", cnt1, cnt2);
	data_log(mti.audit_summary, bf.buffer);
	m2list_free(list1);
	m2list_free(list2);
	return(FAILURE);
    } /* End if */
    /* Now compare each item in two lists */
    for (cnt1 = 0; list1[cnt1] != NULL; ++cnt1)
    {
	if (strcmp(list1[cnt1], list2[cnt1]) != 0) 
	{
	sprintf(bf.buffer, "Check sums different:\nlist1 = %s\nlist2 = %s\n",
	list1[cnt1], list2[cnt1]);
	data_log(mti.audit_summary, bf.buffer);
	    ret = FAILURE;
	    break;
	} /* End if */
    }
    m2list_free(list1);
    m2list_free(list2);
    return(ret);
} /* End checksum_compare */

/* Make log entry in audit file */
void log2audit(char *str1, char *str2)
{
    FILE *log_ptr;

    if ((log_ptr = fopen(mti.audit_summary, "a")) == NULL)
    close_down1m(wr_open_err, mti.audit_summary, WR_OPEN_ERR);
    fprintf(log_ptr, "%s %s\n", str1, str2);
    fclose(log_ptr);
}

void log4audit(char *str1, char *str2, char *str3)
{
    FILE *log_ptr;

    if ((log_ptr = fopen(mti.audit_summary, "a")) == NULL)
    close_down1m(wr_open_err, mti.audit_summary, WR_OPEN_ERR);
    fprintf(log_ptr, "%s %s %s\n", str1, str2, str3);
    fclose(log_ptr);
}

void rmt_log4audit(char *str1, char *str2, char *str3)
{
    FILE *log_ptr;

    if ((log_ptr = fopen(mti.rmt_audit_summary, "a")) == NULL)
    close_down1m(wr_open_err, mti.audit_summary, WR_OPEN_ERR);
    fprintf(log_ptr, "%s %s %s\n", str1, str2, str3);
    fclose(log_ptr);
}

/* Prints additional information to be put in audit log */
void audit_print (char *data_string, int refresh_cnt[])
{
    int cnt;

    for (cnt = 0; cnt < rmt_mpd.server_num; ++cnt)
    {
	rtn.len += sprintf(rtn.msg+rtn.len, "Found inconsistencies and refreshed %d %s on %s\n",
	refresh_cnt[cnt], data_string, rmt_mpd.site[cnt]);
    } /* end for */
} /* End audit_print */
