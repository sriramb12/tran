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
#include <sys/uio.h>
#include <unistd.h>
#include "close_down.h"
#include "dir.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "trans_server_prototypes.h"
#include "zutil.h"
#include "global_debug.h"

#define gz_sockopen_err "Error opening socket"
#define GZ_SOCKOPEN_ERR -256

extern TRANS_FILE_STAT fs;

int single_file_command_build(char *single_file_path, char *single_file, char *command)
{
    int dir_num;
    char *root_ptr;

    DBG("single_file_path='%s', single_file='%s', command='%s'", single_file_path, single_file, command);

    /* See if multiple levels for the file */
    if ((root_ptr = strrchr(single_file_path,'/')) == NULL)
    {
	    strcpy2(command, "gtar zcf - ", single_file_path);
	    chdir(fs.real_name);

	    return(1);
    } else {
	    /* See how many level of directory are present */
	    dir_num = strcnt_char(single_file_path, '/');

	    /* Split name into directory and single file name */
	    root_ptr[0] = '\0';
	    chdir(single_file_path);

        /* Put the slash back in the name */
        root_ptr[0] = '/';
	    ++root_ptr;
        strcpy2(command, "gtar zcf - ", root_ptr);
	    strcpy(single_file, root_ptr);

	    return(dir_num);
    } /* end else */
} /* end single_file_command_build */


/* Retrieves data from client and uncompresses and untars the data */
long long command_put_compressed_dir(char *keyword)
{
    extern char root_dir[];
    char command[64];
    char dir_name[32];
    long long filesize = 0;
    FILE *po;
    int ret;

    DBG("keyword='%s'", keyword);

    /* First must make a directory where the data can reside */
    sprintf(dir_name, "data_files%d/%s.gz", fs.disk_index, keyword);

    if (mkdir(dir_name, 02775) != 0)
        close_down2d(mkdir_err, errno, dir_name, MKDIR_ERR);
    
    chdir (dir_name);
    
    /* Build command to execute */
    strcpy(command, "gtar zxf -");
    
    /* Open command to pipe data from client through */
    if ((po = popen(command, "w")) == NULL)
        close_down1m(wr_open_err, command,  WR_OPEN_ERR);

    /* Tell client ready to get text */
    put_short(SUCCESS);
    while ((ret = get_string(bf.buffer)) > 0)
    {
        ret = fwrite(bf.buffer, 1, ret, po);
	    filesize += ret;
    }

    /* close pipe just wrote to */
    pclose(po);

    /* Get final size */
    fs.compressed_size = get_longlong();

    /* Setting here because client does not know the original size */
    fs.original_size = gu.original_size;
    DBG("fs.original_size='%d', gu.original_size='%s'", fs.original_size, gu.original_size);

    /* Change back to the server directory */
    chdir(root_dir);

    return(fs.compressed_size);
} /* End command_put_compressed_dir */


/* Retrieve compressed file and do not uncompress it as it is written to disk */
long long command_put_compressed_file(char *keyword)
{
    char file_name[FILE_SIZE];
    long long filesize = 0;
    int fd;
    int ret;

    DBG("keyword='%s'", keyword);

    sprintf(file_name, "data_files%d/%s.gz", fs.disk_index, keyword);
    if (mkdir(file_name, 02775) != 0)
        close_down2d(mkdir_err, errno, file_name, MKDIR_ERR);

    /* Now add on the file name */
    strcpy2(file_name+strlen(file_name), "/", gu.key1);
    if ((fd = open(file_name, O_WRONLY | O_CREAT, 0770)) == NULL)
        close_down1m(wr_open_err, file_name,  WR_OPEN_ERR);

    /* Tell client ready to get text */
    put_short(SUCCESS);

    /* Retrieve the data */
    while ((ret = get_string(bf.buffer)) > 0)
    {
	    ret = write(fd, bf.buffer, ret);
	    filesize += ret;
    } /* end while */

    /* Get final size */
    fs.original_size = get_longlong();
    DBG("fs.original_size='%d'", fs.original_size);
    
    /* Close file just created */
    close(fd);
    
    return(fs.original_size);
} /* End command_put_compressed_file */


/* Retrieve a compressed file and uncompress as written to disk */
long long command_put_uncompressed_file(char *keyword)
{
    char file_name[32];
    gzFile file;
    long long filesize = 0;
    FILE *fp;

    DBG("keyword='%s'", keyword);

    sprintf(file_name, "data_files%d/%s.gz", fs.disk_index, keyword);
    if (mkdir(file_name, 02775) != 0)
        close_down2d(mkdir_err, errno, file_name, MKDIR_ERR);

    /* Now add on the file name */
    strcpy2(file_name+strlen(file_name), "/", gu.key1);
    if ((fp = fopen(file_name, "w")) == NULL)
        close_down1m(wr_open_err, file_name,  WR_OPEN_ERR);

    /* Tell client ready to get text */
    put_short(SUCCESS);
    if ((file = trans_open("r", filesize, 0, DIRECTORY)) == NULL)
    {
        strcpy(rtn.msg, gz_sockopen_err);
        close(sock);

        return(GZ_SOCKOPEN_ERR);
    }

    /* for this transaction original size and compressed size are the same */
    fs.original_size = block_retrieve(file, fp, UNSPECIFIED, file_name, DBNO);
    DBG("fs.original_size='%d'", fs.original_size);

    /* Write any remaining output to file */
    trans_close(file, 0);

    /* Get final fs.original_size */
    fs.original_size = get_longlong();
    DBG("fs.original_size='%d'", fs.original_size);

    /* Close file just created */
    fclose(fp);

    return(fs.original_size);
} /* End command_put_compressed_file */


