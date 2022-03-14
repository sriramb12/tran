#include "email_handling.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define BOOLEAN int
#define FALSE 0
#define TRUE  1

/* files */
#define EMAIL_LIST  "all_users.txt"

#define  COREID_SIZE  12
#define  EMAIL_SIZE   256
#define  MAX_USERS    120     /* number of people that uses Maskdesigner or receives notification */

static  int  emails_users_counter  = 0;   /* counter  in struct  email_data */

struct EmailData
{
    char fsl_coreid[COREID_SIZE];
    char nxp_coreid[COREID_SIZE];
    char email[EMAIL_SIZE];
    char is_devel;
    char is_user;
    char is_manager;
};

static char                  recipients[2048];
static struct EmailData      users_email[MAX_USERS];



static void strtolwer(char *str)
{
    int i = 0;
    for(; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}


static char * ltrim(char *s)
{
    if (s != 0 )
    {
       while(isspace(*s)) s++;
    }
    return s;
}


static char * rtrim(char *s)
{
    if (s != 0 && strlen(s) > 0)
    {
        char* back = s + strlen(s);
        while(isspace(*--back));
        *(back+1) = '\0';
    }
    return s;
}


static char * trim(char *s)
{
    return rtrim(ltrim(s));
}


static const char *get_email_file_fullpath(const char *filename)
{
    static char full_path[1024];
    char *ptr = getenv("DMIHOME");
    const char *ret = 0;
    if (ptr != NULL)
    {
        strcpy(full_path, ptr);
        strcat(full_path, "/email/");
        strcat(full_path,filename);
    }
    else
    {
#if 1
        /* allow to use the file outside MDS package */
        strcpy(full_path, filename);
#else
        fprintf(stderr, "DMIHOME not defined file %s:%d", __FILE__, __LINE__);
        exit(2);
#endif
    }
    ret = full_path;
    return ret;
}



static void read_email_list()
{
    const char *full_filename = get_email_file_fullpath(EMAIL_LIST);
    FILE *fd = fopen(full_filename, "r");
    if (fd != NULL)
    {
        char buffer[512];
        char role[128];
        char * person_email_data = 0;
        while (!feof(fd))
        {
            if (fgets(buffer, sizeof(buffer) -1, fd) && (person_email_data=trim(buffer)) && *person_email_data != '#')
            {
                if (sscanf(buffer, "%s %s %s %s",
                                users_email[emails_users_counter].fsl_coreid,
                                users_email[emails_users_counter].nxp_coreid,
                                role,
                                users_email[emails_users_counter].email) == 4)
                {
                    strtolwer(users_email[emails_users_counter].fsl_coreid);
                    strtolwer(users_email[emails_users_counter].nxp_coreid);
                    strtolwer(role);
                    users_email[emails_users_counter].is_devel = strstr(role, "devel") == NULL ? FALSE : TRUE;
                    users_email[emails_users_counter].is_user  = strstr(role, "user") == NULL ? FALSE : TRUE;
                    users_email[emails_users_counter].is_manager = strstr(role, "manager") == NULL ? FALSE : TRUE;
                    if (++emails_users_counter >= MAX_USERS)
                    {
                        fprintf(stderr, "Maximium number of emails (%d) reached in %s:%d\n",
                                MAX_USERS, __FILE__, __LINE__);
                        exit(1);
                    }
                }
            }
        } // end not end of file
        fclose(fd);
    }
    else
    {
        fprintf(stderr, "could not open the file [%s]' in %s:%d",
               EMAIL_LIST, __FILE__, __LINE__);
        exit(2);
    }
}


static  void load_users_email()
{
    if (emails_users_counter == 0)
    {
        read_email_list();
    }
}


static void append_recipient_email(const char *email)
{
    if (strstr(recipients, email) == NULL)
    {
        if (recipients[0])
        {
            strcat(recipients, ",");
        }
        strcat(recipients, email);
        if (strlen(recipients) >= sizeof(recipients))
        {
            fprintf(stderr, "buffer overflow in %s:%d",
                    __FILE__, __LINE__);
            exit(2);
        }
    }
}

static void add_recipient(int index)
{
    append_recipient_email(users_email[index].email);
}



static const char * check_next_recipient(const char *rec)
{
    char single[256];
    char *s = single;

    if (rec != 0)
    {
        while (*rec != 0 && (isspace(*rec) || *rec == ',' || *rec == ';'))
        {
            rec++;
        }

        while (*rec != 0 && !isspace(*rec) && *rec != ',' && *rec != ';')
        {
            *s++ = *rec++;
        }
        *s = 0;

        if (single[0] == 0)
        {
            return NULL;
        }

        /* first case, sending to @freescale.com, try to find coreid */
        if ( (s = strstr(single, "@freescale.com")))
        {
            *s = 0;  // lets search by core id
        }
        if (strstr(single, "@nxp.com"))
        {
             append_recipient_email(single);
        }
        else if ( (s = strchr(single, '@')) == 0)
        {
            int counter = 0;
            int found = 0;
            /* core ID, search coreID in the table of users */
            strtolwer(single);
            for (; counter < emails_users_counter; ++counter)
            {
                if (strcmp(single, users_email[counter].fsl_coreid) == 0
                        || strcmp(single, users_email[counter].nxp_coreid) == 0)
                {
                    add_recipient(counter);
                    found = 1;
                    break;
                }
            }
            if (found == 0)
            {
                strcat(single, "@nxp.com");
                append_recipient_email(single);
            }
        }
        else   /* any other normal email */
        {
            append_recipient_email(single);
        }
    }
    return rec;
}

static void append_all_developers()
{
    int counter = 0;
    for (; counter < emails_users_counter; ++counter)
    {
        if (users_email[counter].is_devel)
        {
            append_recipient_email(users_email[counter].email);
        }
    }
}

//===========================  Public functions ===========================================================

const char * email_get_common_From()
{
    int counter = 0;
    const char * from  = NULL;
    load_users_email();
    for (; counter < emails_users_counter; ++counter)
    {
        if (users_email[counter].is_devel)
        {
            from = users_email[counter].email;
            break;
        }
    }
    return from;
}


const char * email_get_recipients_MaskPrep()
{
    int counter = 0;
    recipients[0] = 0;
    load_users_email();
    for (; counter < emails_users_counter; ++counter)
    {
        if (users_email[counter].is_user)
        {
            add_recipient(counter);
        }
    }
    return recipients;
}


const char * email_get_recipients_Managers()
{
    int counter = 0;
    recipients[0] = 0;
    load_users_email();
    for (; counter < emails_users_counter; ++counter)
    {
        if (users_email[counter].is_manager)
        {
            add_recipient(counter);
        }
    }
    return recipients;
}


const char * email_get_recipients_Developers()
{
     int counter = 0;
     recipients[0] = 0;
     load_users_email();
     for (; counter < emails_users_counter; ++counter)
     {
         if (users_email[counter].is_devel)
         {
             add_recipient(counter);
         }
     }
     return recipients;
}


const char * email_get_recipients_All()
{
     int counter = 0;
     recipients[0] = 0;
     load_users_email();
     for (; counter < emails_users_counter; ++counter)
     {
         add_recipient(counter);
     }
     return recipients;
}


const char * email_check_recipients(const char * cur_recipients)
{
    load_users_email();
    /* case 1 cur_recipients is a result of any of the email_get functions*/
    if (recipients == cur_recipients)
    {
        return cur_recipients;
    }

    /* case 2 cur_recipients is null or empty */
    if (cur_recipients == 0 || cur_recipients[0] == 0)
    {
        (void) email_get_recipients_MaskPrep();
        // it sets recipients buffer
    }
    else
    {
        const char *rec = cur_recipients;
        recipients[0] = 0;
        /* check_next_recipient will fill recipients buffer */
        while ( (rec  = check_next_recipient(rec)) )
        {

        }
    }
    append_all_developers();
    return recipients;
}


int email_isThereAnyInternalRecipient(const char *recipients)
{
    int ret = FALSE;
    if (recipients && strstr(recipients, "@nxp.com"))
    {
        ret = TRUE;
    }
    return ret;
}

/* compile using
     gcc -g -o mz_email -DEMAIL_STANDALONE_TEST \
            email_handling.c \
            global_debug.c \
            ../../db_libs/shared_client_lib/source/email_send.c \
         -I../../../includes/db_includes/global_include/ \
         -I../../../includes/db_includes/trans_include/  \
         -I../../../includes/c_user/
 */
#if EMAIL_STANDALONE_TEST
#include "global_defines.h"
DB_SOFTWARE so;
extern void  email_files_send(char*, char*, char*, char*, char*);
extern void  email_file_send(char*, char*, char*, char*);
extern short email_msg_file_send(char*, char*, char*, char*, char*);
extern void  email_send(char*, char*, char*, char*);
int main(int argc, char *argv[])
{
    if (argc == 2)
    {
         setenv("DMIHOME", argv[1], 1);
    }
    strcpy (so.program, "test");

//    printf("email_get_common_From()=%s\n",           email_get_common_From());
//    printf("email_get_recipients_MaskPrep()=%s\n",   email_get_recipients_MaskPrep());
//    printf("email_get_recipients_Managers()=%s\n",   email_get_recipients_Managers());
//    printf("email_get_recipients_Developers()=%s\n", email_get_recipients_Developers());
//    printf("email_get_recipients_All()=%s\n",        email_get_recipients_All());

//    email_send("b35201", "b35201", "using b35201", "testonly b35201");
//    email_send("B35201", "B35201", "using B35201", "testonly B35201");
//      email_send("nxa09614", "nxa09614", "using nxa09614", "testonly nxa09614");
//    email_send("NXA13339", "NXA13339", "using NXA13339", "testonly NXA13339");
//    email_send("alex.palombo.freescale.com",
//               "alex.palombo.freescale.com",
//               "using alex.palombo.freescale.com",
//               "testonly alex.palombo.freescale.com");
//    email_send("carlos.mazieri@nxp.com",
//               "carlos.mazieri@nxp.com",
//               "using carlos.mazieri@nxp.com",
//               "testonly carlos.mazieri@nxp.com");
//    email_send("carlos.mazieri@gmail.com",
//                  "carlos.mazieri@gmail.com, RAP003, carlos.mazieri@nxp.com",
//                  "using carlos.mazieri@gmail.com, RAP003, carlos.mazieri@nxp.com",
//                  "testonly carlos.mazieri@gmail.com, RAP003, carlos.mazieri@nxp.com");

//        email_send("carlos.mazieri@gmail.com",
//                      "carlos.mazieri@gmail.com",
//                      "using carlos.mazieri@gmail.com",
//                      "testonly carlos.mazieri@gmail.com");

//     email_send("rap003@freescale.com",
//                "rap003@freescale.com",
//                "using rap003@freescale.com",
//                "testonly rap003@freescale.com");

//     email_send("mdsEvents@freescale.com", "b35201",
//                "load_print_panel()", "Test Only.");

    email_send("", "mdsEvents@freescale.com",
               "load_print_panel()", "Test Only.");
    return 0;
}

#endif
