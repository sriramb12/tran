
/****************************************************************************
 *
 * Function:	ftp functions
 *
 * Description:	Routines used to send data via FTP
 *		To very block size, change for the block size and the loop count
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ftp_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <wait.h>

#include "close_down.h"
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_server_globals.h"
#include "trans_server_errs.h"
#include "trans_server_macros.h"
#include "reg_globals.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"
#include "trans_defines.h"

#include "global_debug.h"

#include <curl.h>

#define parallel_num 10

extern COMPANY_DATA cp;
    extern TRANS_FILE_STAT fs;
FTP_TRANSFER_PARAMS ft;
    long long local_size;
    extern char email_buffer[];
    extern char global_mask[];


void handle_SIGCHLD_to_avoid_errno_10_ECHILD(int signum)
{
    DBG("signum=%d", signum);
}

void ftp_mark_encryption_complete(void)
{
    FILE *fp;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];

    DBG();

    fp = read_big_file(fs.key, lines, &fs);
    sscanf(lines[0], ICOM_EXTEND_FORMAT,
    &fs.file_type, &fs.disk_index, &fs.dlt_time, fs.own_id, &fs.email_flag,
    &fs.extension, &fs.dlt_flag, &fs.lock_flag, &fs.mode, &fs.compression, global_mask, fs.tar_type);
    /* Encryption status stored in global mask */
    strcpy(global_mask, "Encryption-complete");
    sprintf(lines[0], ICOM_EXTEND_FORMAT,
    fs.file_type, fs.disk_index, fs.dlt_time, fs.own_id, fs.email_flag,
    fs.extension, fs.dlt_flag, fs.lock_flag, fs.mode, fs.compression, global_mask, fs.tar_type);
    DBG("Before call write_big_file()");
    write_big_file(fp, lines, &fs);
    DBG("after call write_big_file()");
} /* end ftp_mark_encryption_complete */

/* Build message to send to user */
void ftp_parallel_email_build(char *checksum, char *remote_file, char *remote_dir, char *extra_data, time_t ftp_start_time)
{
extern COMPANY_DATA cp; 
    char *file_type_ptr;

    DBG();

    if (fs.file_type == 'd')
    file_type_ptr = "Server directory name";
    else file_type_ptr = "Server file name";
    /* Start time calculated in ftp_send */
    /* Calculate the FTP transfer time */
    elapsed_time_calc(ftp_start_time, ft.remote_transfer_time, ft.remote_finish_time);
    int len = sprintf(email_buffer, "Data transfer to %s:\n\n\
    %s%s:  %s\n\
    Cksum for local zipped data:  %s\n\
    Remote file name:  %s\n\
    Remote directory:  %s%s\n\
    Remote login:  %s\n\
    Remote machine:  %s (%s)\n\
Block size:  %d\n\
    Start date:  %s\
    Local transfer time:  %s\n\
    Local transfer date:  %s\n",
cp.name, extra_data, file_type_ptr, fs.real_name, checksum, remote_file, cp.root_dir, remote_dir, 
    cp.login, cp.machine, cp.address,
    FTP_PARALLEL_SIZE, ft.start_time, ft.local_transfer_time, ft.local_finish_time) ;
    sprintf( email_buffer+len, "FTP transfer time:   %s\n\
    FTP transfer date:   %s\n\
    Original size:       %llu bytes\n\
    Compressed size:    %llu bytes\n\
    Encrypted size:      %llu bytes\n\
    Transfer size:      %llu bytes\n\
    Software version:  %s\n",
    ft.remote_transfer_time, ft.remote_finish_time, 
    fs.original_size, fs.compressed_size, ft.encrypt_size, ft.transfer_size, TRANS_VERSION);
} /* End ftp_parallel_email_build */


/* check if the file is already encypted with the cp.encrypt_key */
/* the return code from gpg command is saved into the outfile, it is done because it is hard to get system return code */
static int  is_file_encrypted(const char *file)
{
     int status = 0;
     int ret    = 0;  // default returns false
     struct stat gpg;
     FILE *fd = NULL;
     char outfile[256];
     char command[1024];
     sprintf (outfile, "gpg_%s_gpg_test.txt", fs.key);
     errno = 0;
     status = sprintf(command, "gpg --batch --list-only %s/%s > %s 2>&1 ; echo $? > %s", get_current_dir_name(), file, tr.log_file, outfile);
     DBG("command=%s length=%d", command, status);
     status  = system(command);
     DBG("status=%d errno=%d", status,  errno);
     if ((fd = fopen(outfile, "r")) != NULL)
     {
         if (fgets(command, sizeof(command) -1, fd) != NULL && strcmp(command, "0\n") == 0)
         {
                 ret = 1;
         }
         fclose(fd);
#if !defined(DEBUG)
         unlink(outfile);
#endif
     }
     DBG("ret=%d", ret);
     return ret;
}



