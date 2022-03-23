/****************************************************************************
 *
 * Function:	files_get
 *
 * Description:	Basic function to send one or more files to the database client.
 *		Files to send are passed in a file array with one or more elements.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <unistd.h>
#include "dir.h"
#include "close_down.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_server_errs.h"
#include "server_lib.h"
#include "strcpy.h"
#include "rtn.h"
#include "global_debug.h" // Mazieri's debug facilities


void file_put(char *dir, char *file_name)
{
    short ret;
    char full_file_name[FILE_SIZE];
    int fd;

    DBG("dir='%s' and file_name='%s'", dir, file_name);

    /* Build complete file name to be opened */
    strcpy3(full_file_name, dir, SLASH,  file_name);

    /* Obtain a file descriptor to file */
    fd = open(full_file_name, O_RDONLY);

    /* Send name of file */
    /* Pass root name only without full path */
    if (fd != EOF) put_string(file_name);

    /* Get acknowledgement */
    /* If unsuccessful, log failure, clean-up and exit */
    if ((ret = get_short()) != SUCCESS)
        shut_down(bad_ack, BAD_ACK);

    /* Read text from the file and write them to the socket */
    while ((ret = read(fd, bf.buffer, BLOCK_SIZE)) > 0)
        put_binary_string(bf.buffer, ret);

    /* Tell client all data for this file sent */
    put_short(SUCCESS);

    /* Close the file just sent */
    close(fd);
} /* End file_put */


/* Send file to client with alternate name */
void file_get_alternate(char *dir, char *file_name, char *alternate_file)
{
    short ret;
    char full_file_name[FILE_SIZE];
    int fd;

    DBG("dir='%s', file_name='%s' and alternate_file='%s'", dir, file_name, alternate_file);

    /* Build complete file name to be opened */
    strcpy3(full_file_name, dir, SLASH,  file_name);

    /* Obtain a file descriptor to file */
    fd = open(full_file_name, O_RDONLY);
    if (fd == EOF) close_down1m(rd_open_err, full_file_name, RD_OPEN_ERR);

    /* Send name of file */
    /* Pass root name only without full path */
    put_string(alternate_file);

    /* Get acknowledgement */
    /* If unsuccessful, log failure, clean-up and exit */
    if ((ret = get_short()) != SUCCESS)
        shut_down(bad_ack, BAD_ACK);

    /* Read text from the file and write them to the socket */
    while ((ret = read(fd, bf.buffer, BLOCK_SIZE)) > 0)
        put_binary_string(bf.buffer, ret);

    /* Tell client all data for this file sent */
    put_short(SUCCESS);

    /* Close the file just sent */
    close(fd);
} /* End file_get_alternate */


void data_get(char *dir, char *file_name)
{
    short ret;
    char full_file_name[FILE_SIZE];
    int fd;

    DBG("dir='%s' and file_name='%s'", dir, file_name);

    /* Build complete file name to be opened */
    strcpy3(full_file_name, dir, SLASH,  file_name);

    /* Obtain a file descriptor to file */
    /* If unsuccessful, log failure, clean-up and exit */
    fd = open(full_file_name, O_RDONLY);
    if (fd == EOF) close_down1f(non_err, full_file_name, NON_ERR);

    put_short(SUCCESS);
    
    /* Get acknowledgement */
    /* If unsuccessful, log failure, clean-up and exit */
    if ((ret = get_short()) != SUCCESS)
        shut_down(bad_ack, BAD_ACK);

    /* Read text from the file and write them to the socket */
    while ((ret = read(fd, bf.buffer, BLOCK_SIZE)) > 0)
        put_binary_string(bf.buffer, ret);

    /* Tell client all data for this file sent */
    put_short(SUCCESS);

    /* Close the file just sent */
    close(fd);
} /* End data_put */


