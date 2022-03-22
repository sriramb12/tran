#ifndef client_maskshop_h
#define client_maskshop_h

#include <sys/stat.h>
#include <sys/types.h>

#define MASKSHOP_LOG "send2maskshop_log"

/* Resets to put verify if data going to mask shop */
int maskshop_req_type_reset(int req_type);

int maskshop_verify_files_send(void);

/* Pulls file names out of send2maskshop log */
int maskshop_files_find(char *files[], char *file_name);

/* Determine mask set name */
/* If can deduce mask set name from file name, simply use it */
/* If not, prompt user for mask set name */
int mask_info_get(char *file_name, char *mask);

int is_mask(char *mask);
/* Begin building the file to return to the data base */
void maskshop_info_build(void);

/* Close the file with the mask set info */
void maskshop_file_close(void);

void maskshop_file_send(void);

void maskshop_file_log(char *file_name, struct stat  *buf, int get_buf);

#endif
