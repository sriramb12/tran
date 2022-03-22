#ifndef intercom_company_h
#define intercom_company_h

#include "global_defines.h"
#include "trans_defines.h"

#ifdef INTERCOM
#ifdef MULTIPLE_SECNETS

typedef struct
{
    char site[SITE_LEN];
    char machine[64];
    char address[16];
    int socket;
} SECNET;

#ifdef NXP 
#ifdef SSL_ENCRYPTION
static SECNET sn[5] = 
{
    { "External", "dropbox8", "192.88.157.190", 6924, },
    { "Send2foundry", "app07-tx30", "10.81.199.12", 6922, },
    {  "FTPgate", "cde-az34-019va", "10.65.24.19", 6922, },
    {  "ITAR", "medusa", "10.80.21.106", 6922, },
    {  "Local", "angel", "172.16.7.179", 6924, },
};
#else
static SECNET sn[5] = 
{
    { "External", "dropbox8", "192.88.157.190", 6923, },
    { "Phoenix", "dropbox7", "192.88.160.33", 6923, },
    {  "FTPgate", "cde-az34-019va", "10.65.24.19", 6928, },
};
#endif

static int s_cnt = 5;
#endif
#endif /* For Multiple secnets */
#endif /* For InterCom */

#endif /* Define for intercom_company_h */