/* Send a group of files to the client */
void files_send(char *dir, char *files[])
{
    extern TMP_BUFFER bf;
    char *file_ptr;	/* Points to first empty character in file path */
    char file_name[FILE_SIZE];
    register int i;
    int fd;		/* File descriptor */
    short ret;		/* Holds return code */   

    /* Initialize the file path */
    file_ptr = file_name + strcpy2(file_name, dir, SLASH);

    for (i = 0; files[i] != NULL; ++i)
    {
	    /* Build complete file name to be opened */
	    strcpy(file_ptr, files[i]);
        DBG("file_name=%s files[%d]=%s", file_name, i, files[i]);

	    /* Obtain a file descriptor to file */
        if ((fd = open(file_name, O_RDONLY)) != EOF)
        {
	    /* Pass root name only without full path */
	    put_string(files[i]);

        /* free(files[i]); */

	    /* Read text from the file and write it to the socket */
	    while ((ret = read(fd, bf.buffer, BLOCK_SIZE)) > 0)
	        put_binary_string(bf.buffer, ret);

	    /* Tell client all data for this file sent */
	    put_short(SUCCESS);

	    /* Close the file just sent */
	    close(fd);
        }
        else
        {
            DBG("file not opened [%s]", file_name);
        }
    } /* end for statement */

    /* Tell client all files have been sent */
    put_short(SUCCESS);
    files[0] = NULL;
    
    /* If unsuccessful, log failure, clean-up and exit */
    if ((ret = get_short()) != SUCCESS)
    {
        DBG("ACK not received at end of files_send()");
        shut_down(bad_ack, BAD_ACK);
    }
} /* end files_send */


int static_files_send(char *dir, char *files[])
{
    extern TMP_BUFFER bf;
    char *file_ptr;	/* Points to first empty character in file path */
    char file_name[FILE_SIZE];
    register int i;
    int fd;		/* File descriptor */
    short ret;		/* Holds return code */

    DBG();

    /* Initialize the file path */
    file_ptr = file_name + strcpy2(file_name, dir, SLASH);

    for (i = 0; files[i] != NULL; ++i)
    {
	    /* Build complete file name to be opened */
	    strcpy(file_ptr, files[i]);
	
	    /* Obtain a file descriptor to file */
	    fd = open(file_name, O_RDONLY);
        if (fd == EOF) continue;

	    /* Send name of file */
	    /* Pass root name only without full path */
	    put_string(files[i]);

	    /* Read text from the file and write it to the socket */
	    while ((ret = read(fd, bf.buffer, BLOCK_SIZE)) > 0)
	        put_binary_string(bf.buffer, ret);

	    /* Tell client all data for this file sent */
	    put_short(SUCCESS);

	    /* Close the file just sent */
	    close(fd);
    } /* end for statement */

    /* Tell client all files have been sent */
    put_short(SUCCESS);
    
    /* Get indication from remote server */
    if ((ret = get_short()) != SUCCESS)
        data_log(tr.log_file, bad_ack);

    return(ret);
} /* end static_files_send */


void file_get(char *dir, char *file)
{
    short ret;
    short fd;
    char full_file_name[FILE_SIZE];
    char *file_ptr;

    DBG("dir='%s' and file='%s'", dir, file);

    file_ptr = full_file_name + strcpy2(full_file_name, dir, SLASH);
    
    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(dir, 0) != 0) dir_create(dir);

    /* First get file name */
    get_string(file_ptr);

    /* Open file for writing */
    if ((fd = open(full_file_name, O_WRONLY|O_CREAT, 0600)) == EOF)
        close_down1m(rd_open_err, full_file_name,  RD_OPEN_ERR);

    /* Tell client ready to get text */
    put_short(SUCCESS);

    /* Read in blocks of text and write them out to the file */
	/* Read a block from the file */
    /* Write the text out to the file */
    while ((ret = get_string(bf.buffer)) > 0)
        write(fd, bf.buffer, ret);

    /* Close the file just created */
    close(fd);

    /* Copy file name into file variable */
    strcpy(file, file_ptr);
} /* End file_get */


void file_put_alternate(char *dir, char *file)
{
    short ret;
    short fd;
    char full_file_name[FILE_SIZE];
    char tmp_file[FILE_SIZE];

    DBG("dir='%s' and file='%s'", dir, file);

    strcpy3(full_file_name, dir, SLASH, file);

    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(dir, 0) != 0) dir_create(dir);

    /* First get file name */
    get_string(tmp_file);

    /* Open file for writing */
    if ((fd = open(full_file_name, O_WRONLY|O_CREAT, 0600)) == EOF)
        close_down1m(rd_open_err, full_file_name,  RD_OPEN_ERR);
    
    /* Tell client ready to get text */
    put_short(SUCCESS);
    
    /* Read in blocks of text and write them out to the file */
	/* Read a block from the file */
    /* Write the text out to the file */
    while ((ret = get_string(bf.buffer)) > 0)
        write(fd, bf.buffer, ret);

    /* Close the file just created */
    close(fd);
} /* End file_put_alternate */


