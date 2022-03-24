/****************************************************************************
 *
 * Function:	Mask functions
 *
 * Description:	Basic routine used to retrieve files from the database client. 
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <alloca.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/uio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "close_down.h"
#include "customer_table.h"
#include "dir.h"
#include "edc_defines.h"
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_server_errs.h"
#include "trans_server_params.h"
#include "trans_server_paths.h"
#include "trans_server_defines.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"
#include <zlib.h>
#include "global_debug.h"

extern char global_mask[];

/* Checks to see if this file has a valid etek file name */
char *is_valid_etek(char *file_name)
{
    int len;
    char *root_ptr;

    DBG();

    /* first point to root file name */
    if ((root_ptr = strrchr(file_name, '/')) != NULL)
    ++root_ptr;
    else root_ptr = file_name;
    len = strlen(root_ptr);
	if ((strcmp(root_ptr+len-5, ".cflt") == 0)
	|| (strcmp(root_ptr+len-5, ".cref") == 0)
    || (is_pattern_file(root_ptr, len) == DBYES))
    return(root_ptr);
    else return(NULL);
} /* end is_valid_etek */

/* Build list of files just transferred to the mask shop */
int transferred_files_build(char *transferred_files, char *mask, char *keyword)
{
    int fnd = DBNO;
    char *file_ptr= NULL;
    char line[1024];
    char valid_files[32];
    FILE *fi;
    FILE *fo;

    DBG();


    strcpy4(valid_files, "masks/", mask, "/", keyword);
    /* Set streams to read and write to the files */
    if ((fi = fopen(transferred_files, "r")) == NULL)
    close_down1m(rd_open_err, transferred_files, RD_OPEN_ERR);
    /* Scan the file until get past the line where file names are */
    while (long_line_get(fi, line) != EOF)
    {
	if ((strncmp(line, "File names:", 9) == 0)
	|| (strncmp(line, "Files transferred:", 18) == 0))
	{
	    fnd = DBYES;
	    /* Get the blank line */
	line_get(fi, line);
	break;
	} /* end if */
    } /* end while */
    /* If found, check names */
    if (fnd == DBNO)
    {
	fclose(fi);
	return(0);
    }
    /* Reset fnd for next check */
    fnd = DBNO;
    /* Now open the output file and write valid names to ti */
    if ((fo = fopen(valid_files, "w")) == NULL)
    close_down1m(wr_open_err, valid_files, WR_OPEN_ERR);
    while (long_line_get(fi, line) != EOF)
    {
	/* Initialize the pointer */
	file_ptr = NULL;
	/* Get rid of trailing date info */
	if ((file_ptr = strchr(line, ' ')) != NULL)
	file_ptr[0] = '\0';
	/* If this is a valid file, write it to the file */
	if ((file_ptr = is_valid_etek(line)) != NULL)
	{
	fnd = DBYES;
	fprintf(fo, "%s\n", file_ptr);
	} /* end if for valid file found */
    } /* end while */
    fclose(fi);
    fclose(fo);
    if (fnd == DBNO)
    unlink(valid_files);
    return(0);
} /* end transferred_files_build */


/* Add keyword to mask mapping in db_admin/mask_fil */
/* This is done to retriever can know which file to update */
void mask_keyword_add(char *file_key, char *mask, char *date)
{
    char entry[80];

    DBG();

    strcpy5(entry, file_key, " ", mask, " ", date);
    if (add_entry(MASK_FILE, entry)< SUCCESS)
        close_down1m(wr_open_err, MASK_FILE, WR_OPEN_ERR);
} /* end mask_keyword_add */

int is_alpha(char c)
{
    if (((c >= 'A') && ( c <= 'Z'))
    || ((c >= 'a') && (c <= 'z')))
    return(DBYES);
    else return(DBNO);
} /* End is_alpha */

int is_digit(char c)
{
    if ((c >= '0') && ( c <= '9'))
    return(DBYES);
    else return(DBNO);
}

