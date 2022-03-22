#ifdef TRANSAPI
/* WARNING:  This software was modified by NXP. */
/* This version of gzio reads and writes to a socket instead of a file */
/* This file contains the following changes */
/* Change Z_BUFSIZE to BLOCK_SIZE */
/* Change fprintf and fwrite to put_binary string */
/* Change fread to get_ssl_string */
/* Change putLong to trans_putLong */
/* Change arguments in putLong so stream pointer not passed in */
/* Change getLong to trans_getLong */
/* Change get_byte to trans_get_byte */
/* Comment out opening and closing files because opening and closing socket done else where */
/* Change gzopen to tfile_gzopen */
/* Change gzclose to tfile_gzclose */
/* Change gzread to tfile_gzread */
/* Change gzwrite to tfile_gzwrite */
/* Change gzdopen to tfile_gzdopen, not used by trans */
/* Change gzflush to tfile_gzflush, not used by Transcend */
/* Change destroy to trans_destroy */
/* Took out references to ferror because not applicable here */
/* Took out references to paths because writing to socket instead */
/* All references to path must be taken out */
/* The code to transfer data when no compression done re-written */
/* This code is invoked when transparent is set to 1 */

#define SUCCESS 0
#define BLOCK_SIZE 4096
/* gzio.c -- IO on .gz files
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 *
 * Compile this file with -DNO_DEFLATE to avoid the compression code.
 */

/* @(#) $Id: gzio.c,v 1.20 2011/05/21 00:03:05 carter Exp $ */

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "ssl_client_lib.h"
#include "trans_hash.h"

#include "zutil.h"

/* struct internal_state {int dummy;};  for buggy compilers */

#ifndef Z_BUFSIZE
#  ifdef MAXSEG_64K
#    define Z_BUFSIZE 4096 /* minimize memory usage for 16-bit DOS */
#  else
#    define Z_BUFSIZE 16384
#  endif
#endif
#ifndef Z_PRINTF_BUFSIZE
#  define Z_PRINTF_BUFSIZE 4096
#endif

#define ALLOC(size) malloc(size)
#define TRYFREE(p) {\
if (p) free(p); p = NULL;}

static int gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */
#define GZ_MAGIC1 0x1f
#define GZ_MAGIC2 0x8b

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

typedef struct gz_stream {
    z_stream stream;
    int      z_err;   /* error code for last stream operation */
    int      z_eof;   /* set if end of input file */
    FILE     *file;   /* .gz file */
    Byte     *inbuf;  /* input buffer */
    Byte     *outbuf; /* output buffer */
    uLong    crc;     /* crc32 of uncompressed data */
    char     *msg;    /* error message */
    int      transparent; /* 1 if input file is not a .gz file */
    char     mode;    /* 'w' or 'r' */
    long     startpos; /* start of compressed data in file (header skipped) */
} gz_stream;


local gzFile gz_open      OF((char *mode, int  fd));
local int do_flush        OF((gzFile file, int flush));
local int    trans_get_byte     OF((gz_stream *s));
local void   check_header OF((gz_stream *s));
local int    trans_destroy      OF((gz_stream *s));
local void   trans_putLong      OF((uLong x));
local uLong  trans_getLong      OF((gz_stream *s));
gzFile ZEXPORT tfile_gzopen (char *mode);
gzFile ZEXPORT tfile_gzdopen (int fd, char *mode);
int ZEXPORT tfile_gzread (gzFile file, voidp buf, unsigned len);
int ZEXPORT tfile_gzwrite (gzFile file, const voidp buf, unsigned len);
int ZEXPORT tfile_gzclose (gzFile file);
const char*  ZEXPORT tfile_gzerror (gzFile file, int *errnum);
int ZEXPORT tfile_gzflush (gzFile file, int flush);