/* This is called by the parallel transfer */
long long encrypt_data(char *local_file, char *real_file, char *remote_file, char *remote_dir, char *history_file, char *extra_data, long *encrypt_time)
{
    char cksum[128];
     char *time_ptr;
    char *ptr;
    char *file_type_ptr;
    struct stat buf;
    char command[FILE_SIZE];
    char message[BUFFER_SIZE];
     int status = 0;


    if (fs.file_type == 'd')
    file_type_ptr = "Local directory name";
    else file_type_ptr = "Local file name";
    /* Calculate the elapsed time */
    time_ptr = ctime(&tr.upd_time);
    strcpy(ft.start_time, time_ptr);
    cksum_build(cksum, local_file);
    /* Update the history file */
    sprintf(email_buffer, "Data transfer to %s:\n\n\
    %s%s:           %s\n\
    Cksum for local zipped data:  %s\n\
    Remote file name:           %s\n\
    Remote directory:    %s%s\n\
    Remote login:  %s\n\
    Remote machine:  %s (%s)\n\
    Start Date:          %s\
    Local transfer time:  %s\n\
    Local transfer date:  %s",
    cp.name, extra_data, file_type_ptr, real_file, cksum, remote_file, cp.root_dir, remote_dir,
    cp.login, cp.machine, cp.address,
    ft.start_time, ft.local_transfer_time, ft.local_finish_time);
    data_log(history_file, email_buffer);
    /* First encrypt the data */
    /* If encryption key set to none, do not encrypt the data */
    if (strcasecmp(cp.encrypt_key, "none") == 0)
    {
    return(0);
    }

    signal(SIGCHLD, SIG_IGN);

    /* Otherwise perform encryption */
	/* Update the status in the info file */
	ftp_status_update(fs.key, "Status:      Encrypting data");
    
    
    DBG("ft.encrypt_file=%s access=%d", ft.encrypt_file, access(ft.encrypt_file, R_OK));
    DBG("real_file=%s access=%d", real_file, access(real_file, R_OK));
    DBG("local_file=%s, access=%d", local_file, access(local_file, R_OK));

    status = 0;
    errno  = 0;

    // make sure original file is NOT already encrypted
    if (is_file_encrypted(local_file) == 0)
    {
            sprintf(command, "gpg --output \"%s\" --batch --recipient %s --encrypt \"%s\" >>encrypt_%s 2>&1",
            ft.encrypt_file, cp.encrypt_key, local_file, tr.log_file);
            FORCE_DBG("gpg command=%s", command);
            DBG_WAIT_LONGER();
        // when DEVEL_USING_SINGLE_PROCESS is defined, fork() is supposed to be redefined as FAKE_fork()
        if (fork() == 0)
        {
            int localstatus = 0;
            signal(SIGCHLD, handle_SIGCHLD_to_avoid_errno_10_ECHILD);
            usleep(500);    //give some time to wait() of the parent process
            FORCE_DBG("starting gpg...");
            localstatus = system(command);
            //lets force it to wait
            FORCE_DBG("gpg finished in subprocess with code = %d", localstatus);
            sleep(3);   // give some time to buffering...
    #if !defined(DEVEL_USING_SINGLE_PROCESS)
            exit(localstatus);
    #endif
        }
        else
        {
            // Carlos Mazieri:
            // FIXME, it does not work, wait() blocks, but result is not right
            // status is always 0 does not matter gpg returns
            wait(&status);
            /*
            * it should be something like
            *
            if (WIFEXITED(status))
            {
                status = WEXITSTATUS(status);
            }
            */
            if (errno == ECHILD)
            {
                errno = 0;
            }
        }  // end else fork()
    } //end it is necessary to encrypt the file
    else
    {
        link(local_file, ft.encrypt_file);
    }

#if defined(DEBUG) || defined(FORCE_DEBUG)
    {
        struct stat gpg;
        int  stat_return = stat(ft.encrypt_file, &gpg);
        char cwd[1024] = {0};
        getcwd(cwd, sizeof(cwd));
        DBG("cwd=%s, ft.encrypt_file=%s", cwd, ft.encrypt_file);
        DBG("return from gpg = %d, return from stat = %d errno=%d size = %lld", status, stat_return, errno, gpg.st_size );
    }
#endif


    buf.st_size = 0;

    if (status != 0 && errno != 0)
    {      
        sprintf(message, "Encryption failed, OS errno=%d message=%s\n", errno, strerror(errno));
        DBG("%s" , message);
    }

    buf.st_size = 0;

    /* Do a stat on the file to get file size */
    if (status != 0 || stat(ft.encrypt_file, &buf) != 0 || buf.st_size == 0)
    {
        DBG("encyption failed %llu", buf.st_size);
        ftp_status_update(fs.key, "Error:       Encryption failed");
        sprintf(rtn.msg, encrypt_err);
        email_send_user(gu.email1, "Encryption failed, data transfer aborted", rtn.msg, -1);
        close_down(rtn.msg, ENCRYPTION_ERR);
    } /* End if */
    else ftp_mark_encryption_complete();
    /* Return encrypted size to calling function */
    return(buf.st_size);
} /* end encrypt_data */

