#ifndef TRANSAPI
/****************************************************************************
 *
 * Functions:	trans_open, trans_read, trans_write, trans_close
 *
 * Description:	Routines to print status of data transfer to the screen.
 *
 ***************************************************************************/

#include <stdio.h>
#include <time.h>
#include "global_defines.h"
#include "global_params.h"
#include "gzio.h"
#include "hash.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transrc.h"
#include "server_params.h"
#include "trans_prototypes.h"
#include "client_lib.h"
#include <zlib.h>

typedef struct {
  int  enable;
  long long file_size;
  long long total_bytes_xfered;
  long report_time;
  long long report_size;
    long begin_time;
    long elapsed_time;
    int time_left;
} xfer_status_t;

char parallel_type_array[3][16] = 
{
    "Serial",
    "Parallel",
    "Separate block",
};

/* holds file size used by trans_hash_init and trans_hash */
xfer_status_t hs;

/* Set the iCAP classification */
void icap_class_assign(char *icap_string)
{
    extern char icap_class[];

#ifdef NXP
    if (icap_class[0] == '\0') strcpy(icap_string, "NXP Confidential Proprietary");
#else
    if (icap_class[0] == '\0') strcpy(icap_string, "Confidential Proprietary");
#endif
    else if (strcasecmp(icap_class, "fcp") == 0) strcpy(icap_string, "NXP Confidential Proprietary");
    else if (strcasecmp(icap_class, "gbi") == 0) strcpy(icap_string, "General Business Information");
    else if (strcasecmp(icap_class, "pubi") == 0) strcpy(icap_string, "Public Information");
    else if (strcasecmp(icap_class, "fiuo") == 0) strcpy(icap_string, "NXP Internal Use Only");
    else if (strcasecmp(icap_class, "cp") == 0) strcpy(icap_string, "confidential Proprietary");
#ifdef NXP
    else strcpy(icap_string, "NXP confidential Proprietary");
#else
    else strcpy(icap_string, "confidential Proprietary");
#endif
} /* end icap_class_assign */

gzFile trans_open ( char *mode, long long hash_size, int hash_enable, TRANS_FILE_TYPE file_type) 
{
    gzFile file = tfile_gzopen(mode);

  trans_hash_init (hash_size, file_type, mode, SERIAL);
    return(file);
} /* End trans_open */

int trans_read (gzFile file, voidp buf, char get_header) 
{
    int size = tfile_gzread(file, buf, BLOCK_SIZE);
     return(trans_hash(size, 0));
} /* End trans_read */

/* ------------------------------------------------------------ trans_write */
void trans_write (gzFile file, const voidp buf, unsigned len) 
{
    int size = tfile_gzwrite(file, buf, len);
    trans_hash(size, 0);
} /* End trans_write */

/* Write data to a pipe for parallel transfer */
/*
void trans_parallel_write (int fi, gzFile file,  const voidp buf, unsigned len) 
{
    int size = trans_parallel_gzwrite(fi, file, buf, len);
    trans_hash(size, 0);
} */
/* End trans_parallel_write */

/* ------------------------------------------------------------ trans_close */
long long trans_close (gzFile file, char file_type) 
{
    tfile_gzclose(file);
    /* Indicate 100% of data transferred */
    trans_hash_close();
     /* Return the final size */
    return(hs.total_bytes_xfered);
} /* End trans_close */

/* ---------------------------------------------------------------- num_bytes */
char* num_bytes (long long bytes) 
{
  static char byte_string [64];

  if (bytes < 1024 * 1024)
	sprintf (byte_string, "%ld bytes", (unsigned long)bytes);
    else if (bytes < 1024*1024 * 1024)
	sprintf (byte_string, "%0.2f MB", 1.0 * bytes / 1024 / 1024);
#ifdef WINDOWS_NT4
	else sprintf (byte_string, "%0.4f GB", 1.0 * (unsigned long)bytes / 1024 / 1024/1024);
#else
	else sprintf (byte_string, "%0.4f GB", 1.0 * bytes / 1024 / 1024/1024);
#endif
  return byte_string;
} /* End num_bytes */

