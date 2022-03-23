#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	file_get
 *
 * Description:	Retrieve a  file from the specified database
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
     #include <sys/types.h>
     #include <sys/stat.h>
#include <time.h>
     #include <fcntl.h>
#include <stdlib.h>
#ifndef WINDOWS_NT4
#include <alloca.h>
#endif
#include <unistd.h>
#include "client_lib.h"
#include "client_files.h"
#include "global_defines.h"
#include "global_client_errs.h"
#include "global_params.h"
#include "hash.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
#include "trans_prototypes.h"
#include "transrc.h"
#include "transapi_defines.h"
#include "global_debug.h"

#define FAILURE -1

typedef struct
{
    long long total_size;
    long long est_size;
    long long bytes;
    long current_time;
} PRINT_STATUS;

    PRINT_STATUS print;


#ifndef WINDOWS_NT4
#ifdef OPCTRANS
unsigned long file_mode_set(void)
{
    DBG();
     mode_t mask;

     /* Set template to 0777 */
    mode_t template = S_IRWXU | S_IRWXG | S_IRWXO;
     mode_t tmp_mask = umask(0);
    umask(tmp_mask);
     /* Create a one's compliment of the current umask */
    mask = ~tmp_mask;
    /* Turn execute mode off */
    /* Clear the high order bits */
        mask &= template;
    return(mask);
} /* End file_mode_set */
#endif
#endif

short file_get(
    char *local_dir,	/* Already initialized */
    char *local_file)	/* Not yet initialized */
{
    DBG();
    short ret;
    int fd;
    char buffer[BUFFER_SIZE];
    char full_file_name[FILE_SIZE];

    /* Get file name from server */
    if ((ret = get_string(local_file)) <= 0)
    {
	get_string(rtn.msg);
	if (ret == 0) return(FAILURE);
	else return(ret);
    }
    /* Build full file name */
    strcpy3(full_file_name, local_dir, "/", local_file);
    /* If file present delete it */
    if (access(full_file_name, 0) == 0) unlink(full_file_name);
/* Mask 2000 files are open with different mode than OPC files */
#ifndef OPCTRANS
    if ((fd = open(full_file_name, O_WRONLY | O_CREAT, 0666)) < 0 )
#else
    if ((fd = open(full_file_name, O_WRONLY | O_CREAT, file_mode_set())) < 0 )
#endif
    {
	sprintf(rtn.msg, bad_create, errno, full_file_name, so.program, HELP_MSG);
	/* Send error message to server */
	put_short(BAD_CREATE);
	close(sock);
		return(FAILURE);
    } /* End if for failure to open file */
    put_short(SUCCESS);
    while ((ret = get_string(buffer) ) >0)
    {
    if (write(fd, buffer, ret) != ret)
	{
	    strcpy7(rtn.msg, bad_write, RET2TAB, "'", full_file_name, "'.", DOUBLE_RET, HELP_MSG);
	    put_short(FAILURE);
	    close(sock);
    close(fd);
	        unlink(full_file_name);
	    return(FAILURE);
	}
	} /* end while */
    close(fd);
    return(SUCCESS);
} /* end file_get */

