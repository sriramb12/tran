#ifndef rmt_globals_h
#define rmt_globals_h
#define TOTAL_SERVERS 5
char rmt_site[TOTAL_SERVERS][32];
char rmt_mach[TOTAL_SERVERS][32];
char rmt_addr[TOTAL_SERVERS][ADDRESS_LEN];
int rmt_sock[TOTAL_SERVERS];
#endif /* ifdef for rmt_globals_h */
