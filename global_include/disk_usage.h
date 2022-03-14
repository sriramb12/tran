#ifndef disk_usage_h
#define disk_usage_h

#include <stdio.h>
struct DIR_INFO 
{
    long st_ino;
    long st_dev;
    struct DIR_INFO *next;
};

long long big_disk_usage(char *directory, int local_dir_cnt);
long long disk_usage(char *directory);
long long ldisk_usage(char *directory, struct DIR_INFO*dir_ptr);
long long big_ldisk_usage(char *directory, struct DIR_INFO*dir_ptr);

int dir_info_traverse(struct DIR_INFO *dir_ptr, long st_ino, long st_dev);

struct DIR_INFO *dir_info_add(struct DIR_INFO *dir_ptr, long st_ino, long st_dev);

/* Calculate directory size */
/* Also calculate estimated compressed size */
long long big_random_disk_usage(char *directory, int local_dir_cnt);

#endif /* Define for disk_usage */
