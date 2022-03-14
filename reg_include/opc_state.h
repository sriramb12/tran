#ifndef opc_state_h
#define opc_state_h

#define OPC_STATE_INPUT "%d %s %f %d"
#define OPC_STATE_OUTPUT "%d %s %2.3f %d\n"

/* Holds OPC states */
typedef enum
{
    NO_ACTION, LOCKED, NOT_FOUND
} OPC_STATES;

typedef struct
{
    int state;
    int fd;
    char email[32];
    float vnum;
    long time_stamp;
    char buffer[256];
    char file[FILE_SIZE];
} OPC_STATE_PARAMS;

extern OPC_STATE_PARAMS st;

#endif
