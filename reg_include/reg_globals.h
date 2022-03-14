#ifndef reg_globals_h
#define reg_globals_h

/* Holds global variables put in the Email registry */
#define RMT_X500FILE "cserv1:emssend.x500/x500.fullz"
#define TMP1X500FILE "tmp_registry/off.reg.tar.Z"
#define TMP2X500FILE "tmp_registry/off.reg"
#define X500FILE "registry/off.reg"
#define OFF_REG_FILE "registry/off.reg"
#define LOCAL_REG_FILE "registry/loc.reg"
#define EMAIL_REG_PATH "registry/email_reg."
#define RMT_PWD_KEY "mask_shop"
#define TRANS_KEY "big_file"

typedef struct
{
char email1[64];
char email2[64];
    char name1[16];
    char name2[16];
    char phone[16];
} REG_PARAMS;

/* Holds data from official registry used to look up users */
typedef struct
{
    char *email1;
    char *email2;
    int offset;
} REGISTRY;

REGISTRY off_reg[100000];
REGISTRY loc_reg[300];
int off_cnt;
int loc_cnt;


#ifdef M2
#define RMT_SERVER_TABLE "db_admin/rmt_servers"
#else
#ifdef MASKPREP
#define RMT_SERVER_TABLE "db_data/rmt_servers"
#else
#ifdef TRANSWEB
#define RMT_SERVER_TABLE "db_admin/rmt_transweb_servers"
#else
#ifdef SSL_ENCRYPTION
#define RMT_SERVER_TABLE "db_admin/ssl.rmt_servers"
#else
#define RMT_SERVER_TABLE "db_admin/rmt_servers"
#endif
#endif
#endif
#endif

#define DBNO_REQ 'm'
#define DBRMT_FORCE_REG 39
#define DBRMT_MAN_REG 64
#define GLBL 'i'
#define REGISTER_REQ 'o'


REG_PARAMS reg;
#endif /* define for reg_globals_h */