/* ===========================================================================
     Opens a gzip (.gz) file for reading or writing. The mode parameter
   is as in fopen ("rb" or "wb"). The file is given either by file descriptor
   or path name (if fd == -1).
     gz_open return NULL if the file could not be opened or if there was
   insufficient memory to allocate the (de)compression state; errno
   can be checked to distinguish the two cases (if errno is zero, the
   zlib error is Z_MEM_ERROR).
*/
local gzFile gz_open (char *mode, int  fd)
{
    int err;
    int level = Z_DEFAULT_COMPRESSION; /* compression level */
    int strategy = Z_DEFAULT_STRATEGY; /* compression strategy */
    char *p = (char*)mode;
    gz_stream *s;
    char fmode[80]; /* copy of mode, without the compression level */
    char *m = fmode;

    if  (!fd) return (Z_NULL);
    if  (!mode) return (Z_NULL);

    s = (gz_stream *)ALLOC(sizeof(gz_stream));
    if (!s) return Z_NULL;

    s->stream.zalloc = (alloc_func)0;
    s->stream.zfree = (free_func)0;
    s->stream.opaque = (voidpf)0;
    s->stream.next_in = s->inbuf = Z_NULL;
    s->stream.next_out = s->outbuf = Z_NULL;
    s->stream.avail_in = s->stream.avail_out = 0;
    s->file = NULL;
    s->z_err = Z_OK;
    s->z_eof = 0;
    s->crc = crc32(0L, Z_NULL, 0);
    s->msg = NULL;
    s->transparent = 0;


    s->mode = '\0';
    do {
        if (*p == 'r') s->mode = 'r';
        if (*p == 'w' || *p == 'a') s->mode = 'w';
        if (*p >= '0' && *p <= '9') {
	    level = *p - '0';
	} else if (*p == 'f') {
	  strategy = Z_FILTERED;
	} else if (*p == 'h') {
	  strategy = Z_HUFFMAN_ONLY;
	} else {
	    *m++ = *p; /* copy the mode */
	}
    } while (*p++ && m != fmode + sizeof(fmode));
    if (s->mode == '\0') return trans_destroy(s), (gzFile)Z_NULL;
    
    if (s->mode == 'w') {
#ifdef NO_DEFLATE
        err = Z_STREAM_ERROR;
#else
        err = deflateInit2(&(s->stream), level,
                           Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, strategy);
        /* windowBits is passed < 0 to suppress zlib header */

        s->stream.next_out = s->outbuf = (Byte*)ALLOC(BLOCK_SIZE);
#endif
        if (err != Z_OK || s->outbuf == Z_NULL) {
            return trans_destroy(s), (gzFile)Z_NULL;
        }
    } else {
        s->stream.next_in  = s->inbuf = (Byte*)ALLOC(Z_BUFSIZE);

        err = inflateInit2(&(s->stream), -MAX_WBITS);
        /* windowBits is passed < 0 to tell that there is no zlib header.
         * Note that in this case inflate *requires* an extra "dummy" byte
         * after the compressed stream in order to complete decompression and
         * return Z_STREAM_END. Here the gzip CRC32 ensures that 4 bytes are
         * present after the compressed stream.
         */
        if (err != Z_OK || s->inbuf == Z_NULL) {
            return trans_destroy(s), (gzFile)Z_NULL;
        }
    }
    s->stream.avail_out = BLOCK_SIZE;

    errno = 0;
    if (s->mode == 'w') {
        /* Write a very simple .gz header:
         */
	/* Change fprintf to sprint to write to socket instead of file */
	{ char buffer[12];
        sprintf(buffer, "%c%c%c%c%c%c%c%c%c%c", GZ_MAGIC1, GZ_MAGIC2,
             Z_DEFLATED, 0 /*flags*/, 0,0,0,0 /*time*/, 0 /*xflags*/, OS_CODE);
	put_ssl_binary_string(buffer, 10);
	} /* End block for declaration of temporary buffer */
	/* End of changed software */
	s->startpos = 10L;
	/* We use 10L instead of ftell(s->file) to because ftell causes an
         * fflush on some systems. This version of the library doesn't use
         * startpos anyway in write mode, so this initialization is not
         * necessary.
         */
    } else {
	check_header(s); /* skip the .gz header */
	s->startpos = 10L;
    }
    
    return (gzFile)s;
}

/* ===========================================================================
     Opens a gzip (.gz) file for reading or writing.
*/
gzFile ZEXPORT tfile_gzopen (char *mode)
{
    return gz_open (mode, -1);
}