int is_alphanumeric(char c)
{

    if ((is_alpha(c) == DBYES)
    || (is_digit(c) == DBYES))
    return(DBYES);
     else return(DBNO);
} /* end is alphanumeric */
/* Determine if the file name is a pattern file */
/* Pattern files are 12 characters in length */
/* They end in .nn where nn is a numbr */
/* The first 4 characters must be a mask set name */
int is_pattern_file(char *file, int len)
{
    int ret = DBYES;
    if (len != 12)           ret = DBNO;
    if (file[9] != '.')      ret = DBNO;
    if (is_alpha(file[0]) == DBNO) ret = DBNO;
    if (is_alpha(file[3]) == DBNO) ret = DBNO;
    if (is_digit(file[1]) == DBNO) ret = DBNO;
    if (is_digit(file[2]) == DBNO) ret = DBNO;
    if (is_alphanumeric(file[10]) == DBNO) ret = DBNO;
    if (is_alphanumeric(file[11]) == DBNO) ret = DBNO;

    DBG("file=%s ret=%d" , file, ret);
    return ret;
} /* End is_pattern_file */

/* Verify this is a valid file name */
/* Adds files sent to list of sent files */
void mask_filelist_send(char *mask, char *keyword)
{
    char *time_ptr;
    char file_name[64];
    char date[32];
    long long final_size;
    long long compressed_size = 0;
    struct stat buf;
    short ret;
    int fd;
    int len;

    DBG();

    /* build directory name */
    /* Create the date */
    time_ptr = ctime(&tr.upd_time);
    strcpy3(date, tr.log_file+4, "_", time_ptr+11);
    date[17] = '\0';
    /* Now build the name for the file */
    len = strcpy2(file_name, "masks/", mask);
    /* If mask set directory not present generate it */
    if (access(file_name, 0) != 0) dir_create(file_name);
    /* build file name */
    strcpy2(file_name+len, "/", date);
    /* Open file for writing */
    if ((fd = open(file_name, O_WRONLY | O_CREAT, 0666)) < 0)
    close_down1q(wr_open_err, file_name,  WR_OPEN_ERR);
    /* Tell client ready to get data */
    put_short(SUCCESS);
    /* Read in blocks of text and write them out to the file */
    while ((ret = get_string(bf.buffer)) > 0)
    {
	ret = write(fd, bf.buffer, ret);
	compressed_size += ret;
    } /* end while */
    /* Get the actual size */
    final_size = get_longlong();
    /* Close the file just generated */
    close(fd);
    /* Compare the two file sizes */
    /* Do a stat to compare compressed size with real size */
    stat (file_name, &buf);
    if (buf.st_size != compressed_size)
    {
	/* Delete the file just transferred */
	unlink(file_name);
	/* Build error message to send to user */
	sprintf(rtn.msg, bad_size, PROGRAM, compressed_size, buf.st_size);
	/* Send error message */
	shut_down(rtn.msg, BAD_SIZE);
    } /* End if for file sizes do not match */
    /* Otherwise put entry in global mask file */
    else
    {
mask_keyword_add(keyword, mask, date);
    /* Build list of valid files to check later */
    transferred_files_build(file_name, mask, keyword);
	return_success("List of files transferred saved in the Intercom database.", DBNO);
    } /* end else */
} /* end mask_filelist_send */

/* Scan in list of files from the file received */
int mask_filelist_parse(char *files[], char *file_name)
{
    char *ptr;
    char line[80];
    int cnt = 0;
    char tmp_file[256];
    FILE *fp;

    DBG();

    if ((fp = fopen(file_name, "r")) == NULL)
    {
	printf("could not open %s\n", file_name);
	return(FAILURE);
    }
	printf("opened %s\n", file_name);
    /* Look for file name text */
    while(line_get(fp, line) != EOF)
    {
	if (strcmp(line, "File names:") == 0) break;
    }
    /* Skip past next line */
    line_get(fp, line);
    /* Now get the file names */
    while (line_get(fp, line) != EOF)
    {
	if (sscanf(line, "%s", tmp_file) != 1) break;
	/* find leaf of file name */
	if ((ptr = rindex(tmp_file, '/')) != NULL)
	++ptr;
	else ptr = tmp_file;
	files[cnt] = malloc(strlen(ptr) +1);
	strcpy(files[cnt], ptr);
	++cnt;
    } /* end while */
    /* Close file */
    fclose(fp);
    return(cnt);
} /* End mask_filelist_parse */