/* Check the encryption status from the global file */
void encrypt_status_check(void)
{
    extern TRANS_FILE_STAT fs;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    FILE *fp;

    fp = read_big_file(fs.key, lines, &fs);
    fclose(fp);
    /* Status is stored in global_mask */
    sscanf(lines[0], ICOM_EXTEND_FORMAT,
    &fs.file_type, &fs.disk_index, &fs.dlt_time, fs.own_id, &fs.email_flag,
    &fs.extension, &fs.dlt_flag, &fs.lock_flag, &fs.mode, &fs.compression, global_mask, fs.tar_type);
    if ((strcmp(global_mask,  "Encryption-complete")== 0)
    || (strncmp(global_mask, "Error", 5) == 0))
    ft.encryption_done = DBYES;
    else ft.encryption_done = DBNO;

    DBG("ft.encryption_done=%d", ft.encryption_done);

} /* end encrypt_status_check */

/* monitor encryption of data */
/* checks to see if encryption is completed */
/* also updates the encrypted size and the number of blocks for the file */
int encrypt_size_calc(void)
{
    int total_blocks = 0;
    struct stat buf;
    /* first see if encryption has completed */
	encrypt_status_check();
    /* Next calculate the size for the file and the number of blocks to send */
	if (stat(ft.encrypt_file, &buf) != 0) 
    {
	total_blocks = 0;
	ft.encrypt_size = 0;
    } else {
	ft.encrypt_size = buf.st_size;
    total_blocks = ((ft.encrypt_size-ft.first_block_offset)/ft.local_block_size) +ft.adjusted_block_num;
    } /* end else */
    /* Return numnber of blocks to send */

    FORCE_DBG("total_blocks=%d", total_blocks);
    return(total_blocks);
} /* end encrypt_size_calc */

/* Calls software to encrypt the data */
/* if under 1 Gb just waits for encryption to complete */
/* Otherwise spawns off process to encrypt data */
int process_encrypt(char *local_file, char *remote_file, char *remote_dir, char *history_file, char *extra_data,  long *encrypt_time)
{
    int total_blocks = 0;
    char message[BUFFER_SIZE];
    char current_time[128];

    DBG();

    /* If the data less than 600 Mb in size, simply encrypt it before transferring it */
    if (fs.compressed_size < FTP_PARALLEL_SIZE*30)
    {
        DBG("doing serial size=%llu",fs.compressed_size );
        long tmp_time = time(0);
        data_log(tr.log_file, "Encrypting data before transfer");
        ft.encrypt_size = encrypt_data(local_file, fs.real_name, remote_file, remote_dir, history_file, extra_data, encrypt_time);
        return(DBYES);
    } /* end if */

    /*
     * Carlos Mazieri  Aug 2014 Disabling encryption in parallel
     *
     *     After facing some bad Send2Foundry transfers it was decided to
     *       DISABLE the "encryption in parallel with the transfer".
     *     Starting the transfer before the encryption be done is dangerous and it
     *     can cause wrong number of blocks calculation for parallel transfers.
     *
     *     To revert this change to original code just revert next  "#if 0" to "#if 1"
     */
#if 0
    /* Otherwise begin the encryption in parallel */
    switch(fork())
    {
    case 0:
    {
        /* This is the child which will encrypt the data */
        data_log(tr.log_file, "Encrypting data as it is being transferred");
        ft.encrypt_size = encrypt_data(local_file, fs.real_name, remote_file, remote_dir, history_file, extra_data, encrypt_time);
        exit(0);
    } /* End child */
    default:
    {
        /* Sleep until file at least 30 blocks or 600 Mb in size */
        /* Indicate no encryption has been done */
        ft.encrypt_size = 0;
        ft.encryption_done = DBNO;
        while ((ft.encrypt_size < FTP_PARALLEL_SIZE*30)
               && (ft.encryption_done == DBNO))
        {
            total_blocks = encrypt_size_calc();
            sleep(5);
        } /* End while */
    } /* end default */
    } /* end switch */
#else     
    ft.encrypt_size = encrypt_data(local_file, fs.real_name, remote_file, remote_dir, history_file, extra_data, encrypt_time);
    ft.encrypt_size = 0;    
    total_blocks = encrypt_size_calc();
    ft.encryption_done = DBYES;
    DBG("local_file=%s remote_file=%s remote_dir=%s extra_data=%s", local_file, remote_file, remote_dir, extra_data ? extra_data : "null");
#endif   
    FORCE_DBG("encrypt_size=%llu total_blocks=%d", ft.encrypt_size, total_blocks);
    return(ft.encryption_done);
} /* end process_encrypt */