/* ===========================================================================
     Associate a gzFile with the file descriptor fd. fd is not dup'ed here
   to mimic the behavio(u)r of fdopen.
*/
gzFile ZEXPORT tfile_gzdopen (int fd, char *mode)
{
    char name[20];

    if (fd < 0) return (gzFile)Z_NULL;
    sprintf(name, "<fd:%d>", fd); /* for debugging */

    return gz_open (mode, fd);
}

/* ===========================================================================
 * Update the compression level and strategy
 */
int ZEXPORT gzsetparams (file, level, strategy)
    gzFile file;
    int level;
    int strategy;
{
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

    /* Make room to allow flushing */
    if (s->stream.avail_out == 0) {

	s->stream.next_out = s->outbuf;
	if (put_ssl_binary_string((char *)s->outbuf, BLOCK_SIZE) != SUCCESS)
	{
		    /* End of data changed */
	    s->z_err = Z_ERRNO;
	}
	s->stream.avail_out = BLOCK_SIZE;
    }

    return deflateParams (&(s->stream), level, strategy);
}

/* ===========================================================================
     Read a byte from a gz_stream; update next_in and avail_in. Return EOF
   for end of file.
   IN assertion: the stream s has been sucessfully opened for reading.
*/
local int trans_get_byte(s)
    gz_stream *s;
{
    if (s->z_eof) return EOF;
    if (s->stream.avail_in == 0) {
	errno = 0;
	/* fread changed to get_ssl_string to read from socket instead of file */
	s->stream.avail_in = get_ssl_string((char *)s->inbuf);
	if (s->stream.avail_in == 0) {
	    s->z_eof = 1;
	    return EOF;
	}
	s->stream.next_in = s->inbuf;
    }
    s->stream.avail_in--;
    return *(s->stream.next_in)++;
}

/* ===========================================================================
      Check the gzip header of a gz_stream opened for reading. Set the stream
    mode to transparent if the gzip magic header is not present; set s->err
    to Z_DATA_ERROR if the magic header is present but the rest of the header
    is incorrect.
    IN assertion: the stream s has already been created sucessfully;
       s->stream.avail_in is zero for the first time, but may be non-zero
       for concatenated .gz files.
*/
local void check_header(s)
    gz_stream *s;
{
    int method; /* method byte */
    int flags;  /* flags byte */
    uInt len;
    int c;

    /* Check the gzip magic header */
    for (len = 0; len < 2; len++) {
	c = trans_get_byte(s);
	if (c != gz_magic[len]) {
	    if (len != 0) s->stream.avail_in++, s->stream.next_in--;
	    if (c != EOF) {
		s->stream.avail_in++, s->stream.next_in--;
		s->transparent = 1;
	    }
	    s->z_err = s->stream.avail_in != 0 ? Z_OK : Z_STREAM_END;
	    return;
	}
    }
    method = trans_get_byte(s);
    flags = trans_get_byte(s);
    if (method != Z_DEFLATED || (flags & RESERVED) != 0) {
	s->z_err = Z_DATA_ERROR;
	return;
    }

    /* Discard time, xflags and OS code: */
    for (len = 0; len < 6; len++) (void)trans_get_byte(s);

    if ((flags & EXTRA_FIELD) != 0) { /* skip the extra field */
	len  =  (uInt)trans_get_byte(s);
	len += ((uInt)trans_get_byte(s))<<8;
	/* len is garbage if EOF but the loop below will quit anyway */
	while (len-- != 0 && trans_get_byte(s) != EOF) ;
    }
    if ((flags & ORIG_NAME) != 0) { /* skip the original file name */
	while ((c = trans_get_byte(s)) != 0 && c != EOF) ;
    }
    if ((flags & COMMENT) != 0) {   /* skip the .gz file comment */
	while ((c = trans_get_byte(s)) != 0 && c != EOF) ;
    }
    if ((flags & HEAD_CRC) != 0) {  /* skip the header crc */
	for (len = 0; len < 2; len++) (void)trans_get_byte(s);
    }
    s->z_err = s->z_eof ? Z_DATA_ERROR : Z_OK;
}

 /* ===========================================================================
 * Cleanup then free the given gz_stream. Return a zlib error code.
   Try freeing in the reverse order of allocations.
 */
