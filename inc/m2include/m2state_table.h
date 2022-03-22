#ifndef m2state_table_h
#define m2state_table_h

#include "m2server_errs.h"

/* Data states which prevent server from doing requested manipulation */
#define RMT2UPDATE 0
#define DBREAD 1
#define CHECKED_OUT 2
#define DBUPDATE 3
#define NO_ACTION 4
#define LOCKED4ARCHIVE 5
#define ARCHIVED 6
#define CHECK_IN -24	/* Transient state file in while being checked in */

#define ROW_OFFSET 14	/* Offset for row index in state_table */
#define MIN_ROW_INDEX 0
#define MIN_COL_INDEX 0
#define MAX_ROW_INDEX 56

#define MAX_COL_INDEX 6
/* The following state table defines resulting action based on state of 
 * requested file and requested action.
 * Rows specify actions and columns represent file states.
 * File states are nonexistent, read, checked out, update, no action, & locked.
 * Possible actions are defined in db_errs.h.
 */

/* Defines for overwrite flag and user statuses */
#define ALL 0		/* Overwrite on and user ID-s equal */
#define NEITHER 1		/* Overwrite off and user ID's not equal */
#define OVERWRITE_ONLY 2 /* Overwrite flag on and user ID's not equal */
#define USER_ONLY 3	/* Overwrite flag off and user ID's equal */