short file_get_alternate(
    char *local_dir,	/* Already initialized */
    char *full_file_name)	/* Not yet initialized */
{
    DBG();
    short ret;
    int put_in_home = DBNO;
    int fd;
    char buffer[BUFFER_SIZE];
    char local_file[FILE_SIZE];

    /* Get file name from server */
    if ((ret = get_string(local_file)) <= 0)
    {
	get_string(rtn.msg);
	if (ret == 0) return(FAILURE);
	else return(ret);
    }
    /* Build full file name */
    strcpy3(full_file_name, local_dir, "/", local_file);
    /* If file present delete it */
    if (access(full_file_name, 0) == 0) unlink(full_file_name);
    if ((fd = open(full_file_name, O_WRONLY | O_CREAT, 0666)) < 0 )
    {
	put_in_home = DBYES;
	strcpy3(full_file_name, user.home_dir, "/", local_file);
	if ((fd=open(full_file_name, O_WRONLY | O_CREAT, 0666)) < 0)
    {
	sprintf(rtn.msg, bad_create, errno, full_file_name, so.program, HELP_MSG);
	/* Send error message to server */
	put_short(BAD_CREATE);
	close(sock);
		return(FAILURE);
    } /* End if for failure to open file */
    } /* End failure to open first file */
    while ((ret = get_string(buffer) ) >0)
    {
    if (write(fd, buffer, ret) != ret)
	{
	    strcpy7(rtn.msg, bad_write, RET2TAB, "'", full_file_name, "'.", DOUBLE_RET, HELP_MSG);
	    put_short(FAILURE);
	    close(sock);
    close(fd);
	        unlink(full_file_name);
	    return(FAILURE);
	}
	} /* end while */
    /* If put in the home directory, let user know */
    if (put_in_home == DBYES)
    {
	ret = strcpy3(buffer, "INFO :File '", local_file, "' was placed in your home directory\n\n");
	write(fd, buffer, ret);
    } /* end if for in home directory */
    close(fd);
    return(SUCCESS);
} /* end file_get */

short file755get(
    char *local_dir,	/* Already initialized */
    char *local_file)	/* Not yet initialized */
{
    DBG();
    short ret;
    int fd;
    char buffer[BUFFER_SIZE];
    char full_file_name[FILE_SIZE];

    /* Get file name from server */
    if ((ret = get_string(local_file)) <= 0)
    {
	get_string(rtn.msg);
	if (ret == 0) return(FAILURE);
	else return(ret);
    }
    /* Build full file name */
    strcpy3(full_file_name, local_dir, "/", local_file);
    /* If file present delete it */
    if ((fd = open(full_file_name, O_WRONLY | O_CREAT, 0755)) < 0 )
    {
	sprintf(rtn.msg, bad_create, errno, full_file_name, so.program, HELP_MSG);
	/* Send error message to server */
	put_short(BAD_CREATE);
	close(sock);
		return(FAILURE);
    } /* End if for failure to open file */
    put_short(SUCCESS);
    while ((ret = get_string(buffer) ) >0)
    {
    if (write(fd, buffer, ret) != ret)
	{
	    strcpy7(rtn.msg, bad_write, RET2TAB, "'", full_file_name, "'.", DOUBLE_RET, HELP_MSG);
	    put_short(FAILURE);
	    close(sock);
    close(fd);
	        unlink(full_file_name);
	    return(FAILURE);
	}
	} /* end while */
    close(fd);
    return(SUCCESS);
} /* end file777get */


short full_file_get(char *full_file_name)
{
    DBG();
    short ret;
    int fd;
    char buffer[BUFFER_SIZE];

    /* Get file name from server */
    if ((ret = get_string(buffer)) <= 0)
    {
	get_string(rtn.msg);
	if (ret == 0) return(FAILURE);
	else return(ret);
    /* Name from remote server is ignored */
    }
    /* Build full file name */
    /* If file present delete it */
    if (access(full_file_name, 0) == 0) unlink(full_file_name);
/* Mask 2000 files are open with different mode than OPC files */
    if ((fd = open(full_file_name, O_WRONLY | O_CREAT, 0666)) < 0 )
    {
	sprintf(rtn.msg, bad_create, errno, full_file_name, so.program, HELP_MSG);
	/* Send error message to server */
	put_short(BAD_CREATE);
	close(sock);
		return(FAILURE);
    } /* End if for failure to open file */
    put_short(SUCCESS);
    while ((ret = get_string(buffer) ) >0)
    {
    if (write(fd, buffer, ret) != ret)
	{
	    strcpy7(rtn.msg, bad_write, RET2TAB, "'", full_file_name, "'.", DOUBLE_RET, HELP_MSG);
	    put_short(FAILURE);
	    close(sock);
    close(fd);
	        unlink(full_file_name);
	    return(FAILURE);
	}
	} /* end while */
    close(fd);
    return(SUCCESS);
} /* end full_file_get */

