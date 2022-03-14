#ifndef fabint_defines_h
#define fabint_defines_h
#define FABINT_KEY "fab-integration"
#define FABINT_SOCK 6192
#define MAX_FABINT_SERVERS 4

/* Default Fabint machine names */
static char fabint_machine[MAX_SERVERS][MACHINE_LEN] =
{
    "maskdb1",
    "maskdb2",
    "maskdb3",
    "waccdb",
};

/* Holds IP addresses for default Fabint servers */
static char fabint_address[MAX_SERVERS][MACHINE_LEN] =
{
    "223.188.10.121",
    "223.13.249.31",
    "223.fabint_defines.h",
    "192.54.83.17",
};

#endif /* ifdef for fabint_defines_h */