local int trans_destroy (s)
    gz_stream *s;
{
    int err = Z_OK;

    if (!s) return Z_STREAM_ERROR;

    TRYFREE(s->msg);

    if (s->stream.state != NULL) {
	if (s->mode == 'w') {
#ifdef NO_DEFLATE
	    err = Z_STREAM_ERROR;
#else
	    err = deflateEnd(&(s->stream));
#endif
	} else if (s->mode == 'r') {
	    err = inflateEnd(&(s->stream));
	}
    }
    if (s->z_err < 0) err = s->z_err;

    TRYFREE(s->outbuf);
    TRYFREE(s->inbuf);
    TRYFREE(s);
    return err;
}

/* ===========================================================================
     Reads the given number of uncompressed bytes from the compressed file.
   tfile_gzread returns the number of bytes actually read (0 for end of file).
*/
int ZEXPORT tfile_gzread (gzFile file, voidp buf, unsigned len)
{
    gz_stream *s = (gz_stream*)file;
    Bytef *start = (Bytef*)buf; /* starting point for crc computation */
    Byte  *next_out; /* == stream.next_out but not forced far (for MSDOS) */

    if (s == NULL || s->mode != 'r') return Z_STREAM_ERROR;

    if (s->z_err == Z_DATA_ERROR || s->z_err == Z_ERRNO) return -1;
    if (s->z_err == Z_STREAM_END) return 0;  /* EOF */

    next_out = (Byte*)buf;
    s->stream.next_out = (Bytef*)buf;
    s->stream.avail_out = len;

    while (s->stream.avail_out != 0) {

  if (s->transparent) 
  {
	uInt n = s->stream.avail_in;
	if (n > 0) {
	  /* Copy first the lookahead bytes: */
	  /* Copy data to the output buffer  from input buffer */
	  zmemcpy(s->stream.next_out, s->stream.next_in, n);
	  next_out += n;
	  /* Point to the next availahble output position */
	  s->stream.next_out = next_out;
	  /* Point to the next available input position */
	  s->stream.next_in   += n;
	  s->stream.avail_out -= n;
	  s->stream.avail_in  -= n;
	} else {
	  /* Copy a block from the server */
	  n = get_ssl_string ( (char*) s->stream.next_out);
	  next_out += n;
	  /* Point to the new output position */
	  s->stream.next_out = next_out;
	  /* Indicate the number of bytes available to be output */
	  s->stream.avail_out -= n;
	}	
#ifndef TRANSWEB
	return tfile_hash(BLOCK_SIZE - s->stream.avail_out, 0, (int)index);
#endif
  } /* End if for transparent data */
        if (s->stream.avail_in == 0 && !s->z_eof) {

            errno = 0;
	    /* change fread to get_ssl_string to read from socket instead of buffer */
	    s->stream.avail_in = get_ssl_string((char *)s->inbuf);
            if (s->stream.avail_in == 0) {
                s->z_eof = 1;
            }
            s->stream.next_in = s->inbuf;
        }
        s->z_err = inflate(&(s->stream), Z_NO_FLUSH);

	if (s->z_err == Z_STREAM_END) {
	    /* Check CRC and original size */
	    s->crc = crc32(s->crc, start, (uInt)(s->stream.next_out - start));
	    start = s->stream.next_out;

	/* Change trans_getLong to trans_getLong to write to socket instead of file */
	    if (trans_getLong(s) != s->crc) {
		s->z_err = Z_DATA_ERROR;
	    } else {
	        (void)trans_getLong(s);
                /* The uncompressed length returned by above getlong() may
                 * be different from s->stream.total_out) in case of
		 * concatenated .gz files. Check for such files:
		 */
		check_header(s);
		if (s->z_err == Z_OK) {
		    uLong total_in = s->stream.total_in;
		    uLong total_out = s->stream.total_out;

		    inflateReset(&(s->stream));
		    s->stream.total_in = total_in;
		    s->stream.total_out = total_out;
		    s->crc = crc32(0L, Z_NULL, 0);
		}
	    }
	}
	if (s->z_err != Z_OK || s->z_eof) break;
    }
    s->crc = crc32(s->crc, start, (uInt)(s->stream.next_out - start));

    return (int)(len - s->stream.avail_out);
}


/* ===========================================================================
      Reads one byte from the compressed file. gzgetc returns this byte
   or -1 in case of end of file or error.
*/
int ZEXPORT gzgetc(gzFile file)
{
    unsigned char c;

    return tfile_gzread(file, &c, 1) == 1 ? c : -1;
}


