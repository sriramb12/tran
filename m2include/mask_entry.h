#ifndef mask_entry_h
#define mask_entry_h

/* Holds fields present in the mask.sets table */
typedef struct 
{
    char mask[8];
    char wl[16];
    char tech[32];
    char flow[32];
    char design[32];
    char creator[32];
    char owner[32];
    char site[16];
    char arc_status[4];
    long time;
} MASK_ENTRY;

void mask_entry_add(char *fab, char *tech, char *flow, char *design);

/* Delete entry from mask set table */
void mask_entry_dlt(void);

/* Modify existing mask entry */
void mask_entry_mod(void);

/* Modifies a mask line based on the request type */
void mask_line_mod(char *line);

/* Looks for a mask set in the 'mask.sets' file */
/* If mask set found, returns DBYES, else returns DBNO */
int mask_entry_find(char *mask);
#endif