/* Sends directory to the client and tars and compresses the data as it is sent */
short command_get_compressed_dir(void)
{
    DBG();

    extern char root_dir[];
    int len;
    int dir_num;
    short ret;
    long long size ;
    FILE *po;
    char buffer[BUFFER_SIZE];
    char single_file[FILE_SIZE];
    char command[80];
    char dir_name[32];

    /* Build directory name */
    sprintf(dir_name, "data_files%d/%s.gz", fs.disk_index, fs.key);
    ret = chdir(dir_name);

    /* If file specified, use it also */
    if (strcmp(gu.par2, NONE) == 0)
    {
	    /* Indicate no file present */
	    strcpy(single_file, NONE);

	    /* build command to execute */
	    strcpy2(command, "gtar zcf - ", fs.real_name);
    } else dir_num = single_file_command_build(gu.par2, single_file, command);

    /* Execute command and get output from command */
    if ((po = popen(command, "r")) == NULL)
        close_down2d(command_open_err, errno, command, COMMAND_OPEN_ERR);

    /* Tell client ready to send data */
    put_short(SUCCESS);
    file_string_build(single_file,buffer);
    put_string(buffer);
    
    /* Get acknowledgement */
    /* If unsuccessful, log failure, clean-up and exit */
    if ((ret = get_short()) != SUCCESS)
        shut_down(bad_ack, BAD_ACK);

    /* Read text from the pipe and write it to the socket */
    /* send the file data itself */
    /* Write out data for as long as can read it in */
    while ( (len = fread(buffer, 1, BLOCK_SIZE, po)) > 0)
        put_binary_string(buffer, len);

    /* Close local file */
    pclose(po);

    /* Tell client all data sent */
    put_short(SUCCESS);

    /* change back to the server directory */
    chdir(root_dir);
    
    return(size);
} /* end command_get_compressed_dir */


/* Sends a file which is in a directory to the client */
short command_get_compressed_file(void)
{
    DBG();

    int len;
    short ret;
    long long size  = 0;
    FILE *fp;
    char buffer[BUFFER_SIZE];
    char file_name[32];

    /* Build the file name */
    sprintf(file_name, "data_files%d/%s.gz/%s", fs.disk_index, fs.key, fs.real_name);

    /* Open the file */
    /* If this is 7.0 or later, simply open the file */
    /* Otherwise pipe it through gzip */
    if ((fp = fopen(file_name, "r")) == NULL)
        close_down1m(rd_open_err, file_name, RD_OPEN_ERR);

    /* Tell client ready to send data */
    put_short(SUCCESS);
    file_string_build(NONE, buffer);
    put_string(buffer);
    
    /* Get acknowledgement */
    /* If unsuccessful, log failure, clean-up and exit */
    if ((ret = get_short()) != SUCCESS)
        shut_down(bad_ack, BAD_ACK);

    /* Read text from the file and write it to the socket */
    /* Now send the file data itself */
    /* Write out data for as long as can read it in */
    while ( (len = fread(buffer, 1, BLOCK_SIZE, fp)) > 0)
    {
	    size += len;
	    put_binary_string(buffer, len);
    }

    /* Close local file */
    fclose(fp);

    /* Tell client all data sent */
    put_short(SUCCESS);

    return(size);
} /* end command_get_compressed_file */


long long file_put(char *dir, char *file)
{
    DBG("dir='%s', file='%s'", dir, file);

    short ret;
    long long total_size = 0;
    short fd;
    char full_file_name[FILE_SIZE];
    char *file_ptr;

    file_ptr = full_file_name + strcpy2(full_file_name, dir, "/");
    
    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(dir, 0) != 0)
        dir_create(dir);

    /* First get file name */
    get_string(file_ptr);

    /* Open file for writing */
    if ((fd = open(full_file_name, O_WRONLY|O_CREAT, 0600)) == EOF)
        close_down1m(wr_open_err, full_file_name,  WR_OPEN_ERR);

    /* Tell client ready to get text */
    put_short(SUCCESS);
    
    /* Read in blocks of text and write them out to the file */
    /* Get strings until 0 length string sent */
    while ((ret = get_string(bf.buffer)) > 0)
    {
        /* Write the text out to the file */
        if (write(fd, bf.buffer, ret) != ret)
            close_down1m(write_err, full_file_name, WRITE_ERR);
    
        total_size += ret;
    } /* end while */

    /* Close the file just created */
    close(fd);

    /* Copy file name into file variable */
    strcpy(file, file_ptr);
    
    return(total_size);
} /* End file_put */


