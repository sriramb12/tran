#ifndef mask_server_macros_h
#define mask_server_macros_h
#define STATE_INPUT "%d %s %s %d %d %d %s %s %s %s %s %s %s %s %s %s %s %d %f %d %s"
#define STATE_AUDIT_INPUT "%d %s %*s %d %d %d %s %s %s %s %s %s %s %s %s %s %s %d %f %d %s"
#define STATE_CWD_INPUT "%*d %*s %*s %*d %*d %*d %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s"
#define STATE_PIN_INPUT "%*d %*s %*s %*d %*d %*d %*s %s"
#define STATE_OWNER "%*d %*s %*s %*d %*d %*d %*s %*s %*s %s"
#define C4MASK_INPUT "%*d %*s %s"
#define SITE_INPUT "%*d %*s %*s %*d %*d %*d %s"
#define STATE5_INPUT "%*d %*s %*s %d %*d %*d %s"
#define AUDIT_INPUT "%d %*s %*s %d %*d %*d %s %*s %*s %*s %*s %*s %*s %s %*s %s %*s %d %*f %*d %*s"
#define AUDIT_TMPL_INPUT "%d %*s %s %d %*d %*d %s %*s %*s %*s %*s %*s %*s %s %*s %s %*s %d %*f %d %s"
#define TMPL_INPUT "%d %s"
#define PC_INPUT "%*d %s"
#define PIN_INPUT "%*d %*s %*s %*d %*d %*d %s %s"
#define DSGN_INPUT "%d"
#define STATE3INPUT "%*d %*s %*s%*d %*d %*d %*s %*s %*s %*s %*s %*s %*s %s %s %s"
#define STATE4INPUT "%d %*s %*s %*d %*d %*d %*s %*s %*s %*s %s %*s %s"
#define STATE2INPUT "%*d %s %s %d %d %d %s %s %s %s %s %s %s %s %s %s %s %d %*f %d %s"

#define STATE_OUTPUT "%d %s %s\n%d %d %d %s %s\n%s %s %s %s %s\n\
%s %s %s %s\n%d %2.3f     \n%d %s\n"

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
	close_down2m(rename_err, file1, file2, RENAME_ERR); \
    }

#define MAKE_DIR(dir) \
{ \
    if (mkdir(dir, 0700) != SUCCESS) \
    { \
	dir_remove(dir); \
	if (mkdir(dir, 0700) != SUCCESS) close_down1q(mkdir_err, dir, MKDIR_ERR); \
    } \
}

#define MAKE_SOURCE_DIR(dir) \
{ \
    if (mkdir(dir, 0700) != SUCCESS) \
    { \
	dir_remove(dir); \
	if (mkdir(dir, 0700) != SUCCESS) close_down1q(mkdir_err, dir, MKDIR_ERR); \
	fl.source_flag = DBYES;\
    } \
}
#endif /* ifdef for mask_server_macros_h */