int put_block(
    int block_cnt,
    unsigned long long local_block_size,
    char *local_file,
    char *new_remote_file,
    char *remote_dir,
    char *status_file,
    unsigned long long offset)
{
    char elapsed_time[32];
    char time_string[32];
    long current_time = time(0);
    int cnt;
    int ret = 0;

    DBG("writing block=%d", block_cnt);

    /* Try putting the block 3 times */
    for (cnt = 0; cnt < 3; ++cnt)
    {
        ret = put_parallel_block(ft.encrypt_file, new_remote_file, remote_dir, offset, local_block_size, block_cnt);
        if (ret == SUCCESS)
        {
            elapsed_time_calc(current_time, elapsed_time, time_string);
            sprintf( rtn.msg, "block %d size %llu offset %lld  count %d. time %s file %s",
                     block_cnt, local_block_size, offset, cnt, elapsed_time, new_remote_file);
            data_log(status_file, rtn.msg);
            ftp_status_update(fs.key, rtn.msg);
            break;
        } /* End if */
        else if (cnt == 2)
        {
            DBG("block failed block_cnt=%d", block_cnt);
            sprintf( rtn.msg, "Failed to put data for block %d with offset %lld  with count %d", block_cnt, offset, cnt);
            data_log(status_file, rtn.msg);
        } /* End else */
    } /* End for */
    return ret;
} /* end put_block */

/* Get rid of the leading slashes */
void leading_slashes_remove(char *remote_dir)
{
    int cnt;
    char tmp_dir[PATH_SIZE];
    int len;
    len = strlen(remote_dir);
    if (len <= 1) return;
    for (cnt = 0; cnt < len; ++cnt)
    if (remote_dir[cnt] != '/') break;
    strcpy(tmp_dir, remote_dir+cnt);
    strcpy(remote_dir, tmp_dir);
} /* end leading_slashes_remove */

/* Transfer script to decrypt and combine files on FTP server */
int ftp_put_finale_script(int parallel, char *remote_file, char *remote_dir, char *remote_data_dir,
    char *cksum)
{
    int len;
    char finale_buffer[BIG_BLOCK_SIZE];
    char email_addr[80];
    char local_file[FILE_SIZE];
    FILE *fp;

    /* Open up the local copy of the finale script */
    strcpy3(local_file, "history_data/", fs.key, ".finale");
    fp = fopen(local_file, "w");
    /* Build the message to send */
    /* Build the email address string */
    strcpy2(email_addr,"chandrag@charteredsemi.com ", gu.email1);
    len = sprintf( finale_buffer, FINALE_FILE_MSG, ft.encrypt_size, cksum, email_addr);
    fprintf(fp, finale_buffer);
    /* Close the file */
    fclose(fp);
    /* Transfer the final script to the foundry */
    put_finale_file(local_file, remote_file, remote_dir);
} /* end ftp_put_finale_script */

/* Increment the remote file name */
void remote_file_increment(char *new_remote_file)
{
    ++new_remote_file[1];
/* If got past 'z', will need to put back to 'a' */
	if (new_remote_file[1] == 123)
	{
	    new_remote_file[1] = 'a';
	    ++new_remote_file[0];
	} /* End if for past 'z' */
} /* end remote_file_increment */

