/****************************************************************************
 *
 * Function:	mask_entry_add
 *
 * Description:	Adds entry to global mask set table when a mask set is created.
 *		Deletes entry from mask set table when mask set deleted 
 *		Modifies mask set table when mask set transferred, archived, etc.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include "close_down.h"
#include "entry.h"
#include "lock_file.h"
#include "files.h"
#include "global_defines.h"
#include "m2req_types.h"
#include "mask_entry.h"
#include "mask_server_defines.h"
#include "m2server_globals.h"
#include "m2server_prototypes.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "m2server_paths.h"
#include "mti.h"
#include "rtn.h"
#include "m2msg.h"
#include "user_params.h"

void mask_entry_add(char *fab, char *tech, char *flow, char *design)
{
    char new_entry[260];

    /* Build the entry to add */
    sprintf(new_entry, "%s;%s;%s;%s;%s;%s;%s;%s;No;%lu",
    key, fab, tech, flow, design, user.email, user.email, ip.master_site, tr.upd_time);
    entry_add(GLOBAL_MASK_FILE, new_entry, key);
} /* end mask_entry_add */


void rmt_mask_entry_add(char *fab, char *tech, char *flow, char *design)
{
    char new_entry[260];

    /* Build the entry to add */
    sprintf(new_entry, "%s;%s;%s;%s;%s;%s;%s;%s;No;%lu",
    key, fab, tech, flow, design, user.email, user.email, mask_msg.key3, tr.upd_time);
    entry_add(GLOBAL_MASK_FILE, new_entry, key);
} /* end mask_entry_add */

/* Delete entry from mask set table */
void mask_entry_dlt(void)
{
    entry_delete(GLOBAL_MASK_FILE, key);
} /* End mask_entry_dlt */

/* Modify existing mask entry */
void mask_entry_mod(void)
{
    int index;
    char fnd = DBNO;
    int ret;
    char line[260];
    FILE *fi;
    FILE *fo;

    /* Open and lock mask set table */
    /* Open with stream pointer */
    if ((fi = fopen(GLOBAL_MASK_FILE, "r")) == NULL)
    close_down1m(rd_open_err, GLOBAL_MASK_FILE, RD_OPEN_ERR);
    /* Lock the file */
    index = lock_file_set(GLOBAL_MASK_FILE);
    /* Open the temporary file */
    if ((fo = fopen(NEW_MASK_FILE, "w")) == NULL)
    {
	/* Unlock the file */
	lock_file_clear(index);
	close_down1m(wr_open_err, NEW_MASK_FILE, WR_OPEN_ERR);
     }
    /* Go through table looking for entry to delete */
    while (line_get(fi, line) > 0)
    {
	if ((ret = strncmp(line, key, 4)) < 0) 
	fprintf(fo, "%s\n", line);
	else if (ret == 0) 
	{
	    fnd = DBYES;
	    /* Modify the entry and rewrite the line */
	    mask_line_mod(line);
	    fprintf(fo, "%s\n", line);
	    break;
	}
	/* If reached this point, entry not found */
	/* Thus do not need to bother writing out any more entries to new file */
	else break;
    } /* End while */
    /* If entry found, write out rest of entries */
    if (fnd == DBYES)
    {
	/* Scan past extra carriage return */
	while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BUFFER_SIZE, fi)) != 0)
	fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
    } /* End else for data still left in file */
    fclose(fi);
    fclose(fo);
    if (fnd == DBYES)
    file_rename(NEW_MASK_FILE, GLOBAL_MASK_FILE);
    /* Otherwise delete new file no longer needed */
    else unlink(NEW_MASK_FILE);
    /* Unlock the file */
    lock_file_clear(index);
} /* End mask_entry_mod */

/* Modifies a mask line based on the request type */
void mask_line_mod(char *line)
{
    MASK_ENTRY entry;

    /* Scan line into individual parameters */
   sscanf(line, "%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%lu",
entry.mask, entry.wl, entry.tech, entry.flow, entry.design,
    entry.creator, entry.owner, entry.site, entry.arc_status, &entry.time);
    /* Modify the entry based on the request type */
    switch(mask_msg.req_type)
    {
	case TRANSFER: strcpy(entry.site, mask_msg.par1); break;
	case MOD_OWNER: strcpy(entry.owner, mask_msg.par1); break;
	case UNARCHIVE: strcpy(entry.arc_status, "No"); break;
	/* Ignore all other transactions */
	default: break;
    }
    /* Write out the modified line */
    sprintf(line, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%lu",
entry.mask, entry.wl, entry.tech, entry.flow, entry.design,
    entry.creator, entry.owner, entry.site, entry.arc_status, entry.time);
} /* end mask_line_mod */

/* Search for a specified mask set in the 'mask.sets' file */
int mask_entry_find(char *mask)
{
    char line[260];
    FILE *fp;

    /* If mask.set file not present, return DBNO */
    if ((fp = fopen(GLOBAL_MASK_FILE, "r")) == NULL) return(DBNO);
    /* Read lines in the file searching for the mask set */
    while(line_get(fp, line) != EOF)
    {
	if (strncmp(line, mask, 4) == 0)
	{
	    fclose(fp);
	    return(DBYES);
	} /* End if for mask set found */
    } /* End while */
    /* If reached this point, mask set not found */
    fclose(fp);
    return(DBNO);
} /* end maskentry_find */
