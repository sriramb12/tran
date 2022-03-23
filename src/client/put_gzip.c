#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	put_gzip_file
 *
 * Description:	Sends the Transcend/InterCom server a big file or tar file.
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "client_maskshop.h"
#include "client_disk_usage.h"
#include "global_defines.h"
#include "global_params.h"
#include "hash.h"
#include "list.h"
#include "server_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "transrc.h"
#include "transapi_defines.h"
#include "trans_prototypes.h"
#include "client_lib.h"
#include "zconf.h"
#include <zlib.h>

int print_open_file_err(char *full_file_name)
{
    	    char *err_ptr = strerror(errno);
    	    char err_string[256];

    	    sprintf(err_string, "%d", errno);
    rtn.len = strcpy4(rtn.msg, bad_rd_open, RET2TAB_QUOTE,
    full_file_name, "'\n\n");
    	    strcpy6(rtn.msg+rtn.len,"Error message: ", err_ptr,
    	    "\nError No:      ", err_string, DOUBLE_RET, HELP_MSG);
    return(BAD_RD_OPEN);
} /* End print_open_file_err */

/* Calculate size of directory */
unsigned long long dir_size_calc(char *full_file_name,    struct DIR_INFO *dir_ptr)
{
    extern long long global_compressed_size;
    extern int global_file_cnt;
     long long dir_size;
    int local_dir_cnt = 0;

    /* Right now calculating global compressed size for all directories */
    global_compressed_size = 0l;
    global_file_cnt = 0;
    /* Get list of file extensions */
    dbsend_list_extension_request();
	/* Get size of directory without links */
    if (dir_ptr == NULL)
	dir_size = big_random_disk_usage(full_file_name, local_dir_cnt);
	/* Get size of directory with links */
    else dir_size = ldisk_usage(full_file_name, dir_ptr, local_dir_cnt);
    return(dir_size);
} /* end dir_size_calc */

int put_serial_data(
    long long size,
    TRANS_FILE_TYPE file_type,
    int compress_during_transfer,
    int is_tar,
    FILE *fp)
{
    extern int size_printed;
    extern long long global_compressed_size;
    int ret;
    int len;
    char buffer[BLOCK_SIZE];
    long long final_size = 0;
    gzFile file;

    /* Prepare to write compressed data to socket */
    size_printed = DBNO;
    if (compress_during_transfer == DBYES)
    {
    file = trans_open("w", size, 1, file_type); 
    if (file == NULL)
    {
	strcpy(rtn.msg, gz_sockopen_err);
	close(sock);
	if (is_tar == DBYES) pclose(fp);
	else fclose(fp);
	return(GZ_SOCKOPEN_ERR);
    }
    /* Now send the file data itself */
    /* Write out data for as long as can read it in */
    while ( 0 != (len = fread(buffer, 1, BLOCK_SIZE, fp)) )
    trans_write(file, buffer, len);
    /* Flush any left over data to buffer */
final_size =    trans_close(file , file_type);
	put_short(SUCCESS);
    ret = put_longlong(final_size);
    } else {
    trans_hash_init(size, file_type, "w", SERIAL); 
    /* Write out data for as long as can read it in */
    final_size = 0;
    while ( 0 != (len = fread(buffer, 1, BLOCK_SIZE, fp)) )
    {
    	put_binary_string(buffer, len);
	final_size += len;
	trans_hash(len, 0);
    } /* End while */
	/* Print final size */
	trans_hash_close();
	/* Tell server all data sent */
    if (put_short(0) != SUCCESS)
    {
	sprintf(rtn.msg, bad_put, errno, HELP_MSG);
	close(sock);
	return(BAD_PUT);
    }
    ret = put_longlong(final_size);
    } /* End else for uncompressed data */
    /* Close local file */
    if (is_tar == DBYES) pclose(fp);
    else fclose(fp);
    /* Put in carriage return for output separation */
#ifndef TRANSCEND_GUI
    if (size_printed == DBYES) PRINTF("\n");
#endif
    /* Get the return code */
    /* Also get the string to put in the log */
    ret = get_log_string();
    close(sock);
    return(ret);
} /* end put_serial_data */