/* ===========================================================================
      Reads bytes from the compressed file until len-1 characters are
   read, or a newline character is read and transferred to buf, or an
   end-of-file condition is encountered.  The string is then terminated
   with a null character.
      gzgets returns buf, or Z_NULL in case of error.

      The current implementation is not optimized at all.
*/
char * ZEXPORT gzgets(file, buf, len)
    gzFile file;
    char *buf;
    int len;
{
    char *b = buf;
    if (buf == Z_NULL || len <= 0) return Z_NULL;

    while (--len > 0 && tfile_gzread(file, buf, 1) == 1 && *buf++ != '\n') ;
    *buf = '\0';
    return b == buf && len > 0 ? Z_NULL : b;
}


#ifndef NO_DEFLATE
/* ===========================================================================
     Writes the given number of uncompressed bytes into the compressed file.
   tfile_gzwrite returns the number of bytes actually written (0 in case of error).
*/
int ZEXPORT tfile_gzwrite (gzFile file, const voidp buf, unsigned len)
{
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

    s->stream.next_in = (Bytef*)buf;
    s->stream.avail_in = len;

    while (s->stream.avail_in != 0) {

        if (s->stream.avail_out == 0) {

            s->stream.next_out = s->outbuf;
	    if (put_ssl_binary_string((char *)s->outbuf, BLOCK_SIZE) != SUCCESS)
	    {
	    /* End of text changed */
                s->z_err = Z_ERRNO;
                break;
            }
            s->stream.avail_out = BLOCK_SIZE;
        }
        s->z_err = deflate(&(s->stream), Z_NO_FLUSH);
        if (s->z_err != Z_OK) break;
    }
    s->crc = crc32(s->crc, (const Bytef *)buf, len);

    return (int)(len - s->stream.avail_in);
} /* end tfile_gzwrite */

/* ===========================================================================
     Converts, formats, and writes the args to the compressed file under
   control of the format string, as in fprintf. gzprintf returns the number of
   uncompressed bytes actually written (0 in case of error).
*/
#ifdef STDC
#include <stdarg.h>

int ZEXPORTVA gzprintf (gzFile file, const char *format, /* args */ ...)
{
    char buf[Z_PRINTF_BUFSIZE];
    va_list va;
    int len;

    va_start(va, format);
#ifdef HAS_vsnprintf
    (void)vsnprintf(buf, sizeof(buf), format, va);
#else
    (void)vsprintf(buf, format, va);
#endif
    va_end(va);
    len = strlen(buf); /* some *sprintf don't return the nb of bytes written */
    if (len <= 0) return 0;

    return tfile_gzwrite(file, buf, (unsigned)len);
}
#else /* not ANSI C */

int ZEXPORTVA gzprintf (file, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
	               a11, a12, a13, a14, a15, a16, a17, a18, a19, a20)
    gzFile file;
    const char *format;
    int a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
	a11, a12, a13, a14, a15, a16, a17, a18, a19, a20;
{
    char buf[Z_PRINTF_BUFSIZE];
    int len;

#ifdef HAS_snprintf
    snprintf(buf, sizeof(buf), format, a1, a2, a3, a4, a5, a6, a7, a8,
	     a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20);
#else
    sprintf(buf, format, a1, a2, a3, a4, a5, a6, a7, a8,
	    a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20);
#endif
    len = strlen(buf); /* old sprintf doesn't return the nb of bytes written */
    if (len <= 0) return 0;

    return tfile_gzwrite(file, buf, len);
}
#endif

/* ===========================================================================
      Writes c, converted to an unsigned char, into the compressed file.
   gzputc returns the value that was written, or -1 in case of error.
*/
int ZEXPORT gzputc(gzFile file, int c)
{
    unsigned char cc = (unsigned char) c; /* required for big endian systems */

    return tfile_gzwrite(file, &cc, 1) == 1 ? (int)cc : -1;
}


/* ===========================================================================
      Writes the given null-terminated string to the compressed file, excluding
   the terminating null character.
      gzputs returns the number of characters written, or -1 in case of error.
*/
int ZEXPORT gzputs(gzFile file, const char *s)
{
    return tfile_gzwrite(file, (char*)s, (unsigned)strlen(s));
}


