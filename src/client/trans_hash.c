#ifdef TRANSAPI
/****************************************************************************
 *
 * Functions:	tfile_open, tfile_read, tfile_write, tfile_close
 *
 * Description:	Routines to print status of data transfer to the screen.
 *
 ***************************************************************************/

#include <stdio.h>
#include <time.h>
#include "global_defines.h"
#include "global_params.h"
#include "gzio.h"
#include "trans_hash.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transrc.h"
#include "server_params.h"
#include "trans_prototypes.h"
#include "ssl_client_lib.h"
#include "zlib.h"

#include "global_debug.h"


typedef struct {
    int  enable;
    unsigned long long file_size;
    unsigned long long total_bytes_xfered;
    long report_time;
    unsigned long long report_size;
    long begin_time;
    long elapsed_time;
    int time_left;
} xfer_status_t;

gzFile ZEXPORT tfile_gzopen (const char *mode);
int ZEXPORT tfile_gzread (gzFile file, voidp buf, unsigned len);
int ZEXPORT tfile_gzwrite (gzFile file, const voidp buf, unsigned len);
int ZEXPORT tfile_gzclose (gzFile file);

extern int gl_is_web;  // special flag for transweb

/* holds file size used by tfile_hash_init and tfile_hash */
xfer_status_t hs;


char* bytes_calc(unsigned long long bytes)
{
  DBG();
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
} /* End bytes_calc */


static char* gmb1024_bytes_calc(long long bytes)
{
    DBG();
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
} /* End gmb1024_bytes_calc */


/* Set the iCAP classification */
void trans_icap_class_set(int index, char *icap_string)
{
    DBG();

#ifdef NXP
    if (trans[index]->icap_class[0] == '\0') strcpy(icap_string, "NXP Confidential Proprietary");
#else
    if (trans[index]->icap_class[0] == '\0') strcpy(icap_string, "Confidential Proprietary");
#endif
    else if (strcasecmp(trans[index]->icap_class, "fcp") == 0) strcpy(icap_string, "NXP Confidential Proprietary");
    else if (strcasecmp(trans[index]->icap_class, "gbi") == 0) strcpy(icap_string, "General Business Information");
    else if (strcasecmp(trans[index]->icap_class, "pubi") == 0) strcpy(icap_string, "Public Information");
    else if (strcasecmp(trans[index]->icap_class, "fiuo") == 0) strcpy(icap_string, "NXP Internal Use Only");
    else if (strcasecmp(trans[index]->icap_class, "cp") == 0) strcpy(icap_string, "confidential Proprietary");
#ifdef NXP
    else strcpy(icap_string, "NXP confidential Proprietary");
#else
    else strcpy(icap_string, "confidential Proprietary");
#endif
} /* end trans_icap_class_set */