static char* num_gmb1024bytes (long long bytes) 
{
  static char byte_string [64];

  if (bytes < 1024 * 1024)
	sprintf (byte_string, "%ld bytes", (unsigned long)bytes);
    else if (bytes < 1024*1024 * 1024)
	sprintf (byte_string, "%0.2f MB", 1.0 * bytes / 1024 / 1024);
#ifdef WINDOWS_NT4
	else sprintf (byte_string, "%0.4f GB", 1.0 * (unsigned long)bytes / 1024 / 1024/1024);
#else
	else sprintf (byte_string, "%0.4f GB", 1.0 * bytes / 1024 / 1024/1024);
#endif
  return byte_string;
} /* End num_gmb1024bytes */

#ifdef WINDOWS_NT4
static char* num_gmb1000bytes (long bytes) 
#else
static char* num_gmb1000bytes (long long bytes) 
#endif
{
  static char byte_string [64];

  if (bytes < 1000 * 1000)
	sprintf (byte_string, "%ld bytes", (unsigned long)bytes);
    else if (bytes < 1000*1000 * 1000)
	sprintf (byte_string, "%0.2f MB", 1.0 * bytes / 1000 / 1000);
#ifdef WINDOWS_NT4
	else sprintf (byte_string, "%0.4f GB", 1.0 * (unsigned long)bytes / 1000 / 1000/1000);
#else
	else sprintf (byte_string, "%0.4f GB", 1.0 * bytes / 1000 / 1000/1000);
#endif
  return byte_string;
} /* End num_gmb1000bytes */

/* Print estimated file size */
void total_size_print(long long fsize, char *mode, TRANS_FILE_TYPE  file_type, int transfer_type)
{
    char icap_string[48];
    extern long long global_compressed_size;

    /* Set the iCAP classificaiton */
    icap_class_assign(icap_string);
    if (*mode == 'r')
#ifdef INTERCOM
    PRINTF ("Data transfer from %s started.\n", server.site);
    else PRINTF("Data transfer to %s started.\n", server.site);
#else
    PRINTF ("Data transfer from %s %s repository started.\n", server.site, "Transcend");
    else PRINTF("Data transfer to %s %s repository started.\n", server.site, "Transcend");
#endif
	if ((*mode == 'w') && (file_type == DIRECTORY))
	{
	if (fsize < 1024*1024) fsize += fsize*.15;
	else if (fsize < 1024*1024*1024) fsize += fsize*.1;
	else fsize += fsize*.05;
 	hs.file_size = fsize; 
    if (fsize > 0)
	    PRINTF("  Estimated directory size is %s.", num_gmb1024bytes (fsize));
	if (transfer_type == DBYES)
	printf("\nEstimated compressed size is %s.",num_gmb1024bytes (global_compressed_size));
	} /* End if for putting directory */
	  else 
    {
	/* If getting directory already know exact size */
	if (file_type == DIRECTORY)
	printf ("  Directory size is %s.", num_gmb1024bytes (fsize));
	else PRINTF("  File size is %s.", num_gmb1024bytes (fsize));
    }
	printf ("\n");
#ifdef SSL_ENCRYPTION
    PRINTF("Transfer Method:  %s with encryption.\nClassification:  %s.\n\n", parallel_type_array[transfer_type], icap_string);
#else
    PRINTF("Transfer Method:  %s with no encryption.\n\n", parallel_type_array[transfer_type]);
#endif
    fflush(stdout);
} /* end total_size_print */

/* ---------------------------------------------------------- trans_hash_init */
void trans_hash_init (long long fsize, TRANS_FILE_TYPE file_type, 
char *mode, int transfer_type) 
{
    extern int display_transfer_status;
/* Passed in total file size and byte to enable hashing */

    hs.begin_time = time(0);
    hs.elapsed_time = hs.begin_time- 15;
  hs.enable = display_transfer_status;
  hs.total_bytes_xfered = 0;
    hs.time_left = 17;
    if (display_transfer_status) 
    {
	if (transrc.hashByTime)
	  hs.report_time = time (NULL) + transrc.hashByTime;
	hs.report_size = (long long)transrc.hashBySize;
#ifndef TRANSWEB
    total_size_print(fsize, mode, file_type, transfer_type);
#endif
	hs.file_size = fsize;
    } /* End if for enable on */
} /* End trans_hash_init */