/* Get the total size transferred from the status file */
unsigned long long ftp_calc_parallel_size(void)
{
    unsigned long long transferred_size;
    unsigned long long total_size = 0;
    char status_file[64];
    FILE *fp;
    char line[260];

    /* build status file name */
    strcpy3(status_file, "history_data/", fs.key, ".status");
    /* Open the status file */
    if ((fp = fopen(status_file, "r")) == NULL) return(0);
    /* Read through status file adding size to total size */
    while (line_get(fp, line) != EOF)
    {
	sscanf(line, "%*s %*s %*s %llu", &transferred_size);
	total_size += transferred_size;
    }
    /* close the status file */
    fclose(fp);
    return(total_size);
} /* end ftp_calc_parallel_size */


int put_intermediate_block(int block_cnt, char *local_file, char *remote_file, char *remote_dir, 	
    unsigned long long offset, unsigned long long total_size)
{
    int cnt;
    int ret;

    for (cnt = 0; cnt < 3; ++cnt)
    {
	    if ((ret = put_parallel_block(ft.encrypt_file, remote_file, remote_dir, 
	offset, total_size, block_cnt)) == SUCCESS)
	break;
    }
    return(ret);
} /* End put_intermediate_block */

/* Send groups of blocks for the file */
void fork_put_block(
	    int block_cnt,
    char *local_file,
    char *new_remote_file,
    char *remote_dir,
    char *status_file,
    unsigned long long offset)
{
    FILE *po;
    int num_processes = 0;

	switch(fork())
	{
	    case 0: 
	{
	    /* Put the block */
	put_block(block_cnt, ft.local_block_size, local_file, new_remote_file, remote_dir, status_file, offset);
#if !defined(DEVEL_USING_SINGLE_PROCESS)
	    exit(0);
#endif
	} /* End child */
	    /* this is the parent which will spawn off more children */
	    default: 
	{
	    /* See if all data has been sent */
	    /* If have not reached the number of parallel processes, just go on */
	    /* this will get 10 processes running right away */
	if (block_cnt < (parallel_num-1)) return;
    /* Check to see if more processes are needed */
	    while (1)
	    {
		po = popen("ps -ef | grep ssl.tftpd | wc -l", "r");
		fscanf(po, "%d", &num_processes);
		pclose(po);
	    if (num_processes <= parallel_num+3) break;
		else sleep(10);
	    } /* End while */
	} /* End default */
	} /* end switch */
} /* end fork_put_block */

