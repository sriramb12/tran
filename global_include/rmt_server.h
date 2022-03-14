#ifndef rmt_servers_h
#define rmt_servers_h

#include "global_defines.h"


/* Info about remote servers */
typedef struct
{
    int server_num;	/* Number of remote servers */
    int server_index;	/* Index of first server to update */
    char log_file[20];	/* Holds log name for remote transactions */
    char site[TOTAL_SERVERS][32];
    char mach[TOTAL_SERVERS][32];
    char addr[TOTAL_SERVERS][ADDRESS_LEN];
    int sock[TOTAL_SERVERS];
} RMT_SERVERS;


extern RMT_SERVERS rmt_mpd;

int rmt_server_find(char *master_site);

void rmt_server_read(void);

void rmt_server_send(char *master_site);

#endif /* ifdef for rmt_servers_h */