/* Just like file_get except does not pass file name */
short data_get(
    char *local_dir,	/* Already initialized */
    char *local_file)	/* Already initialized */
{
    DBG();
    short ret;
    int fd;
    char buffer[BUFFER_SIZE];
    char full_file_name[FILE_SIZE];

    /* Get indication that server will transfer file */
    if ((ret = get_short()) != SUCCESS) 
    {
	get_string(rtn.msg);
	close(sock);
	return(ret);
    }
    /* Build full file name */
    strcpy3(full_file_name, local_dir, "/", local_file);
#ifndef OPCTRANS
    if ((fd = open(full_file_name, O_WRONLY | O_CREAT, 0666)) < 0 )
#else
    if ((fd = open(full_file_name, O_WRONLY | O_CREAT, file_mode_set())) < 0 )
#endif
    {
	sprintf(rtn.msg, bad_create, errno, full_file_name, so.program, HELP_MSG);
	/* Send error message to server */
	put_short(BAD_CREATE);
	close(sock);
		return(FAILURE);
    } /* End if for failure to open file */
    put_short(SUCCESS);
    while ((ret = get_string(buffer) ) >0)
    {
    if (write(fd, buffer, ret) != ret)
	{
	    strcpy7(rtn.msg, bad_write, RET2TAB, "'", full_file_name, "'.", DOUBLE_RET, HELP_MSG);
	    put_short(FAILURE);
	    close(sock);
    close(fd);
	        unlink(full_file_name);
	    return(FAILURE);
	}
	} /* end while */
    close(fd);
    return(SUCCESS);
} /* end data_get */

#ifndef TRANSWEB
/* This is used to retrieve data for Jobsubmit */
short command_get(char *command)
{
    DBG();
    char buffer[BUFFER_SIZE];
    short ret;
    FILE *po;

    /* Get indication that server will transfer data */
    if ((ret = get_short()) != SUCCESS) 
    {
	get_string(rtn.msg);
	close(sock);
	return(ret);
    }
    if ((po = popen(command, "w")) == NULL)
    {
	strcpy7(rtn.msg, command_open4write_err,  RET2TAB, "'",
	command, "'", DOUBLE_RET, HELP_MSG);
	/* Send error message to server */
	put_short(COMMAND_OPEN4WRITE_ERR);
	close(sock);
		return(FAILURE);
    } /* End if for failure to open file */
    put_short(SUCCESS);
    /* Initialize structure for printing amount of data transferred */
    print_init(0);
    while ((ret = get_string(buffer) ) >0)
    {
	if (fwrite(buffer, 1, ret, po) != ret)
	{
	    strcpy7(rtn.msg, command_write_err, RET2TAB, "'", command, "'.", 
	    DOUBLE_RET, HELP_MSG);
	    put_short(FAILURE);
	    close(sock);
    pclose(po);
	    return(FAILURE);
	}
	/* Print amount of data transferred */
    print_status(ret);
	} /* end while */
    /* Tell user data transfer completed */
    PRINTF("Data transfer completed.\n\n");
    pclose(po);
    PRINTF("Data transfer completed.\n\n");
    return(SUCCESS);
} /* end command_get */

long long command_get_compressed(char *command)
{
    DBG();
    gzFile file;
    unsigned long fileSize = 0;
    long long size = 0;
    short ret;
    FILE *po;
    short get_string();

    /* Get indication that server will transfer data */
    if ((ret = get_short()) != SUCCESS) 
    {
	get_string(rtn.msg);
	close(sock);
	return(ret);
    }
    if ((po = popen(command, "w")) == NULL)
    {
	strcpy7(rtn.msg, command_open4write_err,  RET2TAB, "'",
	command, "'", DOUBLE_RET, HELP_MSG);
	/* Send error message to server */
	put_short(COMMAND_OPEN4WRITE_ERR);
	close(sock);
		return(FAILURE);
    } /* End if for failure to open file */
    put_short(SUCCESS);
    /* Initialize structure for printing amount of data transferred */
    if ((file = trans_open("r", fileSize, 0, DIRECTORY)) == NULL)
    {
	strcpy(rtn.msg, gz_sockopen_err);
	close(sock);
	return(GZ_SOCKOPEN_ERR);
    }
    size = block_retrieve(file, po, DIRECTORY, "test", DBNO);
    /* Write any remaining output to file */
    trans_close(file, 0);
    /* Send final ack */
    put_short(SUCCESS);
    /* Close pipe just wrote to */
    pclose(po);
    PRINTF("Data transfer completed.\n\n");
    return(size);
} /* End command_get_compressed */
#endif

