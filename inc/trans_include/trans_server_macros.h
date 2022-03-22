#ifndef s_macros_h
#define s_macros_h

/* Formats used to pull info from global file */
#define TRANS_LINE_NUM 18
#define TRANS_LINE_LEN 256

/* file/directory, disk index, delete time, owner, email flag */
/* Format used when extending date of a file */
#define EXTEND_FORMAT "%c %d %ld %s %d %d %d %c %ld %d %s"
#define ICOM_EXTEND_FORMAT "%c %d %ld %s %d %d %d %c %ld %d %s %s"

/* Format used when performing cron delete */
#define CRON_DLT_FORMAT "%c %*d %ld %s %*d %*d %d %*c %*d %d"

/* Format used when getting a file */
#define TMP_GET_FORMAT "%c %d %*d %s %d %*d %d %c %d %*s %s %*s %*s %d %*s %*s %*d %*s %*s %s"
#define GET_FORMAT "%c %d %*d %s %d %*d %d %c %d %*d %*s %s %*s %*s %d %*s %*s %*d %*s %*s %s"

#define RENAME_DIR(dir1, dir2) \
    if (rename(dir1, dir2) != SUCCESS) \
    {\
	dlt_dir(dir2);\
	if ((rename(dir1, dir2)) != SUCCESS)\
	close_down2q(rename_err, dir1, dir2, RENAME_ERR);\
    }

#define RENAME_FILE(file1, file2) \
    if (rename(file1, file2) != SUCCESS) \
    { \
	unlink(file2); \
	if (rename(file1, file2) != SUCCESS) \
	close_down2q(rename_err, file1, file2, RENAME_ERR); \
    }

#define MAKE_DIR(dir) \
{ \
    if (mkdir(dir, 0700) != SUCCESS) \
    { \
	remove_dir(dir); \
	if (mkdir(dir, 0700) != SUCCESS) close_down1q(mkdir_err, dir, MKDIR_ERR); \
    } \
}

#define MAKE_SOURCE_DIR(dir) \
{ \
    if (mkdir(dir, 0700) != SUCCESS) \
    { \
	remove_dir(dir); \
	if (mkdir(dir, 0700) != SUCCESS) close_down1q(mkdir_err, dir, MKDIR_ERR); \
	source_flag = DBYES;\
    } \
}
#endif /* ifdef for trans_server_macros_h */