/* ===========================================================================
     Flushes all pending output into the compressed file. The parameter
   flush is as in the deflate() function.
*/
local int do_flush (gzFile file, int flush)
{
    uInt len;
    int done = 0;
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

    s->stream.avail_in = 0; /* should be zero already anyway */

    for (;;) {
        len = BLOCK_SIZE - s->stream.avail_out;

        if (len != 0) {
	    if (put_ssl_binary_string((char *)s->outbuf, len) != SUCCESS)
	    {
		/* End of change */
                s->z_err = Z_ERRNO;
                return Z_ERRNO;
            }
            s->stream.next_out = s->outbuf;
            s->stream.avail_out = BLOCK_SIZE;
        }
        if (done) break;
        s->z_err = deflate(&(s->stream), flush);

	/* Ignore the second of two consecutive flushes: */
	if (len == 0 && s->z_err == Z_BUF_ERROR) s->z_err = Z_OK;

        /* deflate has finished flushing only when it hasn't used up
         * all the available space in the output buffer: 
         */
        done = (s->stream.avail_out != 0 || s->z_err == Z_STREAM_END);
 
        if (s->z_err != Z_OK && s->z_err != Z_STREAM_END) break;
    }
    return  s->z_err == Z_STREAM_END ? Z_OK : s->z_err;
}

int ZEXPORT tfile_gzflush (gzFile file, int flush)
{
    gz_stream *s = (gz_stream*)file;
    int err = do_flush (file, flush);

    if (err) return err;
    fflush(s->file);
    return  s->z_err == Z_STREAM_END ? Z_OK : s->z_err;
}
#endif /* NO_DEFLATE */

/* ===========================================================================
      Sets the starting position for the next tfile_gzread or tfile_gzwrite on the given
   compressed file. The offset represents a number of bytes in the
      gzseek returns the resulting offset location as measured in bytes from
   the beginning of the uncompressed stream, or -1 in case of error.
      SEEK_END is not implemented, returns error.
      In this version of the library, gzseek can be extremely slow.
*/
z_off_t ZEXPORT gzseek (gzFile file, z_off_t offset, int whence)
{
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || whence == SEEK_END ||
	s->z_err == Z_ERRNO || s->z_err == Z_DATA_ERROR) {
	return -1L;
    }
    
    if (s->mode == 'w') {
#ifdef NO_DEFLATE
	return -1L;
#else
	if (whence == SEEK_SET) {
	    offset -= s->stream.total_in;
	}
	if (offset < 0) return -1L;

	/* At this point, offset is the number of zero bytes to write. */
	if (s->inbuf == Z_NULL) {
	    s->inbuf = (Byte*)ALLOC(BLOCK_SIZE); /* for seeking */
	    zmemzero(s->inbuf, BLOCK_SIZE);
	}
	while (offset > 0)  {
	    uInt size = BLOCK_SIZE;
	    if (offset < BLOCK_SIZE) size = (uInt)offset;

	    size = tfile_gzwrite(file, s->inbuf, size);
	    if (size == 0) return -1L;

	    offset -= size;
	}
	return (z_off_t)s->stream.total_in;
#endif
    }
    /* Rest of function is for reading only */

    /* compute absolute position */
    if (whence == SEEK_CUR) {
	offset += s->stream.total_out;
    }
    if (offset < 0) return -1L;

    if (s->transparent) {
	/* map to fseek */
	s->stream.avail_in = 0;
	s->stream.next_in = s->inbuf;
        if (fseek(s->file, offset, SEEK_SET) < 0) return -1L;

	s->stream.total_in = s->stream.total_out = (uLong)offset;
	return offset;
    }

    /* For a negative seek, rewind and use positive seek */
    if ((uLong)offset >= s->stream.total_out) {
	offset -= s->stream.total_out;
    } else if (gzrewind(file) < 0) {
	return -1L;
    }
    /* offset is now the number of bytes to skip. */

    if (offset != 0 && s->outbuf == Z_NULL) {
	s->outbuf = (Byte*)ALLOC(BLOCK_SIZE);
    }
    while (offset > 0)  {
	int size = BLOCK_SIZE;
	if (offset < BLOCK_SIZE) size = (int)offset;

	size = tfile_gzread(file, s->outbuf, (uInt)size);
	if (size <= 0) return -1L;
	offset -= size;
    }
    return (z_off_t)s->stream.total_out;
}