/* Erase files freeing the file list */
void files_erase(char *dir, char *files[])
{
    DBG();
    int cnt;
    char file_name[FILE_SIZE];

    for (cnt =0; files[cnt] != NULL; ++cnt)
    {
	strcpy3(file_name, dir, "/", files[cnt]);
	unlink(file_name);
	free(files[cnt]);
     } /* End for */
    files[0] = NULL;
} /* End files_erase */

/* Get specified files */
int files_get(char *dir, char *files[])
{
    DBG();
    short len;
    int fd;
    short ret;
    int cnt = 0;
    char buffer[BUFFER_SIZE];
    char file_name[FILE_SIZE];
    char *file_ptr;

    /* Build first part of file name and point to it */
    file_ptr = strcpy2(file_name, dir, "/") +file_name;
    /* Get file names in a for loop */
    while ((len = get_string(file_ptr)) > 0)
    {
	/* If file is already present, delete it */
	if (access(file_name, 0) == 0) unlink(file_name);
#ifndef OPCTRANS
	if ((fd = open(file_name, O_WRONLY | O_CREAT, 0666)) < 0 )
#else
	if ((fd = open(file_name, O_WRONLY | O_CREAT, file_mode_set())) < 0 )
#endif
	{
	    sprintf(rtn.msg, bad_create, errno, file_name, so.program, HELP_MSG);
	    /* Send error message to server */
	    put_short(BAD_CREATE);
	    close(sock);
		files[cnt] = NULL;
	    	return(FAILURE);
	} /* End if for failure to open file */
	/* Indicate ready to receive file data */
	put_short(SUCCESS);
	while ((ret = get_string(buffer) ) >0)
	{
	    if (write(fd, buffer, ret) != ret)
	    {
		strcpy7(rtn.msg, bad_write, RET2TAB, "'", file_name, "'.", DOUBLE_RET, HELP_MSG);
		put_short(FAILURE);
		close(sock);
    close(fd);
		    unlink(file_name);
		files[cnt] = NULL;
		return(FAILURE);
	    } /* End if for bad write */
	} /* end while */
	/* Close file just created */
	close(fd);
	/* Malloc space for file name */
	if ((files[cnt] = malloc(len + 1)) == NULL)
    close_down1f(malloc_err, "", MALLOC_ERR);
	strcpy(files[cnt++], file_ptr);
    } /* End while */
    files[cnt] = NULL;
    if (len < 0)
    {
	get_string(rtn.msg);
	return(len);
    }
    else return(SUCCESS);
} /* End files_get */

