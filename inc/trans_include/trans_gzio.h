#ifndef _TRANS_ZLIB_H
#define _TRANS_ZLIB_H


#ifndef __MWERKS__TRANS_CLIENT__
#ifdef __cplusplus
extern "C" {
#endif
#endif

#ifndef __MWERKS__TRANS_CLIENT__
extern
#endif


/*
  trans_gzopen() prepares the transcend socket to receive gzipped data.
  
  If delta > 0, print status every |delta| seconds.
  If delta < 0, print status every |delta| bytes.
  If delta = 0, print nothing.
*/
int trans_gzopen(const char *mode, unsigned long hash_size, int hash_enable, TRANS_FILE_TYPE file_type);

#ifndef __MWERKS__TRANS_CLIENT__
extern
#endif
       int trans_gzread (voidp buf, char get_block);
/*
  Reads the given number of uncompressed bytes from the transcend socket.
  If the input file was not in gzip format, trans_gzread copies the given
  number of bytes into the buffer.

  trans_gzread returns the number of uncompressed bytes actually read (0 for
  end of file, -1 for error).
*/

#ifndef __MWERKS__TRANS_CLIENT__
extern
#endif
       int trans_gzwrite (const voidp buf, unsigned len);
/*
  Writes the given number of uncompressed bytes into the transcend socket.
  trans_gzwrite returns the number of uncompressed bytes actually written
  (0 in case of error).
*/

#ifndef __MWERKS__TRANS_CLIENT__
extern
#endif
       int trans_gzclose (long *len);
/*
  Flushes all pending output if necessary and deallocates the (de)compression
  state of the transcend socket. The return value is the zlib error number.
*/

#ifndef __MWERKS__TRANS_CLIENT__
extern
#endif
       const char * trans_gzerror (int *errnum);
/*
     Returns the error message for the last error which occurred on the
   transend socket. errnum is set to zlib error number. If an error occurred
   in the file system and not in the compression library, errnum is set to
   Z_ERRNO and the application may consult errno to get the exact error code.
*/

#ifndef __MWERKS__TRANS_CLIENT__
#ifdef __cplusplus
}
#endif

#endif

#endif /* _ZLIB_H */