void ftp_put_parallel_blocks
(
    char *local_file,
    char *new_remote_file,
    char *remote_dir,
    char *status_file,
    int total_blocks)
{
    int estimated_blocks;
    int last_block = DBNO;
    unsigned long long total_size = ft.encrypt_size;
    unsigned long long offset = 500L;
    int send_cnt;
    int cnt;
    FILE *po;
    char command[80];
    int block_cnt = 0;
    int increment = DBYES;

    DBG_WAIT_LONGER();

    /* Initialize the block size */
    ft.local_block_size = FTP_PARALLEL_SIZE;
    /* Estimate the number of blocks to send */
    if (ft.encryption_done == DBYES)
    estimated_blocks = total_blocks;
    else estimated_blocks = fs.compressed_size/ft.local_block_size;

    FORCE_DBG("estimated_blocks=%d total_blocks=%d", estimated_blocks, total_blocks);

    /* Build command to execute later */
    strcpy2(command, "wc -l ", status_file);
    /* Send the first block before other blocks so directories are made */
	put_block(block_cnt, 500, local_file, new_remote_file, remote_dir, status_file, 0);
    /* Send data until all blocks sent */

    DBG("estimated_blocks=%d block_cnt=%d", estimated_blocks, block_cnt);
    while (block_cnt <= estimated_blocks)
    {
        if (increment == DBYES)
        {
            ++block_cnt;
            /* Increment the remote file name */
            remote_file_increment(new_remote_file);
            /* Reset the offset */
            /* Not done if this is block 1 */
            if (block_cnt > 1)
                offset += (unsigned long long)ft.local_block_size;
            /* Indicate increment just performed */
            increment = DBNO;
        } /* end if for increment needed */
       /* If this is the last block, calculate the file size */
        FORCE_DBG("ft.encryption_done=%d block_cnt=%d (total_blocks-1)=%d", ft.encryption_done,block_cnt, total_blocks-1);
        if (block_cnt >= (total_blocks-1))
        {
            /* If encryption has completed, prepare to send last block */
            if (ft.encryption_done == DBYES)
            {
                last_block = DBYES;
                ft.local_block_size = ft.encrypt_size-offset;
                DBG("TRANSFER completed,ft.local_block_size=%llu offset=%llu", ft.local_block_size, offset);
            } else
            {
                DBG("TRANSFER Not completed yet");
                sleep(30);
            }
        } /* end if for last block */
        if ((last_block == DBYES)
                || (offset < (ft.encrypt_size +ft.local_block_size)))
        {
            fork_put_block(block_cnt, local_file, new_remote_file, remote_dir, status_file, offset);
            /* Indicate name, block size, etc need to be incremented */
            increment = DBYES;
        } else {
            sleep(30);
        } /* end else */
	/* If just sent the last block, fall out of loop */
	if (last_block == DBYES) break;
	/* See if more blocks to send are present */
	if (ft.encryption_done == DBNO)
    {
	total_blocks = encrypt_size_calc();
	/* If encryption completed, adjust number of blocks to send */
	if (ft.encryption_done == DBYES) estimated_blocks = total_blocks;
    } /* end if */
    } /* End while */
    /* Do not leave until all blocks have been sent */
    /* Verify all blocks have been sent */
    data_log(tr.log_file, "Verifying all blocks have been sent");
    for (cnt = 0; cnt < 90; ++cnt)
    {
	sleep(10);
	po = popen(command, "r");
	fscanf(po, "%d", &send_cnt);
	pclose(po);
    if (send_cnt == total_blocks) break;
    } /* end while */
    /* If after 5 minutes send cnt less than total blocks then see what data not sent */
    if (send_cnt < total_blocks) 
    {
    data_log(tr.log_file, "Attempting to resend missing blocks");
	 put_resend_block(block_cnt, local_file, remote_dir, total_blocks);
    /* Need to wait until the rest of the blocks have been sent */
	/* Try waiting 10 minutes */
    for (cnt = 0; cnt < 90; ++cnt)
    {
	sleep(10);
	po = popen(command, "r");
	fscanf(po, "%d", &send_cnt);
	pclose(po);
    if (send_cnt == total_blocks) break;
    }
    } /* End if for not all blocks sent */
    sprintf(rtn.msg, "Total blocks:  %d, sent blocks:  %d",
	 total_blocks, send_cnt);
	data_log(tr.log_file, rtn.msg);
} /* End ftp_put_parallel_blocks */