int mask_files_get(char *dir, char *files[])
{
    DBG();
    short len;
    int fd;
    short ret;
    int cnt = 0;
    char buffer[BUFFER_SIZE];
    char file_name[FILE_SIZE];
    char *file_ptr;

    /* Build first part of file name and point to it */
    file_ptr = strcpy2(file_name, dir, "/") +file_name;
    /* Get file names in a for loop */
    while ((len = get_string(file_ptr)) > 0)
    {
	/* If file is already present, delete it */
	if (access(file_name, 0) == 0) unlink(file_name);
	if ((fd = open(file_name, O_WRONLY | O_CREAT, 0666)) < 0 )
	{
	    sprintf(rtn.msg, bad_create, errno, file_name, so.program, HELP_MSG);
	    /* Send error message to server */
	    put_short(BAD_CREATE);
	    close(sock);
		files[cnt] = NULL;
	    	return(FAILURE);
	} /* End if for failure to open file */
	/* Get the data */
	while ((ret = get_string(buffer) ) >0)
	{
	    if (write(fd, buffer, ret) != ret)
	    {
		strcpy7(rtn.msg, bad_write, RET2TAB, "'", file_name, "'.", DOUBLE_RET, HELP_MSG);
		put_short(FAILURE);
		close(sock);
    close(fd);
		    unlink(file_name);
		files[cnt] = NULL;
		return(FAILURE);
	    } /* End if for bad write */
	} /* end while */
	/* Close file just created */
	close(fd);
	/* Malloc space for file name */
	if ((files[cnt] = malloc(len + 1)) == NULL)
    close_down1f(malloc_err, "", MALLOC_ERR);
	strcpy(files[cnt++], file_ptr);
    } /* End while */
    files[cnt] = NULL;
    if (len < 0)
    {
	get_string(rtn.msg);
	return(len);
    } else {
	/* Tell server got all of the files */
	put_short(SUCCESS);
	return(SUCCESS);
    } /* End else */
} /* End mask_files_get */

int execute_files_get(char *dir, char *files[])
{
    DBG();
    short len;
    int fd;
    short ret;
    int cnt = 0;
    char buffer[BUFFER_SIZE];
    char file_name[FILE_SIZE];
    char *file_ptr;

    /* Build first part of file name and point to it */
    file_ptr = strcpy2(file_name, dir, "/") +file_name;
    /* Get file names in a for loop */
    while ((len = get_string(file_ptr)) > 0)
    {
	/* If file is already present, delete it */
	if (access(file_name, 0) == 0) unlink(file_name);
	if ((fd = open(file_name, O_WRONLY | O_CREAT, 0777)) < 0 )
	{
	    sprintf(rtn.msg, bad_create, errno, file_name, so.program, HELP_MSG);
	    /* Send error message to server */
	    put_short(BAD_CREATE);
	    close(sock);
		files[cnt] = NULL;
	    	return(FAILURE);
	} /* End if for failure to open file */
	/* Get the data */
	while ((ret = get_string(buffer) ) >0)
	{
	    if (write(fd, buffer, ret) != ret)
	    {
		strcpy7(rtn.msg, bad_write, RET2TAB, "'", file_name, "'.", DOUBLE_RET, HELP_MSG);
		put_short(FAILURE);
		close(sock);
    close(fd);
		    unlink(file_name);
		files[cnt] = NULL;
		return(FAILURE);
	    } /* End if for bad write */
	} /* end while */
	/* Close file just created */
	close(fd);
	/* Malloc space for file name */
	if ((files[cnt] = malloc(len + 1)) == NULL)
    close_down1f(malloc_err, "", MALLOC_ERR);
	strcpy(files[cnt++], file_ptr);
    } /* End while */
    files[cnt] = NULL;
    if (len < 0)
    {
	get_string(rtn.msg);
	return(len);
    } else {
	/* Tell server got all of the files */
	put_short(SUCCESS);
	return(SUCCESS);
    } /* End else */
} /* End execute_files_get */

int file_put(
    char *dir,	/* Local directory for file */
    char *file_name)	/* Root file name used locally and remotely */
{
    DBG();
    short ret;
    int fd;
    char buffer[BUFFER_SIZE];
    char full_file_name[FILE_SIZE];

    /* Build full file name */
    strcpy3(full_file_name, dir, "/", file_name);
    if ((fd = open(full_file_name, O_RDONLY)) < 0)
    {
	    strcpy7(rtn.msg, bad_rd_open, RET2TAB, "'", file_name, "'", 
	    DOUBLE_RET, bad_rd_open_msg);
	    /* Send error message to server */
	    put_short(BAD_CREATE);
	    close(sock);
	    	return(FAILURE);
    } /* End if for failure to open file */
    /* Send the file name */
    /* At this point, local and remote file are the same name */
    put_string(file_name);
    /* Get ack that server can receive file */
    ret = get_short();
    while ((ret = read(fd, buffer, BLOCK_SIZE)) > 0)
    {
	if (put_binary_string(buffer, ret) != ret)
	{
	} /* End if */
    } /* End while */
    close(fd);
    /* Tell server all data sent */
    put_short(SUCCESS);
    return(SUCCESS);
} /* end file_put */