static short state_table[56][7] = {
/* Non-existent, Read, check-out, update, no action, remote-update   locked for  archive, Archived */
/* Case 1:  Overwrite flag on and 2 user ID's equal */
  { RMT_OVR, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  ARC1ERR, ARC1ERR } , /* Overwrite */ 
  { RMT_OVR, NCO1ERR, SUCCESS, UPD1CIN, NCO1ERR,  ARC1ERR, ARC1ERR } , /* Check-in */ 
  { RMT_OVR, BAD1UPD, SUCCESS, UPD1UPD, BAD1UPD,  ARC1ERR, ARC1ERR } , /* Update */ 
  { RMT_OVR, BAD1UPD, SUCCESS, UPD1UPD, BAD1UPD,  ARC1ERR, ARC1ERR } , /* Save */ 
  { RMT_OVR, RD_WAIT, SUCCESS, OVR_WRN, SUCCESS,  SUCCESS, SUCCESS } , /* Delete */ 
  { RMT_OVR, SUCCESS, SUCCESS, UPD1OUT, SUCCESS,  SUCCESS, ARC1ERR } , /* Check-out */ 
  { RMT_OVR, SUCCESS, CHA1CHO, UPD1OUT, SUCCESS,  ARC1ERR, ARC1ERR } , /* Approve */ 
  { RMT_OVR, NCB1ERR, SUCCESS, NGI_WRN, NCB1ERR,  NCB1ERR, NCB1ERR } , /* Check-back */ 
  { RMT_OVR, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  SUCCESS, SUCCESS } , /* Transfer data */ 
  { RMT_OVR, SUCCESS, BAM1ERR, NGI_WRN, SUCCESS,  SUCCESS, SUCCESS } , /* Read */ 
  { RMT_OVR, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  SUCCESS, SUCCESS } , /* List-versions  */ 
  { RMT_OVR, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  ARC1ERR, ARC1ERR } , /* Unapprove-data  */ 
  { UPD4ERR,     SUCCESS, OUT4ERR, UPD4ERR, SUCCESS, SUCCESS, ALR1ARC } , /* Lock for archive */ 
  { NOT1ARC,     NOT1ARC, NOT1ARC, NOT1ARC, NOT1ARC, SUCCESS, SUCCESS } , /* Unarchive */ 
/* Case 2 Overwrite flag off and 2 user ID's not equal */ 
  { RMT2UPD, RD_WAIT, SUCCESS, SUCCESS, SUCCESS,  ARC1ERR, ARC1ERR } , /* Overwrite */ 
  { RMT2UPD, NCO1ERR, OUT4ERR, UPD4ERR, SUCCESS,  ARC1ERR, ARC1ERR } , /* Check-in */ 
  { RMT2UPD, BAD1UPD, OUT4ERR, UPD4ERR, BAD1UPD,  ARC1ERR, ARC1ERR } , /* Update */ 
  { RMT2UPD, BAD1UPD, OUT4ERR, UPD4ERR, BAD1UPD,  ARC1ERR, ARC1ERR } , /* Save */ 
  { RMT2UPD, DLT4DATA, OUT4ERR, UPD4ERR, DLT2OVR,  SUCCESS, SUCCESS } , /* Delete */ 
  { RMT2UPD, SUCCESS, OUT4ERR, UPD4ERR, SUCCESS,  SUCCESS, ARC1ERR } , /* Check-out */ 
  { RMT2UPD, SUCCESS, OUT4ERR, UPD1OUT, SUCCESS,  SUCCESS, ARC1ERR } , /* Approve */ 
  { RMT2UPD, NCB1ERR, OUT4ERR, UPD4ERR, NCB1ERR,  NCB1ERR, NCB1ERR } , /* Check-back */ 
  { RMT2UPD, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  SUCCESS, SUCCESS } , /* Transfer data */ 
  { RMT2UPD, SUCCESS, CHO4WRN, UP_WAIT, SUCCESS,  SUCCESS, SUCCESS } , /* Read */ 
  { RMT2UPD, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  SUCCESS, SUCCESS } , /* List-versions */ 
  { RMT2UPD, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  ARC1ERR, ARC1ERR } , /* Unapprove */ 
  { UPD4ERR, SUCCESS, OUT4ERR, SUCCESS, SUCCESS, SUCCESS, ALR1ARC } , /* Lock for archive */ 
  { NOT1ARC, NOT1ARC, NOT1ARC, NOT1ARC, NOT1ARC, SUCCESS, SUCCESS } , /* Unarchive */ 
 /* Case 3:  Overwrite flag on and 2 user ID's not equal */ 
  { RMT_OVR, RD_WAIT, SUCCESS, SUCCESS, SUCCESS,  ARC1ERR, ARC1ERR } , /* Overwrite */ 
  { RMT_OVR, NCO1ERR, OUT4ERR, UPD4ERR, NCO1ERR,  ARC1ERR, ARC1ERR } , /* Check-in */ 
  { RMT_OVR, BAD1UPD, OUT4ERR, UPD4ERR, BAD1UPD,  ARC1ERR, ARC1ERR } , /* Update */ 
  { RMT_OVR, BAD1UPD, OUT4ERR, UPD4ERR, BAD1UPD,  ARC1ERR, ARC1ERR } , /* Save */ 
  { RMT_OVR, RD_WAIT, OUT4ERR, UPD4ERR, DLT4DATA,  SUCCESS, SUCCESS } , /* Delete */ 
  { RMT_OVR, SUCCESS, OUT4ERR, UPD4ERR, SUCCESS,  SUCCESS, ARC1ERR } , /* Check-out */ 
  { RMT_OVR, SUCCESS, OUT4ERR, UPD4WRN, SUCCESS,  ARC1ERR, ARC1ERR } , /* Approve */ 
  { RMT_OVR, NCB1ERR, OUT4ERR, UPD4ERR, NCB1ERR,  NCB1ERR, NCB1ERR } , /* Check-back */ 
  { RMT_OVR, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  SUCCESS, SUCCESS } , /* Transfer data */ 
  { RMT_OVR, SUCCESS, CHO4WRN, UP_WAIT, SUCCESS,  SUCCESS, SUCCESS } , /* Read */ 
  { RMT_OVR, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  SUCCESS, SUCCESS } , /* List-versions */ 
  { RMT_OVR, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  ARC1ERR, ARC1ERR } , /* Unapprove */ 
  { UPD4ERR, SUCCESS, OUT4ERR, UPD4ERR, SUCCESS, SUCCESS, ALR1ARC } , /* Lock for archive */ 
  { UPD4ERR, NOT1ARC, OUT4ERR, UPD4ERR, NOT1ARC, SUCCESS, SUCCESS } , /* Unarchive */ 
 /* Case 4: Overwrite flag off and 2 user ID's equal */ 
  { RMT2UPD, RD_WAIT, SUCCESS, SUCCESS, SUCCESS,  ARC1ERR, ARC1ERR } , /* Overwrite */ 
  { RMT2UPD, NCO1ERR, SUCCESS, CIN2UPD, NCO1ERR,  ARC1ERR, ARC1ERR } , /* Check-in */ 
  { RMT2UPD, RD_WAIT, SUCCESS, OVR2UPD, BAD1UPD,  ARC1ERR, ARC1ERR } , /* Update */ 
  { RMT2UPD, RD_WAIT, SUCCESS, OVR2UPD, BAD1UPD,  ARC1ERR, ARC1ERR } , /* Save */ 
  { RMT2UPD, DLT2OVR, DLT1OVR, DEL2UPD, DLT2OVR,  SUCCESS, SUCCESS } , /* Delete */ 
  { RMT2UPD, SUCCESS, OVR2CHO, CHO1UPD, SUCCESS,  SUCCESS, ARC1ERR } , /* Check-out */ 
  { RMT2UPD, SUCCESS, CHA1CHO, NGI_WRN, SUCCESS,  ARC1ERR, ARC1ERR } , /* Approve */ 
  { RMT2UPD, NCB1ERR, CAN1OVR, CAN2UPD, NCB1ERR,  NCB1ERR, NCB1ERR } , /* Check-back */ 
  { RMT2UPD, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  SUCCESS, SUCCESS } , /* Transfer data */ 
  { RMT2UPD, SUCCESS, RDO2CHO, RD2UPD , SUCCESS,  SUCCESS, SUCCESS } , /* Read */ 
  { RMT2UPD, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  SUCCESS, SUCCESS } , /* List-versions */ 
  { RMT2UPD, SUCCESS, SUCCESS, SUCCESS, SUCCESS,  ARC1ERR, ARC1ERR } , /* Unapprove */ 
  { UPD4ERR, SUCCESS, OUT4ERR, UPD4ERR, SUCCESS, SUCCESS, ALR1ARC } , /* Lock for archive */ 
  { NOT1ARC, NOT1ARC, NOT1ARC, NOT1ARC, NOT1ARC, SUCCESS, SUCCESS } , /* Unarchive */ 
};

#endif /* Define for m2state_table_h */