/* Verify files not already sent to the mask shop */
/* Check against existing files */
int mask_filelist_not_sent(char *new_list[])
{
    char file_name[64];
    char *current_list[50000];
    int cnt;
    int num;

     DBG();

    /* build name of file which holds file names already sent */
    strcpy3(file_name, "masks/", global_mask, "/files_sent");
    /* If the file is not present simply return */
    if (access(file_name, 0) != 0)
    return(SUCCESS);
    if ((num = list_read(file_name, current_list)) <= 0)
    /* If no files sent, there is nothing to compare against */
    return(SUCCESS);
    /* New list is passed in */
    for (cnt = 0; new_list[cnt] != NULL; ++cnt)
    {
	if (list_find( current_list, new_list[cnt]) == SUCCESS)
	{
	sprintf(rtn.msg, file_sent_err, new_list[cnt]);
	    return(FILE_SENT_ERR);
	} /* end if */
    } /* End for */
    return(SUCCESS);
} /* end mask_filelist_not_sent */

/* builds list of .cflt .cref and pattern files to keep track of */
int mask_filelist_build_valid(char *new_list[], char *old_list[])
{
    int cnt;
    int cnt1;
    int len;

    DBG();

    for (cnt = 0, cnt1 = 0; old_list[cnt] != NULL; ++cnt)
    {
	len = strlen(old_list[cnt]);
	if ((strcmp(old_list[cnt]+len-5, ".cflt") == 0)
	|| (strcmp(old_list[cnt]+len-5, ".cref") == 0)
    || (is_pattern_file(old_list[cnt], len) == DBYES))
	{
	new_list[cnt1] = malloc(len+10);
	strcpy(new_list[cnt1], old_list[cnt]);
	++cnt1;
	} /* end if for file to check found */
    } /* end for */
    new_list[cnt1] = NULL;
    return(cnt1);
} /* end mask_filelist_build_valid */

/* Check mask file list to see if files already sent to the mask shop */
int mask_filelist_check(char *new_files[], char *old_files[])
{
    int cnt;
    int ret;

    DBG();
    /* Indicate ready to get data */
    put_short(SUCCESS);
    /* Get the mask file list */
    list_put(old_files);
    /* Determine which names should be checked */
    /* If no valid names to check found, return SUCCESS */
    if ((cnt = mask_filelist_build_valid(new_files, old_files)) == 0)
    {
        DBG("mask_filelist_build_valid() returned 0,skipping ");
    return(0);
    }
    /* Verify the files have not been sent to the mask shop */
    if ((ret = mask_filelist_not_sent(new_files)) != SUCCESS)
    {
    shut_down(rtn.msg, ret);
    }
    return(cnt);
} /* End mask_filelist_check */