void gds_file_get(char *dir)
{
    short ret;
    short fd;
    char *root_ptr;
    char gds_file[FILE_SIZE];
    char full_file_name[FILE_SIZE];
    char *file_ptr;

    DBG("dir='%s'", dir);

    file_ptr = full_file_name + strcpy2(full_file_name, dir, SLASH);

    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(dir, 0) != 0) dir_create(dir);

    /* First get file name */
    get_string(gds_file);
    root_ptr = root_find(gds_file, ':');
    strcpy(file_ptr, root_ptr);

    /* Open file for writing */
    if ((fd = open(full_file_name, O_WRONLY|O_CREAT, 0600)) == EOF)
        close_down1m(rd_open_err, full_file_name,  RD_OPEN_ERR);

    /* Tell client ready to get text */
    put_short(SUCCESS);

    /* Read in blocks of text and write them out to the file */
	/* Read a block from the file */
    /* Write the text out to the file */
    while ((ret = get_string(bf.buffer)) > 0)
        write(fd, bf.buffer, ret);

    /* Close the file just created */
    close(fd);
} /* End gds_file_get */


void gds_file_put(char *dir, char *local_file, char *remote_file)
{
    char buffer[BUFFER_SIZE];
    short ret;
    short fd;
    char full_file_name[FILE_SIZE];

    DBG("dir='%s', local_file='%s' and remote_file='%s'", dir, local_file, remote_file);

    /* Build complete file name to be opened */
    strcpy3(full_file_name, dir, SLASH,  local_file);

    /* Obtain a file descriptor to file */
    fd = open(full_file_name, O_RDONLY);
    if (fd == EOF) close_down1m(rd_open_err, full_file_name, RD_OPEN_ERR);

    /* Tell user ready to send data */
    put_short(SUCCESS);

    /* Send name of file */
    /* Pass remote file name already generated */
    put_string(remote_file);

    /* Save name for return message sent later */
    /* Get ack from client */
    if (get_short() != SUCCESS) shut_down(bad_ack, BAD_ACK);

    /* Read text from the file and write them to the socket */
    while ((ret = read(fd, buffer, BLOCK_SIZE)) > 0)
        put_binary_string(buffer, ret);

    /* Tell client all data for this file sent */
    put_short(SUCCESS);

    /* Close the file just sent */
    close(fd);
} /* End gds_file_put */


void file_put_append(char *dir, char *file)
{
    short ret;
    short fd;
    char full_file_name[FILE_SIZE];
    char *file_ptr;

    DBG("dir='%s' and file='%s'", dir, file);

    file_ptr = full_file_name + strcpy2(full_file_name, dir, SLASH);
    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(dir, 0) != 0) dir_create(dir);

    /* First get file name */
    get_string(file_ptr);

    /* Open file for writing */
    if ((fd = open(full_file_name, O_WRONLY|O_CREAT, 0600)) == EOF)
        close_down1m(rd_open_err, full_file_name,  RD_OPEN_ERR);

    /* Point to the end of the file */
    lseek(fd, 0, SEEK_END);

    /* Tell client ready to get text */
    put_short(SUCCESS);

    /* Read in blocks of text and write them out to the file */
	/* Read a block from the file */
    /* Write the text out to the file */
    while ((ret = get_string(bf.buffer)) > 0)
        write(fd, bf.buffer, ret);

    /* Close the file just created */
    close(fd);

    /* Copy file name into file variable */
    strcpy(file, file_ptr);
} /* End file_put */


/* Get data from client and pipe it through a command */
void command_put(char *command)
{
    short ret;
    FILE *po;
    short get_string();

    DBG("command='%s'", command);

    /* First get ack from client */
    ret = get_short();

    /* Open command to pipe data from client through */
    if ((po = popen(command, "w")) == NULL) close_down1m(rd_open_err, command,  RD_OPEN_ERR);

    /* Tell client ready to get text */
    put_short(SUCCESS);

    /* Read in blocks of text and write them out to the pipe */
    while ((ret = get_string(bf.buffer)) > 0) fwrite(bf.buffer, ret, 1, po);

    /* Close pipe just wrote to */
    pclose(po);
} /* End command_put */