/* --------------------------------------------------------------- trans_hash */
long long trans_hash (int new_bytes_xfered, int transfer_type) 
{
    extern int size_printed;
    extern int perform_background_transfer;
  int print_now = 0;
  long current_time;

    if (transfer_type == 2)
  hs.total_bytes_xfered = new_bytes_xfered;
  else hs.total_bytes_xfered += new_bytes_xfered;
    /* if being performed in background, update shared memory with new size */
    if (perform_background_transfer == DBYES)
  shared_status_write(1,  hs.total_bytes_xfered );
  if ((!hs.enable)
    || (hs.total_bytes_xfered > hs.file_size))
	return new_bytes_xfered;

    /* always print status for separate block transfers */
    print_now = transfer_type;
    if ((!print_now)
  && (transrc.hashByTime)) {
	current_time = time (NULL);
	print_now = (current_time >= hs.report_time);
  }
  if (!print_now && transrc.hashBySize)
	print_now = (hs.total_bytes_xfered >= hs.report_size);
  
  if (print_now) {
	if (transrc.hashByTime)
	  hs.report_time = current_time + transrc.hashByTime;
	if (transrc.hashBySize)
	  hs.report_size = hs.total_bytes_xfered + transrc.hashBySize;
    ++hs.time_left;
  }

  if (print_now) 
    {
	/* Indicate that amount of data transferred printed */
	size_printed = DBYES;
#ifdef SEND2FOUNDRY
	rtn.len = sprintf (rtn.msg, "%s transferred.", num_bytes (hs.total_bytes_xfered));
#else
	printf ("%s transferred.", num_bytes (hs.total_bytes_xfered));
#endif
	/* If file size specified, print percentage */
	if (hs.file_size)
	{
	    /* First calculate percentage */
#ifdef WINDOWS_NT4
	    float x = 100.0 * hs.total_bytes_xfered / (unsigned long)hs.file_size;
#else
	    float x = 100.0 * hs.total_bytes_xfered / hs.file_size;
#endif
	    /* If over 100%, display it as 99.9 */
	    if (x >= 100.0) x = 99.9;
#ifdef SEND2FOUNDRY
	    sprintf(rtn.msg+rtn.len, "  (%0.1f%%)", x); 
#else
	    PRINTF("  (%0.1f%%)", x); 
#endif
	} /* End if for file size specified */
#ifdef SEND2FOUNDRY
    status_log("transfer_status", rtn.msg);
#else
	printf ("\n");
#endif
	/* Now see if estimated time should be printed */
    if (hs.time_left == 20)
	{
	    PRINTF( time_left_calc(hs.file_size, hs.total_bytes_xfered, (current_time-hs.begin_time)));
	    hs.elapsed_time = current_time;
	hs.time_left = 0;
	} /* End if for printing estimated time left */
    fflush(stdout);
    } /* End if for ready to print */
  return new_bytes_xfered;
} /* end trans_hash */

void trans_hash_close(void)
{
    extern int size_printed;

	if (size_printed == DBYES)
#ifdef SEND2FOUNDRY
    sprintf (rtn.msg, "%s transferred.  (100%%)", num_bytes (hs.file_size));
    status_log("transfer_status", rtn.msg);
#else
    PRINTF ("%s transferred.  (100%%)\n", num_bytes (hs.file_size));
#endif
} /* end trans_hash_close */

long long transweb_hash (int new_bytes_xfered, FILE *output_fp) 
{
  int print_now = 0;
  long current_time;

  hs.total_bytes_xfered += new_bytes_xfered;

  if (!hs.enable)
	return new_bytes_xfered;

  if (transrc.hashByTime) {
	current_time = time (NULL);
	print_now = (current_time >= hs.report_time);
  }
  if (!print_now && transrc.hashBySize)
	print_now = (hs.total_bytes_xfered >= hs.report_size);
  
  if (print_now) {
	if (transrc.hashByTime)
	  hs.report_time = current_time + transrc.hashByTime;
	if (transrc.hashBySize)
	  hs.report_size = hs.total_bytes_xfered + transrc.hashBySize;
  }

  if (print_now) {
	fprintf (output_fp,"%s transferred.", num_bytes (hs.total_bytes_xfered));
	/* If file size specified, print percentage */
	if (hs.file_size)
	{
	    /* First calculate percentage */
#ifdef WINDOWS_NT4
	    float x = 100.0 * hs.total_bytes_xfered / (unsigned long)hs.file_size;
#else
	    float x = 100.0 * hs.total_bytes_xfered / hs.file_size;
#endif
	    /* If over 100%, display it as 99.9 */
	    if (x >= 100.0) x = 99.9;
	    fprintf(output_fp, "  (%0.1f%%)", x); 
	} /* End if for file size specified */
	fprintf (output_fp, "\n");
    fflush(output_fp);
  }
  return new_bytes_xfered;
} /* end transweb_hash */
#endif /* For not TRANSAPI */
