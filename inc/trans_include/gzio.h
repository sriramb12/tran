#ifndef gzio_h
#define gzio_h

#include <zlib.h>
#include "zutil.h"
/* ===========================================================================
      Writes c, converted to an unsigned char, into the compressed file.
   gzputc returns the value that was written, or -1 in case of error.
*/
int  gzputc(
    gzFile file,
    int c);


/* ===========================================================================
      Writes the given null-terminated string to the compressed file, excluding
   the terminating null character.
      gzputs returns the number of characters written, or -1 in case of error.
*/
int ZEXPORT gzputs(
    gzFile file,
    const char *s);


/* ===========================================================================
     Flushes all pending output into the compressed file. The parameter
   flush is as in the deflate(); function.
*/
/*
static int do_flush (
    gzFile file,
    int flush);

*/
int ZEXPORT trans_gzflush (
     gzFile file,
     int flush);

/* ===========================================================================
      Sets the starting position for the next trans_gzread or trans_gzwrite on the given
   compressed file. The offset represents a number of bytes in the
      gzseek returns the resulting offset location as measured in bytes from
   the beginning of the uncompressed stream, or -1 in case of error.
      SEEK_END is not implemented, returns error.
      In this version of the library, gzseek can be extremely slow.
*/
z_off_t ZEXPORT gzseek (
    gzFile file,
    z_off_t offset,
    int whence);

/* ===========================================================================
     Rewinds input file. 
*/
int ZEXPORT gzrewind (
    gzFile file);

/* ===========================================================================
     Returns the starting position for the next trans_gzread or trans_gzwrite on the
   given compressed file. This position represents a number of bytes in the
   uncompressed data stream.
*/
z_off_t ZEXPORT gztell (
    gzFile file);

/* ===========================================================================
     Returns 1 when EOF has previously been detected reading the given
   input stream, otherwise zero.
*/
int ZEXPORT gzeof (
    gzFile file);

/* ===========================================================================
   Outputs a long in LSB order to the given file
*/
/*
 static void trans_putLong (
    uLong x);

*/
/* ===========================================================================
   Reads a long in LSB order from the given gz_stream. Sets z_err in case
   of error.
static  unsigned long trans_getLong (
     int *s);
*/

/* ===========================================================================
     Flushes all pending output if necessary, closes the compressed file
   and deallocates all the (
*/
int ZEXPORT trans_gzclose (
    gzFile file);

/* ===========================================================================
     Returns the error message for the last error which occured on the
   given compressed file. errnum is set to zlib error number. If an
   error occured in the file system and not in the compression library,
   errnum is set to Z_ERRNO and the application may consult errno
   to get the exact error code.
*/
const char*  ZEXPORT trans_gzerror (
    gzFile file,
    int *errnum);

/* ===========================================================================
     Opens a gzip (.gz) file for reading or writing.
*/
gzFile ZEXPORT trans_gzopen (const char *mode);

/* ===========================================================================
     Reads the given number of uncompressed bytes from the compressed file.
   trans_gzread returns the number of bytes actually read (0 for end of file).
*/
int ZEXPORT trans_gzread (
    gzFile file,
    voidp buf,
    unsigned len);

/*      Writes the given number of uncompressed bytes into the compressed file. */
int ZEXPORT trans_gzwrite (
    gzFile file,
    const voidp buf,
    unsigned len);

#endif