/* Just like file get except do not pass file name */
int data_put(char *dir, char *file)
{
    short ret;
    short fd;
    short get_string();
    char full_file_name[FILE_SIZE];

    DBG("dir='%s' and file='%s'", dir, file);

    /* Get indication that client ready to transfer data */
    if ((ret = get_short()) != SUCCESS) return(ret);

    strcpy3(full_file_name, dir, SLASH, file);

    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(dir, 0) != 0) dir_create(dir);

    /* Open file for writing */
    if ((fd = open(full_file_name, O_WRONLY|O_CREAT, 0700)) == EOF)
        close_down1m(rd_open_err, full_file_name,  RD_OPEN_ERR);

    /* Tell client ready to get text */
    put_short(SUCCESS);

    /* Read in blocks of text and write them out to the file */
    while (1)
    {
	    /* Read a block from the socket */
	    ret = get_string(bf.buffer);
	    if (ret == 0) break;
	    /* Write the text out to the file */
	    write(fd, bf.buffer, ret);
    } /* end while */

    /* Close the file just created */
    close(fd);

    return(SUCCESS);
} /* End data_put */


/* Returns number of files put */
int files_rcv(char *dir, char *files[])
{
    char full_file_name[FILE_SIZE];
    extern TMP_BUFFER bf;
    short ret = 1;
    int cnt = 0;
    short fd;
    char *file_ptr;	/* Points to end of path name */

    DBG();

    /* Build directory name where files will be placed */
    file_ptr = full_file_name + strcpy2(full_file_name, dir, SLASH);

    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(dir, 0) != 0) dir_create(dir);

    /* Retrieve the files in a while loop */
    while (get_string(file_ptr) > 0)
    {
	    /* Open file for writing */
	    if ((fd = open(full_file_name, O_WRONLY|O_CREAT, 0600)) == EOF)
        {
	        close_down1m(wr_open_err, full_file_name,  WR_OPEN_ERR);
        }

	    /* Read in blocks of text and write them out to the file */
	    while((ret = get_string(bf.buffer)) > 0)
	    {
	        /* Write the text out to the file */
	        write(fd, bf.buffer, ret);
        } /* End while */

	    /* Close the file just created */
	    close(fd);

	    /* Allocate space for file name */
	    files[cnt] = malloc(strlen(file_ptr) +1);

	    /* Copy file name into file array */
	    strcpy(files[cnt++], file_ptr);
    } /* end while */

    files[cnt] = NULL;

    /* Indicate received all data */
    put_short(SUCCESS);

    /* Restore file name to original state */
    file_ptr[0] = '\0';
    
    if (ret < 0)
    {
	    get_string(rtn.msg);
	    close(sock);
	    data_log(tr.log_file, rtn.msg);
	    exit(0);
    } /* End if */

    return(cnt);
} /* end files_rcv */


/* Get output from a command */
void command_get(char *command)
{
    short ret;
    FILE *po;

    DBG("command='%s'", command);

    /* Open pipe for output from command */
    if ((po = popen(command, "r")) == NULL) shut_down(no_data, TRANS_NO_DATA);

    /* Tell client ready to send data */
    put_short(SUCCESS);

    /* Get acknowledgement */
    /* If unsuccessful, log failure, clean-up and exit */
    if ((ret = get_short()) != SUCCESS)
    {
	    pclose(po);
	    shut_down(bad_ack, BAD_ACK);
    } /* End if */

    /* Read text from the pipe and write it to the socket */
    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BLOCK_SIZE, po)) > 0)
        put_binary_string(bf.buffer, ret);

    /* Tell client all data for this command sent */
    put_short(SUCCESS);
    
    /* Close the command output just sent */
    pclose(po);
} /* End command_get */


void command_get_line(char *command)
{
    short ret;
    FILE *po;

    DBG("command='%s'", command);

    /* Open pipe for output from command */
    if ((po = popen(command, "r")) == NULL)
        shut_down(no_data, TRANS_NO_DATA);

    /* Tell client ready to send data */
    put_short(SUCCESS);

    /* Get acknowledgement */
    /* If unsuccessful, log failure, clean-up and exit */
    if ((ret = get_short()) != SUCCESS)
        shut_down(bad_ack, BAD_ACK);

    /* Read text from the pipe and write it to the socket */
    /* Throw away first line of output */
    line_get(po, bf.buffer);
    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BLOCK_SIZE, po)) > 0)
        put_binary_string(bf.buffer, ret);

    /* Tell client all data for this command sent */
    put_short(SUCCESS);

    /* Close the command output just sent */
    pclose(po);
} /* End command_get */


/* Delete a group of files and free memory for file names */
void files_delete(char *dir, char *files[])
{
    int cnt;
    char file_name[FILE_SIZE];
    char *file_ptr = file_name +strcpy2(file_name, dir, SLASH);
    
    DBG();

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	    strcpy(file_ptr, files[cnt]);
	    if (unlink(file_name) != 0)
	    {
	        sprintf(rtn.msg, dlt_err, errno, file_name);
	        shut_down(rtn.msg, DLT_ERR);
	        free(files[cnt]);
	    } /* End if for unable to delete file */
    } /* end for */

    files[0] = NULL;
} /* end files_delete */


