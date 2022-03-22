#ifndef opc_ps_h
#define opc_ps_h

/* Maximum length of record with output from PS command */
#define PS_REC_LEN 260
#define OPC_PS_INPUT "%d %c %s %d %d %d %d %d %s %d %s %s %s %s %s"

#define OPC_PS_OUTPUT "%d %c %s %d %d %d %d %d %s %d %s %s %s %s %s\n"

typedef struct
{
    int flags;
    char state;
    char uid[16];
    int pid;
    int ppid;
    int util_class;
    int priority;
    int nice;
    char addr[12];
    int swap_size;
    char cchan[12];
    char start_time[12];
    char tty[16];
    char exe_time[16];
    char command[16];
} OPC_PS;

OPC_FILE *opc_ps_open(void);

/* Read one entry from ps  file */
/* Pass in OPC ps structure */
/* Function fills in structure and returns 0 on success */
/* Returns -1 if no more entries present */
int opc_ps_read(OPC_FILE *fi, char *entry);

/* Close PS file */
int opc_ps_close(OPC_FILE *fp);

/* Display output from PS command */
/* Used by the ASCII client */
int opc_ps_view(char *dir);

#endif /* Define for opc_ps_h */