/* ===========================================================================
     Rewinds input file. 
*/
int ZEXPORT gzrewind (gzFile file)
{
    gz_stream *s = (gz_stream*)file;
    
    if (s == NULL || s->mode != 'r') return -1;

    s->z_err = Z_OK;
    s->z_eof = 0;
    s->stream.avail_in = 0;
    s->stream.next_in = s->inbuf;
    s->crc = crc32(0L, Z_NULL, 0);
	
    if (s->startpos == 0) { /* not a compressed file */
	rewind(s->file);
	return 0;
    }

    (void) inflateReset(&s->stream);
    return fseek(s->file, s->startpos, SEEK_SET);
}

/* ===========================================================================
     Returns the starting position for the next tfile_gzread or tfile_gzwrite on the
   given compressed file. This position represents a number of bytes in the
   uncompressed data stream.
*/
z_off_t ZEXPORT gztell (gzFile file)
{
    return gzseek(file, 0L, SEEK_CUR);
}

/* ===========================================================================
     Returns 1 when EOF has previously been detected reading the given
   input stream, otherwise zero.
*/
int ZEXPORT gzeof (gzFile file)
{
    gz_stream *s = (gz_stream*)file;
    
    return (s == NULL || s->mode != 'r') ? 0 : s->z_eof;
}

/* ===========================================================================
   Outputs a long in LSB order to the given file
*/
local void trans_putLong (uLong x)
{
    int n;
    char str[4];
    /* Modified to use put_ssl_string instead of putc */
    for (n = 0; n < 4; n++) {
	str[n] = (x&0xff);
	x >>= 8;
  }
  put_ssl_binary_string (str, 4);
}

/* ===========================================================================
   Reads a long in LSB order from the given gz_stream. Sets z_err in case
   of error.
*/
local uLong trans_getLong (gz_stream *s)
{
    uLong x = (uLong)trans_get_byte(s);
    int c;

    x += ((uLong)trans_get_byte(s))<<8;
    x += ((uLong)trans_get_byte(s))<<16;
    c = trans_get_byte(s);
    if (c == EOF) s->z_err = Z_DATA_ERROR;
    x += ((uLong)c)<<24;
    return x;
}

/* ===========================================================================
     Flushes all pending output if necessary, closes the compressed file
   and deallocates all the (de)compression state.
*/
int ZEXPORT tfile_gzclose (gzFile file)
{
    int err;
    gz_stream *s = (gz_stream*)file;

    if (s == NULL) return Z_STREAM_ERROR;

    if (s->mode == 'w') {
#ifdef NO_DEFLATE
	return Z_STREAM_ERROR;
#else
        err = do_flush (file, Z_FINISH);
        if (err != Z_OK) return trans_destroy((gz_stream*)file);

	/* change trans_putLong to trans_putLong to write to socket instead of file */
	trans_putLong(s->crc);
	trans_putLong(s->stream.total_in);
#endif
    }
    return trans_destroy((gz_stream*)file);
}

/* ===========================================================================
     Returns the error message for the last error which occured on the
   given compressed file. errnum is set to zlib error number. If an
   error occured in the file system and not in the compression library,
   errnum is set to Z_ERRNO and the application may consult errno
   to get the exact error code.
*/
const char*  ZEXPORT tfile_gzerror (gzFile file, int *errnum)
{
    char *m;
    gz_stream *s = (gz_stream*)file;

    if (s == NULL) {
        *errnum = Z_STREAM_ERROR;
        return (const char*)ERR_MSG(Z_STREAM_ERROR);
    }
    *errnum = s->z_err;
    if (*errnum == Z_OK) return (const char*)"";

    m =  (char*)(*errnum == Z_ERRNO ? strerror(errno) : s->stream.msg);

    if (m == NULL || *m == '\0') m = (char*)ERR_MSG(s->z_err);

    TRYFREE(s->msg);
    s->msg = (char*)ALLOC(18 + strlen(m) + 3);
    strcpy(s->msg, "Transport socket");
    strcat(s->msg, ": ");
    strcat(s->msg, m);
    return (const char*)s->msg;
}
#endif
