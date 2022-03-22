#ifndef _INCLUDE_MAPPED_FILE_H
#define _INCLUDE_MAPPED_FILE_H

typedef unsigned long long int UINT64;

typedef struct MAPPED_FILE
{
    UINT64 pattern;
    UINT64 mem_key;
    int    blocks;
    size_t size_block;
} MAPPED_FILE;

struct MAPPED_FILE mapped_file_create(char *keyword, int block_cnt, int size_block);

#endif // _INCLUDE_MAPPED_FILE_H