/* Get data from client and pipe it through a command */
long long command_put(char *command)
{
    DBG("command='%s'", command);

    long long size = 0;
    short ret;
    FILE *po;
    short get_string();

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
        if (fwrite(bf.buffer, ret, 1, po) < 1)
            close_down1m(write_err, command, WRITE_ERR);

        size += ret;
    }

    /* Close pipe just wrote to */
    if (pclose(po) != 0)
    {
	    char msg[80];
	    sprintf(msg, pclose_err, errno);
	    email_send("", admin.err_email, "RETTrans tar error", msg);
    }

    return(size);
} /* End command_put */


/* Just like file get except do not pass file name */
int data_put(char *dir, char *file)
{
    DBG("dir='%s', file='%s'", dir, file);

    char fnd = DBNO;
    short ret;
    short fd;
    short get_string();
    char full_file_name[FILE_SIZE];

    /* Get indication that client ready to transfer data */
    if ((ret = get_short()) != SUCCESS)
        return(ret);

    strcpy3(full_file_name, dir, "/", file);
    
    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(dir, 0) != 0)
        dir_create(dir);

    /* Open file for writing */
    if ((fd = open(full_file_name, O_WRONLY|O_CREAT, 0775)) == EOF)
        close_down1m(wr_open_err, full_file_name,  WR_OPEN_ERR);

    /* Tell client ready to get text */
    put_short(SUCCESS);

    /* Read in blocks of text and write them out to the file */
    /* Read strings from socket until 0 length string sent */
    while ((ret = get_string(bf.buffer)) > 0)
    {
        /* Write the text out to the file */
        if (write(fd, bf.buffer, ret) != ret)
            close_down1m(write_err, full_file_name, WRITE_ERR);

        fnd = DBYES;
    } /* end while */

    /* Close the file just created */
    close(fd);
    
    if (fnd == DBYES) return(SUCCESS);
    else return(FAILURE);
} /* End data_put */


int files_put(char *dir, char *files[])
{
    DBG();

    char full_file_name[FILE_SIZE];
    extern TMP_BUFFER bf;
    short ret;
    int cnt = 0;
    short fd;
    char *file_ptr;        /* Points to end of path name */
    short get_string();

    /* Build directory name where files will be placed */
    file_ptr = full_file_name + strcpy2(full_file_name, dir, "/");

    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(dir, 0) != 0)
        dir_create(dir);

    /* Retrieve the files in a while loop */
    while (get_string(file_ptr) > 0)
    {
        /* Open file for writing */
        if ((fd = open(full_file_name, O_WRONLY|O_CREAT, 0600)) == EOF)
            close_down1m(wr_open_err, full_file_name,  WR_OPEN_ERR);

        /* Tell client ready to get text */
        put_short(SUCCESS);

        /* Read in blocks of text and write them out to the file */
        while((ret = get_string(bf.buffer)) > 0)
        {
            /* Write the text out to the file */
            if (write(fd, bf.buffer, ret) != ret)
                close_down1m(write_err, full_file_name, WRITE_ERR);
        } /* End while */

        /* Close the file just created */
        close(fd);

        /* Allocate space for file name */
        files[cnt] = malloc(strlen(file_ptr) +1);
        
        /* Copy file name into file array */
        strcpy(files[cnt++], file_ptr);
    } /* end while */

    files[cnt] = NULL;

    /* Restore file name to original state */
    file_ptr[0] = '\0';

    return(cnt);
} /* end files_put */


/* Get output from a command */
long long command_get(void)
{
    DBG();

    char command[FILE_SIZE];
    char buffer[BUFFER_SIZE];
    short ret;
    long long size = 0;
    FILE *po;

    /* Build the command */
    sprintf(command, "gzip -c data_files%d/%s.gz/%s", fs.disk_index, fs.key, fs.real_name);

    /* Open the command */
    if ((po = popen(command, "r")) == NULL)
        shut_down(no_data_err, NO_DATA_ERR);

    /* Tell client ready to send data */
    put_short(SUCCESS);
    file_string_build(NONE, buffer);
    put_string(buffer);

    /* Get acknowledgement */
    /* If unsuccessful, log failure, clean-up and exit */
    if ((ret = get_short()) != SUCCESS)
        shut_down(bad_ack, BAD_ACK);

    /* Read text from the file and write it to the socket */
    /* Now send the file data itself */
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


void command_get_line(char *command)
{
    DBG("command='%s'", command);

    short ret;
    FILE *po;

    /* Open pipe for output from command */
    if ((po = popen(command, "r")) == NULL)
        shut_down(no_data_err, NO_DATA_ERR);

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


void file_data_get(char *file_data, char *file_name)
{
    DBG("file_data='%s', file_name='%s'", file_data, file_name);

    /* Send file name */
     put_string(file_name);

    /* Get ack from client */
    if (get_short() != SUCCESS)
        shut_down(bad_ack, BAD_ACK);

    /* Send file data */
    put_string(file_data);

    /* Indicate all file data sent */
    put_short(SUCCESS);
} /* end file_data_get */
/*ts.files.fileuse*/