/* Put serial data in blocks */
void ftp_put_serial_blocks
(
    char *local_file,
    char *remote_file,
    char *remote_dir,
    char *status_file,
    int total_blocks)
{
    int ret;
    int block_cnt = 0;
    unsigned long long offset = 0L;
    char elapsed_time[32];
    char time_string[32];
    	time_t current_time = time(0);
    FILE *fp;
    size_t  len = 0;

    FORCE_DBG("local_file=%s total_blocks=%d", local_file, total_blocks);

    /* Open the encrypted file in preparation for transferring the dta */
    if ((fp = fopen(ft.encrypt_file, "r")) == NULL)
    close_down1f(rd_open_err, ft.encrypt_file, RD_OPEN_ERR);
    ftp_status_update(fs.key, "Performing serial FTP transfer");
    ft.local_block_size = FTP_SERIAL_SIZE;
    /* Send data until all blocks sent */

    ft.last_read_return = 100; // just to start it
    DBG("ft.transfer_size=%llu", ft.transfer_size);
    /*
     *  Oct 29 2014  Carlos Mazieri
     *
     * in fact this attempt to save blocks doing appending does not work
     * the control must done in serial_read_callback() returning 0 when a block has finishes
     *
     * in fact the first block reads the whole file
    */
    for (block_cnt = 0; ft.last_read_return != 0 && block_cnt < total_blocks ; ++block_cnt)
    {
        /* Calculate the current time */
        current_time = time(0);
        /* If this is the last block, calculate the file size */
        if (block_cnt == (total_blocks-1))
        {
            /* If encryption has completed, prepare to send last block */
            if (ft.encryption_done == DBYES)
            {
                ft.local_block_size = ft.encrypt_size-offset;
                /* Set the final transfer size */
                ft.transfer_size = ft.encrypt_size;
            }
            else sleep(20);
        } /* end if for last block */
        /* Update meta data with amount transferred */
        sprintf(rtn.msg, "Status:      Transferred %llu bytes", offset);
        ftp_status_update(fs.key, rtn.msg);

        ret = put_serial_block(ft.encrypt_file, remote_file, remote_dir,
                               fp, ft.encrypt_size, block_cnt);

        // Jan 2020, Synopys has blocked creating directories on their server,
        // So the following block handles that trying to move part of 'remote_dir' to the 'remote_file'
        // replacing the slash by underscore in the 'remote_file', extra directories are not created
        DBG("ret=%d CURLE_REMOTE_FILE_NOT_FOUND=%d", ret, CURLE_REMOTE_FILE_NOT_FOUND);
        while (ret == CURLE_REMOTE_FILE_NOT_FOUND && (len = strlen(remote_dir)) > 0)
        {
            char *last_slash = strrchr(remote_dir, '/');
            char *copy_dir = NULL;
            while (last_slash != NULL && (last_slash - remote_dir + 1) == len) // slast is the last character such as 'test/'
            {
                *last_slash = 0;
                len = strlen(remote_dir);
                last_slash = strrchr(remote_dir, '/');
            }
            DBG("last_slash=%p", last_slash);
            if (last_slash != NULL)
            {
                copy_dir = last_slash + 1;
            }
            else
            {
                last_slash = remote_dir;
                copy_dir   = remote_dir;
            }
            // empty block
            {
                char save_remove_file[1024];
                strcpy3(save_remove_file, copy_dir, "_", remote_file);
                strcpy(remote_file, save_remove_file );
            }
            *last_slash = 0;
            ret = put_serial_block(ft.encrypt_file, remote_file, remote_dir,
                                   fp, ft.encrypt_size, block_cnt);

        }

        if (ret != SUCCESS)
        {
            ftp_status_update(fs.key, "Error:       Connection or Transfer failed");
            email_send_user(gu.email1, "Connection or Transfer failed", rtn.msg, -1);
            close_down(rtn.msg, ret);
        }
        elapsed_time_calc(current_time, elapsed_time, time_string);
        sprintf( rtn.msg, "block %d size %llu offset %lld  count %d. time %s ",
                 block_cnt, ft.local_block_size, offset, 0, elapsed_time);
        data_log(status_file, rtn.msg);
        FORCE_DBG("%s", rtn.msg);
        /* See if more blocks to send are present */
        if (ft.encryption_done == DBNO)
            total_blocks = encrypt_size_calc();
        /* Reset the offset */
        offset += (unsigned long long)ft.local_block_size;
    } /* End for */
    fclose(fp);

    DBG("ft.transfer_size=%llu", ft.transfer_size);
    /*
    sprintf(rtn.msg, "Total blocks:  %d, sent blocks:  %d",
         total_blocks, block_cnt);
        data_log(tr.log_file, rtn.msg);
    */
} /* End ftp_put_serial_blocks */

/* This function has not been tested */
int put_resend_block(int block_cnt, char *local_file, char *remote_dir, 	int total_blocks) 
{
    int status_array[1000];
    int ret;
    unsigned long long total_size;
    unsigned long long offset;
    int cnt;
    int status_cnt;
    long current_time = time(0);
    char status_file[64];
    char remote_file[4];
    FILE *fp;
    char line[260];
    char time_string[32];
    char elapsed_time[32];

    data_log(tr.log_file, "Resending unsent blocks");
    strcpy3(status_file, "history_data/", fs.key, ".status");
    if ((fp = fopen(status_file, "r")) == NULL) return(FAILURE);
    /* Initialize that no data has been sent */
    /* Initialize remote file name */
    strcpy(remote_file, "aa");
    /* Indicate at beginning of file */
    offset = 0L;
    for (cnt = 0; cnt < total_blocks; ++cnt)
    status_array[cnt] = 0;
    /* Now see which blocks were sent */
    for (cnt = 0; cnt < total_blocks; ++cnt)
    {
	line_get(fp, line);
	sscanf(line, "%*s %d", &status_cnt);
	status_array[status_cnt] = 1;
    }
    /* Now go back throu the array and resend any blocks that failed */
    for (cnt = 0; cnt < total_blocks; ++cnt)
    {
	/* Increment the offset */
	offset += (unsigned long long)FTP_PARALLEL_SIZE;
	if (status_array[cnt] == 0)
	{
	  if (fork() == 0)
	  {
	    if ((ret = put_intermediate_block(cnt, local_file, remote_file, remote_dir, total_blocks, &total_size)) == SUCCESS)
		{
	    elapsed_time_calc(current_time, elapsed_time, time_string);
		    sprintf( rtn.msg, "block %d size %d offset %lld  count %d. time %s file %s", 
	cnt, total_size, offset, 0, elapsed_time, remote_file);
	    data_log(status_file, rtn.msg);
	    data_log(tr.log_file, rtn.msg);
		/* Update the meta data */
		ftp_status_update(fs.key, rtn.msg);
		} /* End if for block successfully transferred*/
	/* the child process must exit */
	exit(0);
	    } /* end if for fork */
	} /* end if for block needed to be transferred */
	remote_file_increment(remote_file);
    offset += (unsigned long long)FTP_PARALLEL_SIZE;
    } /* end for */
} /* end resend function */