/* Add new file names to list */
void mask_filelist_add(char *mask, char *file_key)
{
    char dir_name[64];
    char command[160];
    char files_sent[64];
    char backup_file[64];
    char tmp_sorted_file[64];
    char keyword_file[64];

    DBG();

    /* Check to see if mask directory and keyword file present */
    strcpy2(dir_name, "masks/", mask);
    /* Build name of keyword file */
    strcpy4( keyword_file, "masks/", mask, "/", file_key);
    if (access(dir_name, 0) != 0) 
    {
	dir_create(dir_name);
	/* Log error for this get */
	sprintf(rtn.msg, "No files sent for mask %s for key %s\n", mask, file_key);
	strcat(dir_name, "/errors");
	data_log(dir_name, rtn.msg);
	return;
    }
    /* If keyword file not present, there are no files to add to list */
    else if (access(keyword_file, 0) != 0) return;
    /* Build name of original file */
    strcpy3(files_sent, "masks/", mask, "/files_sent");

    /*
     *  March 2018, Carlos Mazieri
     *    decided to keep the first keyword file for history purposes
     */
    // If original file not present, simply copy from  keyword_file
    if (access(files_sent, 0) != 0)
    {
         file_copy(keyword_file, files_sent);
         return;
    } // End if for original file not present


    /* build other names needed */
    strcpy2(tmp_sorted_file, files_sent, ".sorted");
    strcpy2(backup_file, files_sent, ".bak");
    /* Build command to do the sort */
    strcpy6(command, "cat ", files_sent, " ", keyword_file, 
    " | sort -u >", tmp_sorted_file);
    system(command);
    /* Overwrite old file with new file */
    file_copy(files_sent, backup_file);
    unlink(files_sent);
    rename(tmp_sorted_file, files_sent);
} /* End mask_filelist_add */



/* Allows user to retrieve history of mask set from Intercom repository */
void maskshop_history_get(char *mask)
{
    char dir[64];
    char *files[1000];

    DBG();

    /* build mask set directory name */
    strcpy2(dir, "masks/", mask);
    if (access(dir, 0) != 0)
    {
     strcpy3(rtn.msg, "No history present for '", mask, "'.");
	shut_down(rtn.msg, -1);
    }
    /* Tell user ready to send files */
    put_short(SUCCESS);
    /* Make list of files */
    dir_list(dir, files);
    /* Send files */
    files_send(dir, files);
    /* Send the return code */
    sprintf(rtn.msg,
     "Directory '%s.history' retrieved.  This directory\ncontains history of transfers for mask set '%s'.",
    mask, mask);
    return_success( rtn.msg, DBNO);
} /* End maskshop_history_get */

/* This adds entry that mask shop retrieved the data */
void mask_data_file_update(char *mask, char *date)
{

    FILE *fp;
    char string[48];
    char file_name[80];
    long completion_date;

    DBG();
    strcpy4(file_name, "masks/", mask, "/", date);
    if ((fp = fopen(file_name, "a")) == NULL)
	return;
    /* Write info to the file */
    fprintf(fp, DASHES);
    fprintf(fp, "Data retrieval by the mask shop:\n\n");
    fprintf(fp, "Mask shop name:  %s\n", gu.from_company);
    fprintf(fp, "Retrieved by:  %s\n", user.name);
    fprintf(fp, "Start date: %s", ctime(&tr.upd_time));
    completion_date = time(0);
    fprintf(fp, "Completion date:  %s", ctime(&completion_date));
    /* Put transfer time in hours, minutes and seconds */
    hms_calc(completion_date-tr.upd_time, string);
    fprintf(fp, "Transfer time:  %s\n", string);
    if (gu.original_size > 0)
    {
	num_gmb_calc(string, gu.original_size);
	fprintf(fp, "Size %s\n\n", string);
    }
    else fprintf(fp, "Size:  0 bytes\n\n");
    fclose(fp);
} /* end mask_data_file_update */

/* There is an entry in db_admin/mask_file which maps keywords to their mask set name */
/* This function returns the associated mask set and date */
int mask_keyword_find(char *file_key, char *mask, char *date)
{
    int ret;
    FILE *fp;
    char line[80];
    char local_key[12];

    DBG();
    if ((fp = fopen(MASK_FILE, "r")) == NULL) return(FAILURE);
    while (line_get(fp, line) != EOF)
    {
	sscanf(line, "%s %s %s", local_key, mask, date);
	if ((ret = strcmp(local_key, file_key)) == 0) 
	{
	sscanf(line, "%*s %s %s", mask, date);
	    fclose(fp);
	    return(SUCCESS);
	} /* end if for found */
	else if (ret > 0) break;
    } /* End while */
    /* If reached this point, not found */
    fclose(fp);
    return(FAILURE);
}