/* Build the command to write data to the socket */
/* Returns the command built plus the tar type */
int gzip_command_build(char *gtar_command, char *file_command, char * full_file_name, char *tar_type, char *c_option, int req_type, int is_tar)
{
    int local_tar_option;
    char zip_char[4];
    char tar_command[64];
    char zip_command[32];

    if(is_tar == DBYES)
    {
     /* Build the tar command */
      /* Presidence order, gtar, gnutar or tar */
        local_tar_option = whereis_gzip_tar(tar_command);
    /* If local_tar_option set to DBYES, know that we are using gtar */
    /* Now set tar type based on local_tar_option */
    tar_type_set(local_tar_option, zip_char, tar_type, zip_command);
      switch ((int)req_type)
      {
	case  DBPUT_FILES:
	{
	    strcpy6(gtar_command, zip_command, tar_command, zip_char, "cfh - ", c_option, full_file_name);
	    strcpy5(file_command, tar_command, " ", "cfh - ", c_option, full_file_name);
	    req_type = DBPUT_TAR_FILE;
	}
	break;
	case  DBPUTDLT_FILES:
	{
	    strcpy6(gtar_command, zip_command, tar_command, zip_char, "cfh - ", c_option, full_file_name);
	    strcpy5(file_command, tar_command, " ", "cfh - ", c_option, full_file_name);
	    req_type = DBPUTDLT_TAR_FILE;
	}
	break;
	case  DBPUT_DATA:
	{
	    strcpy6(gtar_command, zip_command, tar_command, zip_char, "cf - ", c_option, full_file_name);
	    strcpy5(file_command, tar_command, " ", "cf - ", c_option, full_file_name);
	    req_type = DBPUT_TAR_FILE;
	}
	break;
	case  DBPUTDLT_DATA:
	{
	    strcpy6(gtar_command, zip_command, tar_command, zip_char, "cf - ", c_option, full_file_name);
	    strcpy5(file_command, tar_command, " ", "cf - ", c_option, full_file_name);
	    req_type = DBPUTDLT_TAR_FILE;
	} /* End break */
	break;
	default:
	{
	    strcpy6(gtar_command, zip_command, tar_command, zip_char, "cf - ", c_option, full_file_name);
	    strcpy5(file_command, tar_command, " ", "cf - ", c_option, full_file_name);
	} /* End default */
      } /* End switch */
    } else {
#ifdef NXP
#ifdef INTERCOM
/* 	maskshop_file_log(full_file_name, &buf, DBYES); */
#endif
#endif
	/* Build the command to just pipe it through gzip */
    strcpy2(gtar_command, "gzip --stdout ", full_file_name);
    } /* End else */
    return(local_tar_option);
} /* End gzip_command_build */
/* Check for alternate from email address */
int alternate_from_email_check(char **argv, char *alternate_from_email)
{
    int cnt;

    strcpy(alternate_from_email, NONE);
    for (cnt = 0; argv[cnt] != NULL; ++cnt)
    {
	if (strcmp(argv[cnt], "-f") == 0)
	{
	    /* do not use the -f option as an e-mail address */
	argv[cnt] = NULL;
	    /* If alternate e-mail no address specified after the -f option, nothing to use */
	    if (argv[cnt+1] == NULL) return(DBNO);
	    /* Now copy in the alternate e-mail address */
	    strcpy(alternate_from_email, argv[cnt+1]);
	    return(DBYES);
	} /* end if */
    } /* end for */
    /* If reach this point, no address found */
    return(DBNO);
} /* end alternate_from_email_check */

/* Check for default email addresses in company table */
int  email_check(int argc, char **argv, char *alternate_from_email,  char *description, char *email_string, char **email_ptr)
{
    void static_list_string_convert(char *list[], char *string);
#ifdef TRANSCEND_GUI
    int email_cnt = argc-4;
#else
    int email_cnt = argc-3;
#endif
    int cnt;

    /* If email address specified on command line, convert to string format */
    if (email_cnt > 0)
    {
	/* See if an alternate from e-mail address was specified */
	alternate_from_email_check(argv+4, alternate_from_email);
#ifndef TRANSCEND_GUI
	static_list_string_convert(argv+3, email_string);
	*email_ptr = argv[3];
#endif
#ifdef TRANSCEND_GUI
	static_list_string_convert(argv+4, email_string);
	*email_ptr = argv[4];
#endif
    }
#ifdef INTERCOM
#ifdef NXP 
    else if (strcmp(remote_company_email, NONE) != 0)
	{
	    strcpy(email_string, remote_company_email);
	    email_cnt = 1;
	*email_ptr = email_string;
	} /* End if for company specified email */
#endif
#endif
    else 
    {
	*email_ptr = NONE;
	strcpy(email_string, NONE);
    }
#ifndef TRANSCEND_GUI
    if (email_cnt > 0)
    {
	printf("Enter a one line description for %s data (optional).\n> ", so.program);
	cnt = 0; 
	while (((description[cnt] = getchar()) != '\n') 
	&& (description[cnt] != EOF)
    && (cnt < 159)) ++cnt;
	description[cnt] = '\0';
	/* Put in carriage return for output separation */
	printf("\n");
    }
#endif
    return(email_cnt);
} /* End email_check */

