#ifndef solaris_stat_h
#define solaris_stat_h

#define	_ST_FSTYPSZ 16		/* array size for file system type name */

typedef struct  timespec {              /* definition per POSIX.4 */
        time_t          tv_sec;         /* seconds */
        long            tv_nsec;        /* and nanoseconds */
} timespec_t;

typedef struct timespec timestruc_t;    /* definition per SVr4 */

struct  solaris_stat {
        unsigned long   st_dev;
        long    st_pad1[3];     /* reserved for network id */
        ino_t   st_ino;
        unsigned long  st_mode;
        unsigned long st_nlink;
        long   st_uid;
        long   st_gid;
        unsigned long   st_rdev;
        long    st_pad2[2];
        off_t   st_size;
        long    st_pad3;        /* future off_t expansion */
        timestruc_t st_atim;
        timestruc_t st_mtim;
        timestruc_t st_ctim;
        long    st_blksize;
        long    st_blocks;
        char    st_fstype[_ST_FSTYPSZ];
        long    st_pad4[8];     /* expansion area */
};

#endif
