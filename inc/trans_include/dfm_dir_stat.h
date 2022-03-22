#ifndef dfm_dir_stat_h
#define dfm_dir_stat_h

#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#define TWO_GB 2147483647


#define	timestruc	timespec	/* structure name per SVr4 */
typedef struct timespec timestruc_t;	/* definition per SVr4 */

typedef struct
{
    char file_name[256];
    struct stat file_info;
} RET_FILE_INFO;

typedef struct
{
	dev_t		st_dev;
	long		st_pad1[3];	/* reserve for dev expansion, */
					/* sysid definition */
	long		st_ino;
	mode_t		st_mode;
	nlink_t		st_nlink;
	uid_t		st_uid;
	gid_t		st_gid;
	dev_t		st_rdev;
	long		st_pad2[2];
	long		st_size;
	long		st_pad3;	/* pad for future off_t expansion */
	long	st_atim;
	long	st_mtim;
	long	st_ctim;
	long	st_blksize;
	long	st_blocks;
	char		st_fstype[16];
	long		st_pad4[8];	/* expansion area */
} OPC32STAT;

typedef struct
{
    char file_name[256];
    OPC32STAT file_info;
} OPC32INFO;

typedef struct
{
    mode_t st_mode;
    long long st_size;
    long st_atim;
    long st_mtim;
    long st_ctim;
} OPC64STAT;


/* Structure for 64 bit mode */
typedef struct
{
    char file_name[256];
    OPC64STAT file_info;
} OPC64INFO;

#endif /* define for dfm_dir_h */