int put_gzip_file(
    REQ_TYPE req_type,
    REQ_TYPE parallel_type,
    long long size,
    long mode,
    TRANS_FILE_TYPE file_type,
    char *full_file_name,
    char *root_file_name,
     char *c_option_ptr,
#ifdef TRANSCEND_GUI
    char *description,
#endif
    int argc,
    char **argv,
    int is_tar)
{
    char new_full_file_name[512];
#ifdef INTERCOM
#ifdef NXP
    struct stat buf;
#endif
#endif
    char tar_type[12]= "0";
    char c_option[4];
    int local_tar_option;
    char gtar_command[128];
    char file_command[80];

#ifdef WINDOWS_NT4
    if (size < 1) size = -size;
#endif
    /* do following if data needs to be tarred */
	    /* Verify files not already sent to mask shop */
    /* Determine if need to follow links */
	/* If this is a path, separate root name from path and add -C option */
    /* This is done only if data is being tarred */
    if ((c_option_ptr != NULL)
    && (is_tar == DBYES))
    {
	int len = abs(c_option_ptr-full_file_name);
	strcpy(c_option, "-C ");
	/* Build new full file name including quotes */
	new_full_file_name[0] = '"';
	strncpy(new_full_file_name+1, full_file_name, len);
	/* Now add names enclosed in quotes */
	strcpy3(new_full_file_name+len+1, "\" \"", full_file_name+len+1, "\"");
    } else {
	strcpy3(new_full_file_name, "\"", full_file_name, "\"");
	c_option[0] = '\0';
    } /* End else */
    /* build command to pipe output to socket */
    local_tar_option = gzip_command_build(gtar_command, file_command, new_full_file_name, tar_type, c_option, req_type, is_tar);
#ifdef tar_problem
    if (is_tar == DBYES)
    {
    sprintf(rtn.msg, nt_put_wrn, root_file_name, root_file_name, root_file_name);
    return(NT_PUT_WRN);
    } /* end if */
#endif
    return(gzip_file(req_type, parallel_type, size, mode, file_type, 
    root_file_name, full_file_name, 
#ifdef TRANSCEND_GUI
    description,
#endif
    tar_type, argc, argv, is_tar, gtar_command, file_command));
} /* End put_gzip_file */

int gzip_file(
    REQ_TYPE req_type,
    REQ_TYPE parallel_type,
    long long size,
    long mode,
    TRANS_FILE_TYPE file_type,
    char *root_file_name,
    char *full_file_name,
#ifdef TRANSCEND_GUI
    char *description,
#endif
    char *tar_type,
    int argc,
    char **argv,
    int is_tar,
    char *gtar_command,
    char *file_command)
{
#ifdef TRANSCEND
    extern int log_files;
#endif
    FILE *fp;
    extern char global_keyword[];
    int email_cnt;
    int compress_during_transfer;
    int is_zipped;
    char *par6;
    char alternate_from_email[48] = NONE;
    char parallel_type_string[4];
    char separate_size_string[12] = NONE;
    extern char global_project[];
    char *email_ptr;
#ifdef INTERCOM
#ifdef NXP 
    extern char is_maskshop[];
    extern char remote_company_email[];
    extern char remote_mask[];
    extern char remote_dir[];
extern char remote_mask[];
#endif
#endif
    extern char tar_option;
    gzFile trans_open();
    char email_string[BUFFER_SIZE];
    short ret;
#ifndef TRANSCEND_GUI
    char description[256];
#endif

    /* Initialize any Email addresses being sent */
    email_cnt = email_check(argc, argv, alternate_from_email, description, email_string, &email_ptr);
    /* See if file is zipped */
    is_zipped = is_zipped_file(root_file_name);
    /* Set up the compression type */
    if ((tar_option == 'u') 
    || (is_zipped_file(root_file_name) == DBYES))
    compress_during_transfer = DBNO;
    else compress_during_transfer = DBYES;
	 /* Connect to the database server */
/* If this is Intercom at NXP, add mask set name */
#ifdef INTERCOM
#ifdef NXP 
    if (strcmp(is_maskshop, "yes") == 0)
    strcpy2(root_file_name+strlen(root_file_name), ",", remote_mask);
    /* Change the request type for mask shops */
    req_type = maskshop_req_type_reset(req_type);
    /* Close the mask shop file being created */
    maskshop_file_close();
#else
    /* Must set global project for Transcend and non NXP versions of InterCom */
    strcpy(global_project, NONE);
#endif
#else
    strcpy(global_project, NONE);
#endif
    /* Put parallel type into string format */
    sprintf(parallel_type_string, "%d", parallel_type);
    /* Indicate if this file has no expiration */
    if (tar_option == 'd')
    par6 = "d";
    else par6 = NONE;
#ifndef WINDOWS_NT4
    sprintf(separate_size_string, "%llu", transrc.separate_block_size);
#endif
    /* If this file is larger than 8 Mb, send as large parallel data */
	if ((ret = init_trans_socket(req_type, root_file_name, email_ptr, 
    remote_dir, size, mode, file_type, 
    /* If sending with foundry_send, supress e-mail notification */
#ifdef SEND2FOUNDRY
	email_ptr, tar_type, separate_size_string, "send2foundry", parallel_type_string, par6)) != SUCCESS)
#else
	email_ptr, tar_type, separate_size_string, alternate_from_email, parallel_type_string, par6)) != SUCCESS)