/* this function is called when data actually sent to the mask shop */
void maskshop_files_update(char *file_key)
{
    char mask[8];
    char date[32];

    DBG();
    /* See if this is an external company */
    if (external_verify(gu.from_company) == FAILURE) return;
    /* check for keyword in mask file */
    if (mask_keyword_find(file_key, mask, date) == FAILURE) 
    return;
    /* Now do needed updates */
    /* Add file names to files sent */
     /* A file withthe keyword name holds the list of files sent */
    mask_filelist_add(mask, file_key);
    /* Update the date file to indicate the data has been retrieved */
    mask_data_file_update(mask, date);
    /* Delete entry from the mask set file no longer needed */
    entry_delete(MASK_FILE, file_key);
} /* end maskshop_files_update */

/* Save list of files being transferred */
void mask_filelist_save(char *mask, char *files[], char *file_keyword)
{
    char dir_name[64];
    char file_name[FILE_SIZE];

    DBG();
    /* Check to see if mask directory present */
    strcpy2(dir_name, "masks/", mask);
    if (access(dir_name, 0) != 0) dir_create(dir_name);
    /* Build name of file */
    strcpy3(file_name, dir_name, "/", file_keyword);
    /* Sort the files before writing them out */
    files_sort(files);
    list_write(file_name, files);
} /* End mask_filelist_save */

/* does put for mask shop requests */
void mask_filelist_check_put(char *remote_dir, char *key, long long size)
{
    char *old_files[10000];
    char *new_files[10000];
    int cnt;

    DBG();
    cnt = mask_filelist_check(new_files,old_files);
    /* Retrieve the data */
    put_big_file(remote_dir, key, size);
    /* Add new files to file list */
    if (cnt > 0)
    {
        mask_filelist_save(global_mask, new_files, fs.key);
        mask_filelist_add(global_mask, fs.key);
    }
} /* End mask_filelist_check_put */

/* Initialize the global mask */
    void global_mask_set(void)
{
       extern char global_mask[];
    char *ptr;

    DBG();
    /* Initialize default for mask */
    strcpy(global_mask, NONE);
    /* If key 4 characters in length set global mask */
    if (strlen(gu.key1) == 4) 
    {
	strcpy(global_mask, gu.key1);
	return;
    }
    if ((ptr = strchr(gu.key1, ',')) == NULL) return;
    /* Correct file name */
    ptr[0] = '\0';
    if (strlen(ptr+1) != 4) return;
    /* If reached this point, found a global mask */
    strcpy(global_mask, ptr+1);
    ptr[0] = '\0';
} /* End global_mask_set */

/* Verify receiving company is a mask shop */
int external_verify(char *company)
{

    DBG("company=%s", company);
    if (strcmp(company, "External InterCom Customer") == 0) return(SUCCESS);
    else if (strcmp(company, "External TransWeb Customer") == 0) return(SUCCESS);
    else return(FAILURE);
} /* end external_verify */

/* Generates mask name from data being sent */
void mask_name_generate(char *key)
{
    char mask[5];
    char *ptr;

    DBG("key=%s", key);

    /* If comma already present, mask already present */
if ((ptr = rindex(key, ',')) != NULL)
    return;
    /* If strlength of the key less than 4, valid mask name not present */
    if (strlen(key) < 4) return;
    else strncpy(mask, key, 4);
    mask[4] = '\0';
    /* Now add the name to the key */
    strcpy2(key+strlen(key), ",", mask);
} /* end mask_name_generate */

/* Determines if is a valid mask set name */
int is_mask(char *mask)
{
    DBG("mask=%s", mask);
    if ((mask[0] < 'a') || (mask[0] > 'z')) return(DBNO);
    else if ((mask[1] < '0') || (mask[1] > '9')) return(DBNO);
    else if ((mask[2] < '0') || (mask[2] > '9')) return(DBNO);
    else if ((mask[3] < 'a') || (mask[3] > 'z')) return(DBNO);
    else return(DBYES);
} /* end is_mask */
/*es.files.fileuse*/
/*is.files.fileuse*/