int file_put_alternate(
    char *dir,	/* Local directory for file */
    char *file_name,	/* Root file name used locally and remotely */
    char *alternate_name)	/* Root name used on the server */
{
    DBG();
    short ret;
    int fd;
    char buffer[BUFFER_SIZE];
    char full_file_name[FILE_SIZE];

    /* Build full file name */
    strcpy3(full_file_name, dir, "/", file_name);
    if ((fd = open(full_file_name, O_RDONLY)) < 0)
    {
	    strcpy7(rtn.msg, bad_rd_open, RET2TAB, "'", file_name, "'", 
	    DOUBLE_RET, bad_rd_open_msg);
	    /* Send error message to server */
	    put_short(BAD_CREATE);
	    close(sock);
	    	return(FAILURE);
    } /* End if for failure to open file */
    /* Send the file name */
    /* At this point local and remote file names are different */
    put_string(alternate_name);
    /* Get ack that server can receive file */
    ret = get_short();
    while ((ret = read(fd, buffer, BLOCK_SIZE)) > 0)
	put_binary_string(buffer, ret);
    close(fd);
    /* Tell server all data sent */
    put_short(SUCCESS);
    return(SUCCESS);
} /* end file_put */

short command_put( char *command, long long size)	/* Command to execute to get output */
{
    DBG();
    short ret;
    FILE *po;
    char buffer[BUFFER_SIZE];

    /* Execute command and get output from command */
    if ((po = popen(command, "r")) == NULL)
    {
	sprintf(rtn.msg, bad_create, errno, command, so.program, HELP_MSG);
	/* Send error message to server */
	put_short(BAD_CREATE);
	close(sock);
		return(FAILURE);
    } /* End if for failure to open file */
    /* Send ack to server indicating ready to send data */
    put_short(SUCCESS);
    /* Get ack that server can receive file */
    ret = get_short();
    /* Initialize structure for printing amount of data transferred */
    print_init(size);
    while ((ret = fread(buffer, 1, BLOCK_SIZE, po)) > 0)
    {
	put_binary_string(buffer, ret);
	/* Print amount of data transferred */
	print_status(ret);
    }
    /* Tell user data transfer completed */
    PRINTF("Data transfer completed.\n\n");
    pclose(po);
    /* Tell server all data sent */
    put_short(SUCCESS);
    return(SUCCESS);
} /* end command_put */

short command_put_compressed( char *command, long long size)	/* Command to execute to get output */
{
    DBG();
     int len;
    short ret;
    FILE *po;
    char buffer[BUFFER_SIZE];
    gzFile file;

    /* Execute command and get output from command */
    if ((po = popen(command, "r")) == NULL)
    {
	sprintf(rtn.msg, bad_create, errno, command, so.program, HELP_MSG);
	/* Send error message to server */
	put_short(BAD_CREATE);
	close(sock);
		return(FAILURE);
    } /* End if for failure to open file */
    /* Send ack to server indicating ready to send data */
    put_short(SUCCESS);
    /* Get ack that server can receive file */
    ret = get_short();
    /* Prepare to write compressed data to socket */
    file = trans_open("w", size, 1, DIRECTORY); 
    if (file == NULL)
    {
	strcpy(rtn.msg, gz_sockopen_err);
	close(sock);
	pclose(po);
    }
    /* Now send the file data itself */
    /* Write out data for as long as can read it in */
    while ( (len = fread(buffer, 1, BLOCK_SIZE, po)) != 0)
    trans_write(file, buffer, len);
    /* Flush any left over data to buffer */
    size = trans_close(file, DIRECTORY);
    /* Close local file */
    pclose(po);
    /* Tell user data transfer completed */
    PRINTF("Data transfer completed.\n\n");
    /* Tell server all data sent */
    put_short(SUCCESS);
    return(SUCCESS);
} /* end command_put_compressed */

