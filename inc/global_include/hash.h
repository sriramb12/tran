#ifndef hash_h
#define hash_h

#include "zlib.h"
#include "trans_defines.h"

gzFile trans_open ( char *mode, long long hash_size, int hash_enable, TRANS_FILE_TYPE file_type); 

int trans_read (gzFile file, voidp buf, char get_header); 

/* ------------------------------------------------------------ trans_write */
void trans_write (gzFile file, const voidp buf, unsigned len); 

/* ------------------------------------------------------------ trans_close */
long long trans_close (gzFile file , char file_type); 

/* ---------------------------------------------------------------- num_bytes */
char* num_bytes (long long bytes); 

/* ---------------------------------------------------------- trans_hash_init */
void trans_hash_init (long long fsize, TRANS_FILE_TYPE file_type, 
char *mode, int parallel); 

/* --------------------------------------------------------------- trans_hash */
long long trans_hash (int new_bytes_xfered, int transfer_type); 

void trans_hash_close(void);

long long transweb_hash (int new_bytes_xfered, FILE *fp_output); 

/* Print estimated file size */
void total_size_print(long long fsize, char *mode, TRANS_FILE_TYPE  file_type, int parallel);

#endif
