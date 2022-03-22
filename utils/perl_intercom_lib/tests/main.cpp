#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "intercom_lib.h"

#define DEVEL 1     // set this to either 0 or 1,  or use the option

#if DEVEL
# define DEBUG(format, ...)    fprintf(stderr, "[debug] %s:%03d \n" format,__FILE__, __LINE__, ##__VA_ARGS__);
#else
# define DEBUG(format, ...)    /**/
#endif

#define SUCCESS 0
#define DBYES 1
#define DBNO  2


#define is_equal_DBYES(x)  (x == DBYES)
#define is_equal_DBNO(x)   (x == DBNO)

#define   COMPRESSED_GZ   1
#define   UNCOMPRESSED    2

struct FileInfo
{
     char           filename[128];
     long long int  size;
     long long int  compressed_size;
     int            compressed_mode;
};




void orthers()
{
    char **msg_ptr = 0;
    icom_repository_rtv(&msg_ptr);
    icom_company_rtv(&msg_ptr);

    icom_authen("carlos.mazieri@gmail.com", "bad_password");

}



int PUT(char *argv)
{
    struct stat   statbuf;
    int ret = -1;
    char *msg_ptr  = NULL;    
    defaults_init("127.0.0.1", "carlos.mazieri@gmail.com", NULL, "Mazieri");
    char ** repo = icom_repository_rtv();
    while (repo && *repo)
    {
        printf ("repo = %s\n", *repo);
        repo++;
    }
    if (stat(argv, &statbuf) == 0)
    {
        FILE *file = icom_put_compress_init(argv,
                                           "NXP Semiconductors;localhost;127.0.0.1;6923;carlos.mazieri@gmail.com;Transweb dir;",
                                           "Description done",
                                           "carlos.mazieri@nxp.com",
                                            DBNO);

        printf("file=%p\n", file);
        msg_ptr = get_global_msg();

        if (msg_ptr)
        {
            printf ("%s\n", msg_ptr);
        }

        if (file)
        {
            int fd = open(argv, O_RDONLY);
            if (fd != -1)
            {
                char buffer [1024];
                ssize_t bytes_read = 0;
                long long written = 0;
                while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
                {
                    written = icom_write(file, buffer, (unsigned)bytes_read);
                    printf("written=%lld\n", written);
                }
                msg_ptr = icom_put_compress_close(file, "test using C interface");
                if (msg_ptr)
                {
                    printf ("%s\n", msg_ptr);
                    ret = 0;
                }
            }
        }
    }
    return ret;
}


int prepare_headers(struct FileInfo *info, int compressed_flag)
{
#if DEVEL
    int fd = open(info->filename, O_CREAT | O_WRONLY, 0666);
#else
    int fd = 1;
    char headers [512];
    int  length = sprintf(headers, "Content-Disposition: attachment; filename=\"%s\"\r\n", info->filename);
    if (compressed_flag == COMPRESSED_GZ)
    {
        length += sprintf(&headers[length], "Content-Type: application/zip; charset=ISO-8859-1\r\nContent-Length: %lld\r\n\r\n", info->compressed_size);
    }
    else
    {
         length += sprintf(&headers[length], "Content-Type: application/octet-stream; charset=ISO-8859-1\r\nContent-Length: %lld\r\n\r\n", info->size);
    }
   // write(fd, headers, length);
    sleep(1);

#endif
    return fd;
}

int getFileinfo(char *secure_keyword, char *file_keyword,  int delete_flag, struct FileInfo *info)
{
     int ret     = icom_size(secure_keyword, file_keyword);
     memset(info, 0, sizeof(struct FileInfo));
     if (ret == 0)
     {
          char none[16];
          char *msg  = get_global_msg();
          DEBUG("msg=%s\n", msg);
          sscanf(msg, "File: %s\nOriginal size: %lld bytes\nCompressed size: %lld bytes",  info->filename, &info->size, &info->compressed_size);

          ret     = icom_get_init(secure_keyword, file_keyword, delete_flag);
          if (ret == 0)
          {
              DEBUG("msg=%s\n", msg);
              sscanf(msg, "%s %s %s %s %d",
                        none, none, none, none, &info->compressed_mode);
              DEBUG("filename=%s size=%lld comp_size=%lld comp_mode=%d\n",
                        info->filename, info->size, info->compressed_size, info->compressed_mode);
          }
     }
     return ret;
}