/* --------------------------------------------------------------- tfile_hash */
unsigned long long tfile_hash (int new_bytes_xfered, int transfer_type, int index) 
{
    DBG();
    extern int size_printed;
    extern int perform_background_transfer;
  int print_now = 0;
  long current_time;

  if (transfer_type == 2)
  {
      hs.total_bytes_xfered = new_bytes_xfered;
  }
  else
  {
      hs.total_bytes_xfered += new_bytes_xfered;
  }
    /* if being performed in background, update shared memory with new size */
/* Background transfer not supported for windows */
#ifndef WINDOWS_NT4
    if (perform_background_transfer == DBYES)
  shared_status_write(1,  hs.total_bytes_xfered );
#endif
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

  if (print_now && gl_is_web == 0)
  {
      /* Indicate that amount of data transferred printed */
      size_printed = DBYES;
#ifdef SEND2FOUNDRY
      rtn.len = sprintf (rtn.msg, "%s transferred.", bytes_calc (hs.total_bytes_xfered));
#else
      printf ("%s transferred.", bytes_calc (hs.total_bytes_xfered));
      fflush(stdout);
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
} /* end tfile_hash */

gzFile tfile_open ( char *mode, int hash_enable, int index)
{
    DBG();
    gzFile file = tfile_gzopen(mode);

  tfile_hash_init (trans[index]->original_size, trans[index]->file_type, mode, SERIAL, index);
    return(file);
} /* End tfile_open */

int tfile_read (gzFile file, voidp buf, char get_header, int index) 
{
    DBG();
    int size = tfile_gzread(file, buf, BLOCK_SIZE);
     return(tfile_hash(size, 0, index));
} /* End tfile_read */

/* ------------------------------------------------------------ tfile_write */
void tfile_write (gzFile file, const voidp buf, unsigned len, int index) 
{
    DBG();
    int size = tfile_gzwrite(file, buf, len);
    tfile_hash(size, 0, index);
} /* End tfile_write */

/* Write data to a pipe for parallel transfer */
/*
void trans_parallel_write (int fi, gzFile file,  const voidp buf, unsigned len) 
{
    int size = trans_parallel_gzwrite(fi, file, buf, len);
    tfile_hash(size, 0, index);
} */
/* End trans_parallel_write */

/* ------------------------------------------------------------ tfile_close */
unsigned long long tfile_close (gzFile file, char file_type) 
{
    DBG();
    tfile_gzclose(file);
    /* Indicate 100% of data transferred */
    tfile_hash_close();
     /* Return the final size */
    return(hs.total_bytes_xfered);
} /* End tfile_close */

char* gmb100bytes_calc (unsigned long long bytes) 
{
    DBG();
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
} /* End gmb100bytes_calc */


/* Print estimated file size */
void total_bytes_print(int index, unsigned long long fsize, char *mode, TRANS_FILE_TYPE  file_type, int transfer_type)
{
    DBG();
    char parallel_type_array[3][16] =
    {
        "Serial",
        "Parallel",
        "Separate block",
    };

    char icap_string[48];
    extern unsigned long long global_compressed_size;

    /* Set the iCAP classification */
    trans_icap_class_set(index, icap_string);
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
	    PRINTF("  Estimated directory size is %s.", gmb1024_bytes_calc (fsize));
	if (transfer_type == DBYES)
	printf("\nEstimated compressed size is %s.",gmb1024_bytes_calc (global_compressed_size));
	} /* End if for putting directory */
	  else 
    {
	/* If getting directory already know exact size */
	if (file_type == DIRECTORY)
	printf ("  Directory size is %s.", gmb1024_bytes_calc (fsize));
	else PRINTF("  File size is %s.", gmb1024_bytes_calc (fsize));
    }
	printf ("\n");
#ifdef SSL_ENCRYPTION
    PRINTF("Transfer Method:  %s with encryption.\nClassification:  %s.\n\n", parallel_type_array[transfer_type], icap_string);
#else
    PRINTF("Transfer Method:  %s with no encryption.\n\n", parallel_type_array[transfer_type]);
#endif
    fflush(stdout);
} /* end total_bytes_print */

/* ---------------------------------------------------------- tfile_hash_init */
void tfile_hash_init (unsigned long long fsize, TRANS_FILE_TYPE file_type, 
char *mode, int transfer_type, int index) 
{
    DBG();
/* Passed in total file size and byte to enable hashing */

    hs.begin_time = time(0);
    hs.elapsed_time = hs.begin_time- 15;
  hs.enable = trans[index]->display_transfer_status;
  hs.total_bytes_xfered = 0;
    hs.time_left = 17;
    if (trans[index]->display_transfer_status) 
    {
	if (transrc.hashByTime)
	  hs.report_time = time (NULL) + transrc.hashByTime;
	hs.report_size = (unsigned long long)transrc.hashBySize;
#ifndef TRANSWEB
    if (gl_is_web == 0)
    {
        total_bytes_print(index, fsize, mode, file_type, transfer_type);
    }
#endif
	hs.file_size = fsize;
    } /* End if for enable on */
    fflush(stdout);
} /* End tfile_hash_init */

void tfile_hash_close(void)
{
    DBG();
    extern int size_printed;

	if (size_printed == DBYES)
#ifdef SEND2FOUNDRY
    sprintf (rtn.msg, "%s transferred.  (100%%)", bytes_calc (hs.file_size));
    status_log("transfer_status", rtn.msg);
#else
    PRINTF ("%s transferred.  (100%%)\n", bytes_calc (hs.file_size));
#endif
} /* end tfile_hash_close */

#ifdef TRANSWEB
unsigned long long transweb_hash (int new_bytes_xfered, FILE *output_fp) 
{
    DBG();
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
	fprintf (output_fp,"%s transferred.", bytes_calc (hs.total_bytes_xfered));
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
#endif
#endif
