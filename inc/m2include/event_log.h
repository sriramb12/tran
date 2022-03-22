#ifndef event_log_h
#define event_log_h

/******************************************************************************/
/* Free Scale Semiconductor Internal Use Only                                                 */
/* (c) Copyright Free Scale Semiconductor Corporation 2002.                                   */
/*                                                                            */
/* Modification Log:                                                          */
/*      Renee Carter */
/*      Initial Release                                                       */
/*                                                                            */
/******************************************************************************/

/*

The Event Log utility is used  to  log  Mask  Designer  tool
events  in  the  intermediate  log  file  where  they can be
retrieved by  Mettracks  utilities  and  put  in  the  MySQL
database also referred to as the data warehouse.  Event Log
connects to a central server and sends the data to be logged
to  the  server.   The  Event Log utility spawns off a child
process to perform the logging of the data.  this is done to
guarantee  any  delays  connecting  to  the  server  do  not
interfere with the tool operation.  If Event Log  is  unable
to connect to the server, it simply exits.

Event Log expects four arguments.  These include:

Tool name:  Each tool owner is responsible for  defining  an
ASCII string to be used to identify his tool.

-    action:  See actions defined below.

-    Mask set name or NA if the mask set name is unknown.

-    Data:  Each tool owner is responsible for defining  the
     data  that will be passed to Event Log.  Developers can

*/

/* Used when no value for a function argument is present or is unknown */
#define NA "N/A"

/* Actions that can be passed to Event Log */
#define START "Start"
#define DATA "Data"
#define END "End"
#define CANCEL "Cancel"
#define ABORT "Abort"
#define COMPLETE "Complete"
#define SAVE_EXIT "Save-exit"

void event_log(char *mask, char *tool, char *action, char *data);

/* Log MDS start in event log */
/* A child process is spawned off to do the work */
/* At this point, the Mask Designer database client knows the user's user ID */
/* version holds the current Mask Designer version number */
void event_log_mds_start(char *version);

void event_log_(char *mask1, char *tool1, char *action1, char *data1, int len_mask, int len_tool, int len_action, int len_data);

int metrics_request_(int req_type);

#endif