int  getFile_uncompressed(struct FileInfo *info)
{
    FILE *file_ptr = NULL;
    long long int total_bytes = 0;
    int ret = -1;
    file_ptr = icom_get_compress_init(info->size);
    if (file_ptr != NULL)
    {
        ssize_t read_bytes = icom_read(file_ptr, NULL);
        char *buffer = get_read_buffer();
        DEBUG("icom_read()=%lu\n", read_bytes);
        int fd_out = prepare_headers(info, UNCOMPRESSED);
        while (read_bytes > 0)
        {
            if (write(fd_out, buffer, read_bytes) != read_bytes)
            {
                fprintf(stderr, "ERROR: could not write");
            }
            total_bytes += read_bytes;
            read_bytes = icom_read(file_ptr, NULL);
        }
        ret = icom_get_compress_close(file_ptr);
        DEBUG("getFile_uncompressed() total_bytes=%lld\n", total_bytes);
    }
    return ret;
}


int getFile_compressed(struct FileInfo *info)
{
    int file_len = strlen(info->filename);
    long long int total_bytes = 0;
    ssize_t read_bytes = 0;
    int ret = -1;
    char *buffer = get_read_buffer();
    if (file_len > 3 && strcasecmp(&info->filename[file_len -2], "gz"))  // filename does not end with gz
    {
        strcat(info->filename, ".gz");
    }
    int fd_out = prepare_headers(info, COMPRESSED_GZ);
    read_bytes = get_string(NULL);
    while (read_bytes > 0)
    {
        if (write(fd_out, buffer, read_bytes) != read_bytes)
        {
            fprintf(stderr, "ERROR: could not write");
        }
        total_bytes += read_bytes;
        read_bytes = get_string(NULL);
    }
    DEBUG("getFile_compressed() total_bytes=%lld\n", total_bytes);
    ret = icom_get_close();
    return ret;
}



int get(char *user_id, char *secure_keyword, char *file_keyword, int get_compress_flag, int delete_flag)
{
    defaults_init("127.0.0.1", user_id, user_id, "Nobody");
    char **status = icom_status(secure_keyword, file_keyword);
    int ret = -1;
    if (status)
    {
        struct FileInfo info;
        int ret_int = getFileinfo(secure_keyword, file_keyword, delete_flag, &info);
        if (ret_int == 0 && put_short(0) == 0)
        {
            if (is_equal_DBNO(get_compress_flag) && is_equal_DBYES(info.compressed_mode))
            {
                ret = getFile_uncompressed(&info);
            }
            else
            {
                ret = getFile_compressed(&info);
            }
        }
    }
    return ret;
}

/* argv[1] = user_id  argv[2] = secure_keyword argv[3] = file_keyword argv[4] = get_compressed_flag argv[5] = delete_after_get_flag */
int GET(int argc, char *argv[])
{
    int   get_compressed_flag = DBNO;
    int   delete_after_get_flag = DBNO;
    int   ret= 0;

    if (argc >= 4)
    {
        int integer = 0;
        if (argc > 4)
        {
            integer = atoi(argv[4]);
            if (is_equal_DBNO(integer) || is_equal_DBYES(integer))
            {
                get_compressed_flag = integer;
            }
            if (argc > 5)
            {
                integer = atoi(argv[5]);
                if (is_equal_DBNO(integer) || is_equal_DBYES(integer))
                {
                    delete_after_get_flag = integer;
                }
            }
        }
        DEBUG("get_compressed_flag=%d delete_after_get_flag=%d", get_compressed_flag, delete_after_get_flag);
        ret = get(argv[1], argv[2], argv[3], get_compressed_flag, delete_after_get_flag);
    }
    DEBUG("ret=%d", ret);
    return ret;
}

int test_get()
{
    char secure_keyword [12] = "mmzt3507T";
    char file_keyword[12]    = "jjqi0679t";
    return get("nxa13339", secure_keyword, file_keyword, 1, 2);
}


int main(int argc, char *argv[])
{
    int ret = 0;
//    switch(argc)
//    {
//       case 2:  ret = PUT(argv[1]);
//                break;
//       case 1:  printf("%s <file> for put or\n%s <user_id> <secure_keyword> <file_keyword> [ compressed_flag [delete_flag]]\n", argv[0], argv[0]);
//                break;
//      default:  ret = GET(argc, argv);
//                break;
//    }
    test_get();
    return ret;
}
