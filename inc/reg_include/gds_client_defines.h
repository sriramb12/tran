#ifndef gds_client_defines_h
#define gds_client_defines_h
#include "global_defines.h"

#define GDS_FORMAT_MSG "Incorrect format entered\n"

#define GDS_HELP_MSG \
"To transfer a GDS file, type:\n\
  dtms_dessync -D  <design_name>        design name                              [required] \n\
  -I  <maskID>             mask ID                                  [required] \n\
  -T  <technology_name>    technology name                          [required] \n\
  -b  <badge>              badge number                             [optional] \n\
  -c  <file>               comment file                             [optional] \n\
  -g  <file>               gdb file                                 [required] \n\
  -s  <state>              gdb file state (BE,FE,FIN)               [required]\n"


typedef enum
{
    MASK,
    TECHNOLOGY,
    DESIGN,
    GDS_FILE,
    STATE,
    BADGE,
} JOB_OPTION;

static char job_type_string[6][16]  =
{
     "Mask",
    "Technology",
    "Design",
    "Gds file",
    "State",
    "Badge",
};

#define bad_param \
"Error -199:    Too few command line parameters.\n\
               %s not specified."

#endif