/* build dynamic list of specified files */
int files_build_list(char *dir[], char *file0, char *file1, char *file2, char *file3, char *file4)
{
    DBG();

    if (strcmp(file0, NONE) != 0)
    {
	    dir[0] = malloc(strlen(file0) +1);
	    strcpy(dir[0], file0);
    } else {
	    dir[0] = NULL;
	    return(0);
    }

    if (strcmp(file1, NONE) != 0)
    {
	    dir[1] = malloc(strlen(file1) +1);
	    strcpy(dir[1], file1);
    } else {
	    dir[1] = NULL;
	    return(1);
    }

    if (strcmp(file2, NONE) != 0)
    {
	    dir[2] = malloc(strlen(file2) +1);
	    strcpy(dir[2], file2);
    } else {
	    dir[2] = NULL;
	    return(2);
    }

    if (strcmp(file3, NONE) != 0)
    {
	    dir[3] = malloc(strlen(file3) +1);
	    strcpy(dir[3], file3);
    } else {
	    dir[3] = NULL;
	    return(3);
    }

    if (strcmp(file4, NONE) != 0)
    {
	    dir[4] = malloc(strlen(file4) +1);
	    strcpy(dir[4], file4);
    } else {
	    dir[4] = NULL;
	    return(4);
    }

    dir[5] = NULL;
    return(5);
} /* End files_build_list */


/* Sort the files */
void tmp_files_sort(char *files[])
{
    char tmp_files[4096];
    int cnt;
    int file_num;
    int last_file;
    int i;
    int position;
    char last_name[FILE_SIZE] = " ";
    char *ptr = last_name;

    DBG();

    /* find out how many files there are */
    for (file_num = 0; files[file_num] != NULL; ++file_num)
        last_file = file_num-1;

    for (i = 0; i < file_num; ++i)
    {
	    for (cnt = 0; files[cnt] != NULL; ++cnt)
        {
	        if (strcmp(files[cnt], ptr) < 0)
	            position = cnt;
	    } /* End for */
	
	    /* Found first name on the list */
	    /* Point to file just found */
	    ptr = tmp_files[i] = files[position];

	    /* Move last item on list to fill in slot just taken */
	    files[position] = files[last_file];
	    files[last_file] = NULL;
	
	    --last_file;
    } /* end for */

    /* copy file names back to original list */
    for (cnt = 0; cnt <file_num; ++cnt)
        files[cnt] = tmp_files[cnt];

    files[cnt] = NULL;
} /* End files_sort */


/* Get data from client and pipe it through a command */
long long size_command_put(char *command)
{
    long long size = 0;
    short ret;
    FILE *po;
    short get_string();

    DBG("command='%s'", command);

    /* First get ack from client */
    ret = get_short();

    /* Open command to pipe data from client through */
    if ((po = popen(command, "w")) == NULL)
        close_down1m(wr_open_err, command,  WR_OPEN_ERR);

    /* Tell client ready to get text */
    put_short(SUCCESS);

    /* Read in blocks of text and write them out to the pipe */
    while ((ret = get_string(bf.buffer)) > 0)
    {
        if (fwrite(bf.buffer, ret, 1, po) < 1) close_down1m(write_err, command, WRITE_ERR);
        size += ret;
    }

    /* Close pipe just wrote to */
    pclose(po);

    return(size);
} /* End command_put */


/* Get output from a command */
long long size_command_get(char *command)
{
    short ret;
    long long size = 0;
    FILE *po;

    DBG("command='%s'", command);

    /* Open pipe for output from command */
    if ((po = popen(command, "r")) == NULL)
        shut_down(no_data, TRANS_NO_DATA);

    /* Tell client ready to send data */
    put_short(SUCCESS);

    /* Get acknowledgement */
    /* If unsuccessful, log failure, clean-up and exit */
    if ((ret = get_short()) != SUCCESS)
        shut_down(bad_ack, BAD_ACK);

    /* Read text from the pipe and write it to the socket */
    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BLOCK_SIZE, po)) > 0)
    {
        put_binary_string(bf.buffer, ret);
        size += ret;
    }

    /* Tell client all data for this command sent */
    put_short(SUCCESS);

    /* Close the command output just sent */
    pclose(po);

    return(size);
} /* End command_get */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
