#ifndef trans_hash_h
#define trans_hash_h

#include <zlib.h>
#include "trans_defines.h"

/* Set the iCAP classification */
void trans_icap_class_set(int index, char *icap_string);

unsigned long long tfile_hash (int new_bytes_xfered, int transfer_type, int index); 

gzFile tfile_open ( char *mode, int hash_enable, int index);

int tfile_read (gzFile file, voidp buf, char get_header, int index); 

void tfile_write (gzFile file, const voidp buf, unsigned len, int index); 

/* Write data to a pipe for parallel transfer */
void trans_parallel_write(int fi, gzFile file,  const voidp buf, unsigned len); 

unsigned long long tfile_close(gzFile file, char file_type); 

char* gmb100bytes_calc(unsigned long long bytes); 

/* Print estimated file size */
void total_bytes_print(int index, unsigned long long fsize, char *mode, TRANS_FILE_TYPE  file_type, int transfer_type);


void tfile_hash_init (unsigned long long fsize, TRANS_FILE_TYPE file_type, 
char *mode, int transfer_type, int index); 

void tfile_hash_close(void);


#endif
