#ifndef maskshop_h
#define maskshop_h

#define KEYWORD 0
#define USER_ID 1
#define USER_NAME 2
#define PLATFORM 3
#define ADDRESS 4
#define CLIENT_VERSION 5
#define FROM_COMPANY 6
#define TO_COMPANY 7
#define REQUESTTYPE 8
#define FILENAME 9
#define MASKSET 10
#define ORIGINALSIZE 11
#define COMPRESSEDSIZE 12
#define STARTTIME 13
#define COMPLETETIME 14
#define TRANSFERTIME 15
#define ORDER_STATUS 16
#define DATE 17

#define REPORT_ARRAY_SIZE 18
#define STRING 's'
#define SIZE 'i'
#define TIME 't'
#define ELAPSED_TIME 'e'

typedef struct
{
    char user_id[32];
    char user_name[64];
    char platform[16];
     char addr[20];
    char client_version[16];
    char from_company[32];
    char to_company[32];
    char req_type[16];
    char file_name[64];
    char date[12];
    long long original_size;
    long long compressed_size;
    long start_time;
    long complete_time;
} MASKSHOP_REPORT;

#endif