short dtms_put_compressed( char *command, long long size)	/* Command to execute to get output */
{
    DBG();
     int len;
    FILE *po;
    char buffer[BUFFER_SIZE];
    gzFile file;

    /* Specify printing every 4 Mb and every 30 seconds */
    transrc.hashByTime = 30;
    transrc.hashBySize = 4*1024*1024;
    /* Execute command and get output from command */
    if ((po = popen(command, "r")) == NULL)
    {
	sprintf(rtn.msg, bad_create, errno, command, so.program, HELP_MSG);
	/* Send error message to server */
	put_short(BAD_CREATE);
	close(sock);
		return(FAILURE);
    } /* End if for failure to open file */
    /* Send ack to server indicating ready to send data */
    put_short(SUCCESS);
    /* Get ack that server can receive file */
/*
    ret = get_short();
*/
    /* Prepare to write compressed data to socket */
    file = trans_open("w", size, 1, DIRECTORY); 
    if (file == NULL)
    {
	strcpy(rtn.msg, gz_sockopen_err);
	close(sock);
	pclose(po);
    }
    /* Now send the file data itself */
    /* Write out data for as long as can read it in */
    while ( (len = fread(buffer, 1, BLOCK_SIZE, po)) != 0)
    trans_write(file, buffer, len);
    /* Flush any left over data to buffer */
    size = trans_close(file, DIRECTORY);
    /* Close local file */
    pclose(po);
    /* Tell user data transfer completed */
    PRINTF("Data transfer completed.\n\n");
    /* Tell server all data sent */
    put_short(SUCCESS);
    return(SUCCESS);
} /* end dtms_put_compressed */

/* Send data to server */
/* Server already knows name of file being sent */
short data_put(
    char *file_name,	/* Root file name used locally and remotely */
    char *dir)	/* Local directory for file */
{
    DBG();
    short ret;
    int fd;
    char buffer[BUFFER_SIZE];
    char full_file_name[FILE_SIZE];

    /* Build full file name */
    strcpy3(full_file_name, dir, "/", file_name);
    if ((fd = open(full_file_name, O_RDONLY)) < 0)
    {
	sprintf(rtn.msg, bad_create, errno, full_file_name, so.program, HELP_MSG);
	/* Send error message to server */
	put_short(BAD_CREATE);
	close(sock);
		return(FAILURE);
    } /* End if for failure to open file */
    /* Send ack instead of file name */
    put_short(SUCCESS);
    /* Get ack from remote server */
    if ((ret = get_short()) != SUCCESS) return(ret);
    while ((ret = read(fd, buffer, BLOCK_SIZE)) > 0)
	put_binary_string(buffer, ret);
    close(fd);
    /* Tell server all data sent */
    put_short(SUCCESS);
    return(SUCCESS);
} /* end data_put */


int files_put(char *dir, char *files[])
{
    DBG();
    short ret;
    int fd;
    int cnt;
    char buffer[BUFFER_SIZE];
    char file_name[FILE_SIZE];
    char *file_ptr;

    /* Build first part of file name and point to it */
    file_ptr = strcpy2(file_name, dir, "/") +file_name;
    /* Get file names in a for loop */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	strcpy(file_ptr, files[cnt]);
	if ((fd = open(file_name, O_RDONLY)) < 0)
	{
	    strcpy7(rtn.msg, bad_rd_open, RET2TAB, "'", file_name, "'", 
	    DOUBLE_RET, bad_rd_open_msg);
	    /* Send error message to server */
	    put_short(BAD_CREATE);
	    close(sock);
	    	return(FAILURE);
	} /* End if for failure to open file */
	/* Send file name */
	ret = put_string(file_ptr);
	/* If ret netative, get return string and return */
	if (ret < SUCCESS)
	{
	    get_string(rtn.msg);
	    return(ret);
	}
	/* Send the file data to the server */
	while ((ret = read(fd, buffer, BLOCK_SIZE)) > 0)
	put_binary_string(buffer, ret);
	/* Close file just sent */
	close(fd);
	/* Tell server all text for file sent */
	put_short(SUCCESS);
    } /* End for */
    /* Indicate all files sent */
    put_short(SUCCESS);
    /* Get indication that all files received */
    return(get_short());
} /* End files_put */

