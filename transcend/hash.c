#include <stdio.h>
#include <time.h>
#include "global_defines.h"
#include "server_hash.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "transrc.h"
#include "server_params.h"
#include "trans_prototypes.h"
#include "client_lib.h"
#include "zlib.h"
#include "global_debug.h"

typedef struct {
  int  enable;
  long long file_size;
  long long total_bytes_xfered;
  long report_time;
  long long report_size;
} xfer_status_t;

/* holds file size used by trans_hash_init and trans_hash */
xfer_status_t hs;

gzFile trans_open ( char *mode, long long hash_size, int hash_enable, TRANS_FILE_TYPE file_type) 
{
    DBG();

    gzFile file = trans_gzopen(mode);
#ifdef NXP
  if (*mode == 'r')
  trans_hash_init (hash_size, hash_enable, file_type, "from");
  else trans_hash_init (hash_size, hash_enable, file_type, "to");
#else
  trans_hash_init (hash_size, hash_enable, file_type);
#endif
    return(file);
} /* End trans_open */

int trans_read (gzFile file, voidp buf, char get_header) 
{
    DBG();
    int size = trans_gzread(file, buf, BLOCK_SIZE);
     return(trans_hash(size));
} /* End trans_read */

int mac_header_read (gzFile file, voidp buf, char get_header) 
{
    DBG();
    int size = trans_gzread(file, buf, 128);
     return(trans_hash(size));
} /* End mac_header_read */

/* ------------------------------------------------------------ trans_write */
void trans_write (gzFile file, const voidp buf, unsigned len) 
{
    DBG();
    int size = trans_gzwrite(file, buf, len);
    trans_hash(size);
} /* End trans_write */

/* ------------------------------------------------------------ trans_close */
long long trans_close (gzFile file, char file_type) 
{
    DBG();
    trans_gzclose(file);
     /* Return the final size */
    return(hs.total_bytes_xfered);
} /* End trans_close */

/* ---------------------------------------------------------------- num_bytes */
char *test(long long size)
{
    DBG();
    return(num_bytes(size));
}
char* num_bytes (long long bytes) 
{
    DBG();
  static char byte_string [64];

  if (bytes < 1024 * 1024)
	sprintf (byte_string, "%lld bytes", (long long)bytes);
    else if (bytes < 1024*1024 * 1024)
	sprintf (byte_string, "%0.2f MB", 1.0 * bytes / 1024 / 1024);
	else sprintf (byte_string, "%0.4f GB", 1.0 * bytes / 1024 / 1024/1024);
  return byte_string;
} /* End num_bytes */

/* ---------------------------------------------------------- trans_hash_init */
#ifdef NXP
void trans_hash_init (long long fsize, int enable, TRANS_FILE_TYPE file_type, char *to_from) 
#else
void trans_hash_init (long long fsize, int enable, TRANS_FILE_TYPE file_type)
#endif
{
/* Passed in total file size and byte to enable hashing */

    DBG();
  hs.enable = enable;
  hs.total_bytes_xfered = 0;

    if (enable)
    {
	if (transrc.hashByTime)
	  hs.report_time = time (NULL) + transrc.hashByTime;
	hs.report_size = transrc.hashBySize;
#ifdef NXP
#ifdef INTERCOM
	strexc(server.site, '_', ' ');
#endif
#ifdef M2
	strexc(server.site, '_', ' ');
#endif
	printf ("Data transfer %s %s started.\n", to_from, server.site);
#ifdef INTERCOM
	strexc(server.site, ' ', '_');
#endif
#ifdef M2
	strexc(server.site, ' ', '_');
#endif
#else
	printf ("Data transfer started.");
#endif
	if (file_type == DIRECTORY)
	{
	if (fsize < 1024*1024) fsize += fsize*.15;
	else if (fsize < 1024*1024*1024) fsize += fsize*.1;
	else fsize += fsize*.05;
	hs.file_size = fsize;
	    PRINTF ("  Estimated directory size is %s.", num_bytes (hs.file_size));
	} /* End if for directory */
	  else 
    {
	printf ("  File size is %s.", num_bytes (fsize));
	hs.file_size = fsize;
    }
	printf ("\n");
    } /* End if for enable on */
} /* End trans_hash_init */

/* --------------------------------------------------------------- trans_hash */
long long trans_hash (int new_bytes_xfered) 
{
  int print_now = 0;
  long current_time;

    DBG();
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
	printf ("%s transferred.", num_bytes (hs.total_bytes_xfered));
	/* If file size specified, print percentage */
	if (hs.file_size)
	{
	    /* First calculate percentage */
	    float x = 100.0 * hs.total_bytes_xfered / hs.file_size;
	    /* If over 100%, display it as 99.9 */
	    if (x >= 100.0) x = 99.9;
	    PRINTF ("  (%0.1f%%)", x); 
	} /* End if for file size specified */
	printf ("\n");
  }
  return new_bytes_xfered;
} /* end trans_hash */