/* Send the remote file via FTP */
void ftp_put_parallel(int history_offset, char *local_file, char *remote_file, char *remote_dir,
 char *history_file, char *extra_data, long encrypt_time, char **email_array)
{
    char time_buffer[30];
    char time_stamp[32];
    int total_blocks;
    char new_remote_file[16];
    char remote_data_dir[PATH_SIZE];
    char command[160];
    char cksum[128] = "";
    char status_file[64];
    time_t ftp_start_time;

    DBG();
    /* Build name for local and encrypted file */
    strcpy(ft.local_file, local_file);
    strcpy2(ft.encrypt_file, local_file, ".gpg");
    ft.adjusted_block_num = 2;
    ft.local_block_size = FTP_PARALLEL_SIZE;
    /* Get the encryption running */
    /* Encrypt the data */
    /* Do not encrypt the data if this is a resend */
    if (gu.req_type == DBFTP_RESEND)
    ft.encryption_done = DBYES;
    else ft.encryption_done = process_encrypt(local_file, remote_dir, remote_file, history_file, extra_data, &encrypt_time);
    /* initialize everything needed for the data transfer */
    /* Build the time stamp */
    /* Not currently inserting the time stamp */
/*
    time_stamp_bld(time_stamp);
    time_stamp_insert(remote_file, time_stamp);
*/
    /* Get rid of leading slashes before anything else done */
    leading_slashes_remove(remote_dir);
    /* Add extra subdirectory to remote directory */
    strcpy4(remote_data_dir, remote_dir, "/", remote_file, ".incomplete");
    /* Calculate the number of blocks to send */
/* Each block will be 20000000 bytes in size */
    total_blocks = ((ft.encrypt_size-ft.first_block_offset)/ft.local_block_size) +ft.adjusted_block_num;
    sprintf( rtn.msg, "total_blocks is %d\n", total_blocks);
    data_log(history_file, rtn.msg);
    ftp_start_time = time(0);
    /* Initialize the new remote file name */
    strcpy(new_remote_file, "aa");
    /* Initialize the status file name */
    strcpy3(status_file, "history_data/", fs.key, ".status");
    /* If this file is already present, delete it */
    if (access(status_file, 0) == 0)
    unlink(status_file);
    /* Now create it as a blank file */
	strcpy2(command, "touch ", status_file);
    system(command);
    /* Now ready to send the data */
    ftp_put_parallel_blocks(ft.encrypt_file, new_remote_file, remote_data_dir, status_file, total_blocks);
    /* Calculate the FTP transfer time */
    data_log(tr.log_file, "Just transferred all of the blocks");
    /* Build the cksum */
    cksum_build(cksum, ft.encrypt_file);
    /* Send the finale script */
    if ((strncmp(cp.company, "Chartered", 9) == 0)
    || (strncmp(cp.company, "Local", 5) == 0)
    || (strncmp(cp.company, "Global Foundries", 16) == 0))
    ftp_put_finale_script(DBYES, remote_file, remote_dir, remote_data_dir, cksum);
    /* Get the size of the file */
    ft.transfer_size = ftp_calc_parallel_size();
    /* Update completion status and encryption size in the meta data */
    ftp_complete_update(fs.key, ft.transfer_size);
    /* built message sent to user after transfer complete */
    ftp_parallel_email_build(cksum, remote_file, remote_dir, extra_data, ftp_start_time);
    /* Append new information to history file */
    data_log_offset(history_file, email_buffer, history_offset);
    ftp_email_send(email_array);
    data_log(tr.log_file, rtn.msg);
    /* Update the SQL database for traceability */
    sql_intercom_history_update(fs.key);
} /* End ftp_put_parallel */

