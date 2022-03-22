#ifndef registry_prototypes_h
#define registry_prototypes_h

#include "reg_globals.h"
int add_site_entry(
    char *new_site,
    char *new_email1,
    char *new_email2);
int dlt_site_entry(char *site);

int new_reg(
    char *site,	/* Needed only to add entry to site table */
    char *user_email1,	/* User's primary Email address */
    char *user_email2,	/* User's secondary Email address */
    char *user_name1,
    char *user_name2,
    char *user_name3,
    char *user_pwd,
    REG_PARAMS *reg);	/* Holds user's name and Email address if found */
void
resend_reg();

void rmt_force_reg(
    char *key1,
    char *key2,
    char *key3,
    char *dpt,
    char *par1, 
    char *par2,
    char *par3,
    char *par4,
    char *par5);

void rmt_man_reg(
    char req_type,	/* Needed for different request types in Mask Prep */
    char *site,	/* Site of user being registered */
    char *email1,
    char *email2);

void reg_name_build(char *file_name, char *site);

#endif /* define for registry_prototypes_h */
