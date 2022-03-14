#ifndef big_file_h
#define big_file_h

#include <stdio.h>
#include "trans_server_defines.h"
#include "trans_server_macros.h"

/* Reads in big file and makes sure user owns file */
FILE *read4own_big_file(char *key, char lines[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs);


FILE *read4info_big_file(char *key, char lines[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs, int close_flag);

/* Write out modified contents of big file */
void write_big_file(FILE *fp, char line[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs);

#endif
