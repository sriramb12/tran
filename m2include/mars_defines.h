#ifndef mars_defines_h
#define mars_defines_h

#include "global_defines.h"
#define MARS_BANNER \
"\n       Welcome to version 2.0.3 of MARS\n\
               (Mask Retrieval System)\n\
      Free Scale Semiconductor Confidential Proprietary software\n\
        Not for distribution outside Free Scale\n\n"

#define MARS_HELP \
"To retrieve one or more mask set names, type:\n\n\
    mars -m <from_email> <to-email> <orig. mask> <fab> <tech> <MBG> <leader name> <leader ID><dante num> <product> <chipset> <duedate> <eccn> <desc> <remarks> \n\n\
To retrieve information for specified mask set, type:\n\n\
    mars -M <mask> <user ID>\n\n"

typedef struct
{
    char request_date[12];
    char mask[16];
    char orig_mask[16];
    char user_id[16];
     char name1[32];
    char name2[16];
    char dpt[9];
     char old_mask[16];
    int layers;
    char maskshop[20];
    char size[20];
    char date[20];
    char remarks[80];
} OLD_MARS_INFO;

/* New MARS info */
typedef struct 
{
    char request_date[9];
    char mask[16];
    char user_id[10];
    char name[32];
    char orig_mask[16];
    char fab[32];
    char tech[64];
    char mbg[64];
    char leader[32];
    char leader_id[16];
    char product_num[32];
    char product_name[64];
    char chipset[64];
    char duedate[64];
    char eccn[32];
    char desc[BUFFER_SIZE];
    char remarks[BUFFER_SIZE];
} MARS_INFO;
#endif /* Define for mars_defines_h */