#endif
    return(ret);
#ifdef INTERCOM
#ifdef NXP 
    /* If sending data to the mask shop, send list of files for verification */
    if ((req_type == DBPUT_VERIFY_FILES)
    || (req_type == DBPUTDLT_VERIFY_FILES))
    {
	if((ret = maskshop_verify_files_send()) < SUCCESS)
	return(ret);
    } /* End if for sending file names to mask shop */
#endif
#endif
    if (email_cnt > 0)
    {
	put_string(description);
    /* Send email in string format */
    put_string(email_string);
    } /* End if */
    /* Now send the file data itself */
    /* Get the file keyword and display on the screen */
    get_string(rtn.msg);
     /* Get indication that server ready to receive data */
    if ((ret = get_short()) < SUCCESS)
    {
	get_string(rtn.msg);
	close(sock);
	return(ret);
    }
    /* Overwrite the parallel type with value from the server */
    parallel_type = (REQ_TYPE)ret;
    /* Scan keyword into variable for Transcend/InterCom library */
#ifdef SEND2FOUNDRY
    sscanf(rtn.msg, "%*s %*s %*s %s", global_keyword);
    send2foundry_results_update(description, global_keyword);
	status_log("keyword", global_keyword);
#endif
#ifdef TRANSCEND_GUI
    sscanf(rtn.msg, "%*s %*s %*s %s", global_keyword);
#else
    /* Display keyword on the screen */
    PRINTF("%s\n\n", rtn.msg);
#ifndef WINDOWS_NT4
    fflush(stdout);
#endif
#endif
    sscanf(rtn.msg, "%*s %*s %*s %s", global_keyword);
#ifdef WINDOWS_NT4
    if (is_tar == DBYES)
    {
    char **files;
	/* Build list of files */
	transdirlist_read(files);
	/* Call put_archive to send data */
	put_archive(root_file_name, full_file_name, &files);
    }
    else
#endif
    if (parallel_type == DBPUT_SEPARATE)
	return(put_separate_compressed_data(full_file_name, global_keyword, size, 10, file_type));
    else if ((parallel_type == DBPUT_PARALLEL)
    || (parallel_type == DBPUT_LARGE_PARALLEL))
	return(put_parallel_compressed_data(parallel_type, full_file_name, global_keyword, size, file_type));
    else if (parallel_type == DBPUT_RANDOM_PARALLEL)
    {
	if ((fp = popen(gtar_command, "r")) == NULL)
	return(print_open_file_err(full_file_name));
	ret = put_parallel_uncompressed_data(full_file_name, global_keyword, size, file_type,  fp);
    } else {
	if (is_tar == DBNO)
	fp = fopen(full_file_name, "r");
	else if (compress_during_transfer == DBNO)
	fp = popen(gtar_command, "r");
	else fp = popen(file_command, "r");
	if (fp == NULL)
	return(print_open_file_err(full_file_name));
    ret = put_serial_data(size, file_type, 
    compress_during_transfer, is_tar, fp);
    } /* End if */
#ifdef NXP
#ifdef INTERCOM
    /* If this is a mask shop, send file to Intercom and Mask Designer */
    if (strcmp(is_maskshop, "yes") == 0)
    {
	char tmp_msg[BUFFER_SIZE];
	char *tmp_msg_ptr = tmp_msg;

	tmp_msg_ptr += strcpy1(tmp_msg_ptr, rtn.msg);
	intercom_file_send(remote_mask, MASKSHOP_LOG);
	tmp_msg_ptr += strcpy1(tmp_msg_ptr, rtn.msg);
	maskshop_file_send();
	tmp_msg_ptr += strcpy1(tmp_msg_ptr, rtn.msg);
	strcpy(rtn.msg, tmp_msg);
    } /* End if for mask shop */
#endif /* for InterCom */
#ifdef TRANSCEND
    if (log_files == DBYES)
    transdirlist_put();
#endif /* for TRANSCEND */
#endif
    return(ret);
} /* End gzip_file */

#endif /* For not TRANSAPI */
