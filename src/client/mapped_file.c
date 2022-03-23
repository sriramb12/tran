
/****************************************************************************
 *
 * Function:	Mapped file functions
 *
 * Description:	it replaces functions of shared memory
 *
 * Author:	Carlos Jose Mazieri <carlos.mazieri@nxp.com>
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include "global_defines.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_client_errs.h"
#include "trans_prototypes.h"

#include "mapped_file.h"
#include "global_debug.h"


#ifdef CGWIN_BUILD
# include <sys/cygwin.h>
#endif

static char * create_block_filename(MAPPED_FILE *mapped_file, int block)
{
    static char filename[1024] = {0};
    char *temp = "/tmp";
#ifdef CGWIN_BUILD
    static char win_temp_path[512] = {0};
    if (filename[0] == 0)
    {
        cygwin_internal (CW_SYNC_WINENV);
        char *win_temp = getenv("TEMP");
        if (win_temp != NULL && *win_temp != '\0')
        {
           strcpy(win_temp_path, win_temp);
           char* slash = strchr(win_temp_path, '\\');
           while (slash != NULL)
           {
              *slash = '/';
              slash = strchr(win_temp_path, '\\');
           }
        }
    }
    if (win_temp_path[0] == 0)
    {
        strcpy (win_temp_path, "/cygdrive/c/temp");
    }
    temp = win_temp_path;
    DBG("temp=%s", temp);
#endif
    int size = sprintf(filename, "%s/trans_%03d_%llu_%llu", temp, block, mapped_file->mem_key, mapped_file->pattern);
    if (size >= sizeof(filename))
    {
        DBG("creating temp file [%s]", filename);
        fprintf(stderr, "create_block_filename(): buffer overflow\n");
        exit(0);
    }
    DBG("mapped_file->blocks=%d mapped_file->size_block=%d mapped_file->mem_key=%llu",
        mapped_file->blocks, mapped_file->size_block, mapped_file->mem_key);
    DBG("filename=%s mapped_file->pattern [%llu]", filename, mapped_file->pattern);
    return filename;
}


static int open_block_filename(MAPPED_FILE *mapped_file, int block, int mode)
{
    char *filename = create_block_filename(mapped_file, block);
    int fd = open(filename, mode);
    if (fd == -1)
    {
        fd = open(filename, O_CREAT | O_WRONLY, 0644);
        close(fd);
        fd = open(filename, mode);
    }
    DBG("fd=%d", fd);
    return fd;
}


/* Create the shared memory */
struct MAPPED_FILE mapped_file_create(char *keyword, int block_cnt, int size_block)
{
    DBG("keyword=%s block_cnt=%d", keyword, block_cnt);

    int key_id;
    char tmp_key[8];
    struct MAPPED_FILE mapped_file;
    memset(&mapped_file, 0, sizeof(mapped_file));
    strncpy(tmp_key, keyword+4, 4);
    key_id = atoi(tmp_key);
    /* Set the key */
    mapped_file.mem_key = getpid();
    mapped_file.blocks     = block_cnt;
    mapped_file.size_block = (size_t) size_block;
    UINT64 *pt_utin64 = (UINT64 *)keyword;
    mapped_file.pattern = *pt_utin64;
    int fd = open_block_filename(&mapped_file, 0, O_RDWR);
    if (fd != -1)
    {
        close(fd);
        char * mapped_block = malloc(mapped_file.size_block);
        memset(mapped_block, 0, mapped_file.size_block);
        int counter = block_cnt;
        while (counter--)
        {
            fd = open_block_filename(&mapped_file, counter, O_RDWR);
            write(fd, mapped_block, mapped_file.size_block);
            close(fd);
        }
        free(mapped_block);
    }
    else
    {
        memset(&mapped_file, 0, sizeof(mapped_file));
    }
    return(mapped_file);
} /* end mapped_file_create */



int mapped_file_allocate(int mem_key)
{
    DBG("mem_key = '%d'", mem_key);
    return 1;
} /* end mapped_file_allocate */


/* Write to the shared memory */
void mapped_file_write(MAPPED_FILE *mapped_file, char *data, int block_num)
{
    ssize_t bytes = 0;
    int fd = open_block_filename(mapped_file, block_num, O_RDWR);
    if (fd != -1)
    {
        bytes = write(fd, data, mapped_file->size_block);
        close(fd);
    }
    DBG("bytes=%lu", (unsigned long)bytes);
} /* End mapped_file_write */



int mapped_fileread(MAPPED_FILE *mapped_file, char *buffer, int block_num)
{
    DBG();
    ssize_t bytes = 0;
    int fd = open_block_filename(mapped_file, block_num, O_RDONLY);
    if (fd != -1)
    {
        bytes = read(fd, buffer, mapped_file->size_block);
        close(fd);
    }
    DBG("bytes=%lu", (unsigned long)bytes);
    return (int) bytes;
} /* end mapped_fileread */


/* Removed the shared memory no longer needed */
void mapped_file_release(MAPPED_FILE * mapped_filename)
{
    DBG();
    int counter = mapped_filename->blocks;
    while (counter--)
    {
        char *name = create_block_filename(mapped_filename, counter);
        unlink(name);
    }
} /* End mapped_file_release */


/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
