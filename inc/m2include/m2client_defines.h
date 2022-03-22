#ifndef m2client_defines_h
#define m2client_defines_h

/* Primary server is dropbox8, secondary server is dropbox6 */
#define DB_ADDRESS "10.81.198.121"
#define SEC_DB_ADDRESS "10.65.24.19"
#define TERSH_DB_ADDRESS "10.81.198.121"
#define QUAD_DB_ADDRESS "10.65.24.19"

/* Names of database machines */
#define DB_MACHINE "mds-db1"
#define SEC_DB_MACHINE "mds-db2"
#define TERSH_DB_MACHINE "mds-db1"
#define QUAD_DB_MACHINE "mds-db2"

/* Socket number for requests from users */
#define DB_SOCKET 6195

/* Address for Mesa Mask Shop */
#define MASKSHOP_ADDRESS "221.20.20.20"
#define MASKSHOP_MACHINE "cde-az34-019va"

/* Holds global client parameters */
typedef struct
{
    char password[32];
    int override_flag;
     int program;
    float vnum;
    char version_flag;
    char reg_verify;
} M2PARAMS;

extern M2PARAMS m2;

#endif