/* Writes string to specified file */

/* Retrieve list of names and place in array of strings */
/* Number of names returned */
/* Returns negative return code if transaction fails */
int names_get(char *names[])
{
    DBG();
    int cnt= 0;
    int len;
    char string[256];

    while((len = get_string(string)) > 0)
    {
	if ((names[cnt] = malloc(len +1)) == NULL)
    close_down1f(malloc_err, "", MALLOC_ERR);
	strcpy(names[cnt++], string);
    } /* End for */
     names[cnt] = NULL;
    /* len returned if negative return code received */
    if (len == 0) return(cnt);
    else return(len);
} /* end names_get */

/* Free names on array of pointers */
void files_free(char *files[])
{
    DBG();
    int cnt;

    for (cnt = 0; files[cnt]!= NULL; ++cnt)
    free(files[cnt]);
    files[0] = NULL;
} /* end files_free */

char *root_find(char *root_ptr, char c)
{
    DBG("root_ptr='%s', c='%c'", root_ptr, c);
    char *ptr;

	ptr = strrchr(root_ptr, c);

#ifdef WINDOWS_NT4
    if (ptr == NULL)
	    ptr = strrchr(root_ptr, '\\');
     if (ptr == NULL)
	    ptr = strrchr(root_ptr, ':');
#endif

	if (ptr == NULL) ptr = root_ptr;
	else ++ptr;
     
    return(ptr);
} /* End root_find */

void print_init(long long size)
{
    DBG();
    /* Indicate no data transferred */
    print.bytes = print.total_size = 0;
    print.est_size = size;
    /* Set time to current time */
    print.current_time = time(0);
    /* Print out message to indicate data transfer has started */
    PRINTF("Data transfer started.\n\n");
} /* end print_init */

/* See if status message must be printed */
void print_status(int bytes)
{
    DBG();
    int elapsed_time;
    long current_time = time(0);

    print.bytes += bytes;
    print.total_size += bytes;
    elapsed_time = current_time-print.current_time;
    if ((elapsed_time >= 30) || (print.bytes >= 4194304))
    {
	printf("%llu MB. transferred", print.total_size/1024/1024);
    /* If file size known, print percentage transferred */
    if (print.est_size)
    {
	/* First calculate percentage */
	    float x = 100.0*print.total_size/print.est_size;
	/* If over 100%, display it as 99.9 */
	if (x >= 100.0) x = 99.9;
	printf ("  (%0.1f%%)", x); 
    } /* End if for file size specified */
    PRINTF ("\n");
	/* Set time back to current time */
	print.current_time = current_time;
    print.bytes = 0;
    } /* end if */
} /* End print_status */



/* Determine if this is a file or directory */
TRANS_FILE_TYPE data_type_get(char *file_name)
{
    DBG("file_name='%s'", file_name);

    int ret;
    struct stat buf;

    ret = stat(file_name, &buf);
    if (ret == -1)
    {
	    char *err_ptr = strerror(errno);
		char err_string[8];
		sprintf(err_string, "%d", errno);

	    rtn.len = strcpy4(rtn.msg, bad_rd_open, RET2TAB_QUOTE, file_name, "'\n\n");
		strcpy6(rtn.msg+rtn.len,"Error message: ", err_ptr, "\nError No:      ", err_string, DOUBLE_RET, HELP_MSG);
	    return(BAD_RD_OPEN);
    }

    /* Set the file type */
    if (S_ISDIR(buf.st_mode)) return(DIRECTORY);
    else return(UNSPECIFIED);
} /* end data_type_get */

#endif /* For not TRANSAPI */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
