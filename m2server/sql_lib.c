#include <mysql.h>
#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_sql_lib.h"
#include "mti.h"
#include "m2globals.h"
#include "m2server_errs.h"
#include "m2server_globals.h"
#include "rtn.h"
#include "server_lib.h"
#include "server_params.h"
#include "strcpy.h"
#include "user_params.h"
#include "global_debug.h"


/* Convert date to SQL format */
void sql_date_convert(char *sql_date, char *line)
{
    int day;
    char tmp_day[4];
    char hour[12];
    char month[8];
    char year[8];

    sscanf(line, "%s %d %s %s",
    month, &day, hour, year);
    /* Copy in the year */
    strncpy(sql_date, year, 4);
    str_month_convert(sql_date+5, month);
    sql_date[4] = sql_date[7] = '-';
    /* Change day to character format */
    sprintf(tmp_day, "%d", day);
    if (strlen(tmp_day) == 1)
    {
	sql_date[8] = '0';
	sql_date[9] = tmp_day[0];
    } /* End if */
    else strcpy(sql_date+8, tmp_day);
    sql_date[10] = ' ';
    /* Now put in the hour */
    strcpy(sql_date+11, hour);
} /* End sql_date_convert */

/* Verify this is a valid mask name */
/* Returns DBYES if is a mask, else returns DBNO */
int is_mask(char *mask)
{

    if (strlen(mask) != 4) return(DBNO);
    if ((mask[0] < 'a') || (mask[0] > 'z')) return(DBNO);
    else if ((mask[1] < '0') || (mask[1] > '9')) return(DBNO);
    else if ((mask[2] < '0') || (mask[2] > '9')) return(DBNO);
    else if ((mask[3] < 'a') || (mask[3] > 'z')) return(DBNO);
    else return(DBYES);
} /* end is_mask */

void new_data_log(char *log_file, char *msg)
{
    printf("%s\n", msg);
    data_log(log_file, msg);
}


/* Add mask entry to common_mars table */
void mars_insert(char *mask, char *name,      char *from_email, char *orig_mask,
char *fab, char *tech, char *mbg, char *project_leader, char *project_uid,
char *dante_num, char *project_name, char *chipset, char *due_date, char *eccn, char *description, char *remarks)
{
    char query[8192];

    /* No longer modifying the format of the due date */
    sprintf(query, " insert into common.common_mars \
 (mars_request_date, mars_maskset, mars_requester_userid, mars_requester_name, mars_redesign,  \
 mars_fab, mars_technology, mars_mbg, mars_project_leader_name, mars_project_leader_userid,  \
 mars_dante_number, mars_project_name, mars_platform_chipset, mars_projected_tapeout_date, mars_eccn, mars_comment, mars_additional_comment, mars_last_modified) \
    values (\
    %s, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', now())",
    "NOW()", mask,      from_email, name, orig_mask,
fab, tech, mbg, project_leader, project_uid,
dante_num, project_name, chipset, due_date, eccn, description, remarks);
    /* Get rid of HTML characters */
    html_filter(query);
    /* Run command to put record in the SQL database */
    data_log("query", query);
    insert_data(DTMS_DB, "common", query, DBNO);
} /* End mars_insert */


void tmp_die_area_insert(
    char *     die_maskset,
    double die_x_scribe,
    double die_y_scribe,
    double die_x_no_scribe,
    double die_y_no_scribe,
    double vertical_scribe,
    double horizontal_scribe,
    char *fab)
{
    char query[1024];

    sprintf(query, " replace into common.die_area \
values ('%s',  %10.3f, %10.3f, %10.3f, %10.3f, %10.3f, %10.3f, '%s', '%s', now())",
die_maskset, die_x_scribe, die_y_scribe, die_x_no_scribe, die_y_no_scribe, vertical_scribe, horizontal_scribe, fab, "rxfh80");
    /* Run command to put record in the SQL database */
    insert_data(DTMS_DB, "common", query, DBNO);
} /* End die_area_insert */

/* Update reticle_die_table */
int reticle_die_update(char *mask, char *rows, char *columns, char *num_die, int program_type)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[1024];
    char pizza_die[8];
    int num_rows;
    int die_per_column = atoi(columns);
    int die_per_row = atoi(rows);
    int die_per_shot = atoi(num_die);

    /* If the number of rows is = to -1, this is a pizza die */
    if (die_per_row == -1)
    {
        die_per_row = 1;
        strcpy(pizza_die, "yes");
    } /* End if */
    else
    {
        strcpy(pizza_die, "no");
    }

    DBG("mask=%s num_die=%s die_per_shot=%d die_per_row=%d pizza_die=%s", mask, num_die, die_per_shot, die_per_row, pizza_die);

    /* See if the record is already present */
    strcpy3(query, "select * from maskops.reticle_die where device = '", mask, "'");
    /* Run the query */
    result = result_get(DTMS_DB, "maskops", query, NULL);
    /* count the number of rows */
    num_rows = mysql_num_rows(result);
    /* If record already present and die_per_shot is 1 and this is IMPORT_COORDS, no update needed */
    if ((num_rows >= 1) && (die_per_shot == 1) && (program_type == IMPORT_COORDS))
    {
        put_short(SUCCESS);
        return_success("Update to reticle die table not needed", DBNO);
        exit(0);
    }
    /* If one row present, update existing entry */
    if (num_rows >= 1)
    {
        /* Get the row */
        row = mysql_fetch_row(result);
        sprintf(query, "replace into maskops.reticle_die (device, die_per_row, die_per_column, die_per_shot, create_date, modification_date, pizza_die) values('%s', %d, %d, %d, '%s', now(), '%s')",
                mask, die_per_row, die_per_column, die_per_shot, row[4], pizza_die);
    }
    else sprintf(query, "insert into maskops.reticle_die (device, die_per_row, die_per_column, die_per_shot, create_date, modification_date, pizza_die) values('%s', %d, %d, %d, now(), now(), '%s')",
                 mask, die_per_row, die_per_column, die_per_shot, pizza_die);
    insert_data(DTMS_DB, "maskops", query, DBNO);
    /* May 31 2019, Carlos Mazieri
     * Whey there is a code here trying to insert into metrics ?
     */
       // insert_data(DTMS_DB, "metrics", query, DBNO);
} /* end reticle_die_update */

    void mars_insert_old_data(char *request_date, char *mask, char *name,      char *from_email, char *orig_mask,
char *fab, char *tech, char *mbg, char *project_leader, char *project_uid,
char *dante_num, char *project_name, char *chipset, char *due_date, char *eccn, char *description, char *remarks)
{
    char new_due_date[12];
    char new_request_date[12];
    char query[1024];

    /* Modify the format of the due date */
    if (strlen(due_date) == 10)
    {
	strncpy( new_due_date, due_date+6, 4);
	strncpy( new_due_date+5, due_date, 2);
	strncpy( new_due_date+8, due_date+3, 2);
    new_due_date[4] = new_due_date[7] = '/';
    }
    else strcpy(new_due_date, due_date);
    /* Set the request date to correct format */
    /* Put in first part of the year */
    strcpy(new_request_date, "20");
    /* Put in last 2 digits of year */
    strncpy(new_request_date+2, request_date+6, 2);
    /* Now copy in the month */
    strncpy(new_request_date+5, request_date, 2);
    /* Now copy in the day */
    strncpy(new_request_date+8, request_date+3, 2);
    /* Now put in the slashes */
    new_request_date[4] = new_request_date[7] = '/';
    /* Put NULL at end of string */
    new_request_date[10] = '\0';
    sprintf(query, " insert into common.common_mars \
 (mars_request_date, mars_maskset, mars_requester_userid, mars_requester_name, mars_redesign,  \
 mars_fab, mars_technology, mars_mbg, mars_project_leader_name, mars_project_leader_userid,  \
 mars_dante_number, mars_project_name, mars_platform_chipset, mars_projected_tapeout_date, mars_eccn, mars_comment, mars_additional_comment) \
    values (\
    '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
    new_request_date, mask, name,      from_email, orig_mask,
fab, tech, mbg, project_leader, project_uid,
dante_num, project_name, chipset, new_due_date, eccn, description, remarks);
    /* Run command to put record in the SQL database */
    insert_data(DTMS_DB, "common", query, DBNO);
} /* end mars_insert_old_data */


/* Put process steps into database */
/* Process steps are the same as layers */
int sql_insert_process_steps(char *user_id, int fab_num, int tech_num, char *tech_file, char *approve_status)
{
    char comment_field[80];
    char layer_field[32];
    FILE *fp;
    char line[2048];
    char query[1024];
    int sql_errno;
    int ret;
    MYSQL sql;

    /* First open the process step table */
    if ((fp = fopen(tech_file, "r")) == NULL)
    {
	printf("could not open %s\n", tech_file);
	return(-1);
    } /* end if */
    /* Scan past the first 2 lines */
    long_line_get(fp, line); long_line_get(fp, line);
    /* Initialize the library */
    /* Open a connection */
    sql_connect(&sql, DTMS_DB, "common");

    /* Read information from process step table */
    while (long_line_get(fp, line) != EOF)
    {
    layer_comment_find(line, layer_field, comment_field);
	sprintf(query, "insert into common.common_layers (common_userid, common_id_fab, common_id_technology, common_layer_name, common_datetime, common_comment, common_approve_status) \
    values('%s', %d, %d, '%s',  NOW(), '%s', '%s')",
    user_id, fab_num, tech_num, layer_field, comment_field, approve_status);
	ret = mysql_query(&sql, query);
	sql_errno = mysql_errno(&sql);
	if (sql_errno != 0)
	{
	    const char *err_ptr = mysql_error(&sql);
	    sprintf(rtn.msg, sql_err, sql_errno, query, err_ptr);
	new_data_log("sql_errs", rtn.msg);
	} /* End if */
    } /* end while */
    mysql_close(&sql);
    mysql_library_end();
    return(SUCCESS);
} /* End old_sql_insert_process_step */

/* Map the MDS technology to the common technology */
int sql_mds_tech_name_map(int tech_id_array[], char *tech_name)
{
    int cnt;
    int num_rows;
    int num_fields;
    MYSQL_RES *result;
    MYSQL_ROW row;
     char query[1024];

    /* Build the query */
    sprintf(query, "select common_id_technology from common.common_mds_technology_mapping where common_mds_technology = '%s'", tech_name);
    data_log("query", query);
    result = result_get(DTMS_DB, "common", query, NULL);
    /* count number of rows and fields */
    /* If there is a mapping, there should be one row */
    num_rows = mysql_num_rows(result);
    num_fields = mysql_num_fields(result);
    if ((num_rows <= 0) || (num_fields <= 0))
    {
	return(0);
    } /* end if for no match found */
    else
    {
	for (cnt = 0; cnt < num_rows; ++cnt)
	{
	row = mysql_fetch_row(result);
	tech_id_array[cnt] = atoi(row[0]);
	} /* end for */
	return(num_rows);
    } /* end else */
} /* end sql_mds_tech_name_map */

/* Find the technology number for a technology */
int sql_tech_num_find(char *technology, int first)
{
    int num_rows;
    int num_fields;
    MYSQL_RES *result;
    MYSQL_ROW row;
    char *ptr;
    char local_tech[48];
     char query[1024];

    /* copy technology into local variable so master copy not changed */
    strcpy(local_tech, technology);
    /* Strip off everything after the '_' */
    if ((ptr = strchr(local_tech, '_')) != NULL)
    {
	if (first == DBYES)
	{
	    ptr[0] = '\0';
	    /* Point to the next character after the underscore */
++ptr;
	    /* If no characters found, indicate */
	    if (ptr[0] == '\0') ptr = NULL;
	} /* end if */
	    /* Otherwise simply overwrite the _ character */
	    else strcpy(ptr, ptr+1);
    }
    else ptr = NULL;
    /* Build the query */
    strcpy3(query,"select common_id_technology from common.common_listTechnologiesAll_v where common_full_technology = '",
    local_tech, "'");
    data_log("query", query);
    result = result_get(DTMS_DB, "common", query, NULL);
    /* count number of rows and fields */
    /* There should be one row */
    num_rows = mysql_num_rows(result);
    num_fields = mysql_num_fields(result);
    if ((num_rows <= 0) || (num_fields <= 0))
    {
	if ((first == DBYES) && (ptr != NULL))
	/* Call function again */
	{
	    /* Take the underscore out of the name */
	strcat(local_tech, ptr);
	    return(sql_tech_num_find(local_tech, DBNO));
	} /* End if for calling function again */
	else return(0);
    } /* end if for no match found */
    else
    {
	row = mysql_fetch_row(result);
	return(atoi(row[0]));
    }
} /* End sql_tech_num_find */

/* Update the layers in the database */
void old_layers_update(char *off_data_dir, char *fab, char *technology, char approve_status)
{
    char comment_field[80];
    char layer_field[32];
    int sql_errno;
    int ret;
    FILE *fp;
    char ps_file[FILE_SIZE];
    char line[2048];
    char query[1024];
    int tech_num;
    int fab_num;
    int num_rows;
    MYSQL sql;
    MYSQL_RES *result;
    char secondary_approve_status;

    /* Find the fab num */
    fab_num = fab_lookup(fab);
    /* If this is an invalid fab, return */
    if (fab_num <= 0)
    {
	sprintf(rtn.msg, fab_num_err, technology);
	data_log(tr.log_file, rtn.msg);
	return;
    } /* end if */
    /* find the technology number */
    tech_num = sql_tech_num_find(technology, DBYES);
    if (tech_num == 0)
    {
	sprintf(rtn.msg, tech_num_err, technology);
	data_log(tr.log_file, rtn.msg);
	return;
    }
    /* build the name for the process step file */
    strcpy2(ps_file, off_data_dir, "/.db/process_step");
    /* Verify the file is present */
    if ((fp = fopen(ps_file, "r")) == NULL)
    {
	sprintf(rtn.msg, non_err, ps_file);
	data_log(tr.log_file, rtn.msg);
	return;
    }
    /* Set the secondary approve status */
    /* Cannot remember what this is for */
    if (approve_status == '*') secondary_approve_status = '-';
    else secondary_approve_status = approve_status;
    /* Skip past first two lines */
    long_line_get(fp, line); long_line_get(fp, line);
    /* Initialize the library */

    /* Connect */
     sql_connect(&sql, DTMS_DB, "common");
    /* Go through the file checking each layer */
    /* Read information from process step table */
    while (long_line_get(fp, line) != EOF)
    {
	/* find the layer name and comment */
    layer_comment_find(line, layer_field, comment_field);
	/* See if this record is already present */
	sprintf(query, "select * from common.common_template_layers where common_process_step_name = '%s' and common_id_technology = %d and common_id_fab = %d",
    layer_field, tech_num, fab_num);
    data_log("query", query);
	/* Get the results */
	result = connected_result_get(&sql, query);
	/* Count the number of rows */
    num_rows = mysql_num_rows(result);
    /* If row not present, insert it */
	if (num_rows < 1)
    /* Build query */
	sprintf(query, "insert into common.common_template_layers (common_userid, common_id_fab, common_id_technology, common_process_step_name, common_datetime, common_description, common_approve_status) \
    values('%s', %d, %d, '%s',  NOW(), '%s', '%c')",
    user.email, fab_num, tech_num, layer_field, comment_field, approve_status);
     else sprintf(query, "update common.common_template_layers set common_userid = '%s', common_description = '%s', common_approve_status = '%c',\
    common_datetime = now()\
	where common_process_step_name = '%s' and common_id_technology = %d and common_id_fab = %d",
    user.email, comment_field, approve_status,
    layer_field, tech_num, fab_num);
    data_log("query", query);
	ret = mysql_query(&sql, query);
	sql_errno = mysql_errno(&sql);
	if (sql_errno != 0)
	{
	    const char *err_ptr = mysql_error(&sql);
	    sprintf(rtn.msg, sql_err, sql_errno, query, err_ptr);
	data_log("sql_errs", query);
	data_log("sql_errs", rtn.msg);
	} /* End if */
    } /* end while */
    /* close the file */
    fclose(fp);
    /* Close the database connection */
    mysql_close(&sql);
    mysql_library_end();
} /* end old_layers_update */

/* Look up fab in local file */
int fab_lookup(char *fab)
{
    FILE *fp;
    int fab_num;
    char new_fab[20];
    char line[80];

    errno = 0;
    if ((fp = fopen("db_admin/fabs", "r")) == NULL)
    return(0);
    while (line_get(fp, line) != EOF)
    {
	sscanf(line, "%s %d", new_fab, &fab_num);
	if (strcmp(fab, new_fab) == 0)
	{
	    fclose(fp);
	    return(fab_num);
	} /* end if for fab found */
    } /* end while */
    /* close file and indicate not found */
    fclose(fp);
    return(0);
} /* end fab_lookup */

/* Inserts records for start and end of Reticle Builder */
int sql_reticle_bld_insert(char *mask, char *state, char *user_id)
{
    char query[1024];

    sprintf(query, "insert into mwr.reticlebld (reticlebld_maskset, reticlebld_state, reticlebld_userid, reticlebld_datetime)\
    values ('%s', '%s', '%s', now())",
    mask, state, user_id);
    insert_data(DTMS_DB, "mwr", query, DBNO);
    return(SUCCESS);
} /* end sql_reticle_bld_insert */

/* Find the layer and comment in a line from the process_step table */
    int layer_comment_find(char *line, char *layer_field, char *comment_field)
{
    char *layer_ptr;
    char *comment_ptr;
    char *ptr;

    /* Look for the first quote in the line */
    /* It should be at column 1 */
    ptr = strchr(line, '"');
    if (ptr == NULL) return(FAILURE);
    layer_ptr = ptr+1;
    /* Now find the end of the string */
    ptr = strchr(layer_ptr, '"');
     ptr[0] = '\0';
    strcpy(layer_field, layer_ptr);
    /* Now look for the first character of the comment */
    ptr = strchr(ptr+1, '"');
    comment_ptr = ptr+1;
    /* Now look for the end of the comment */
    ptr = strchr(comment_ptr, '"');
    ptr[0] = '\0';
    strcpy(comment_field, comment_ptr);
    return(0);
} /* end layer_comment_find */

/* Scan send2maskshop_log and put data */
/* in common_maskshop_layers when data sent to mask shop */
int send2maskshop_log_scan(char *file_name, int multiple_entries)
{
    char sql_date[32];
    int cnt;
    int ret;
    FILE *fp;
    char *ptr;
    char line[260];
    char query[1024];
    char mask[8];
    char revision[8] = "NULL";
    char user_id[260];
    char maskshop[48];
    char *plates[500];
    char tmp_plate[32];
    int num_plates = 0;

    if ((fp = fopen(file_name, "r")) == NULL)
    return(FAILURE);
label: cnt = 0;
    while (line_get(fp, line) != EOF)
    {
	if (strncmp(line, "               Send To Mask Shop History For Mask Set", 53) == 0)
	{
	     if ((ptr = strchr(line, '\'')) != NULL)
	    strncpy(mask, ptr+1, 4);
	    /* Verify this is a valid mask set */
	    if (is_mask(mask) != DBYES) return;
	} /* end if */
	/* Look for the date */
	else if (strncmp(line, "Date:", 5) == 0)
	sql_date_convert(sql_date, line+11);
	else if (strncmp(line, "Sender's email:", 15) == 0)
	{
	    sscanf(line, "%*s %*s %s", user_id);
	    if ((ptr = strchr(user_id, '@')) != NULL)
	    ptr[0] = '\0';
	} /* End if */
	else if (strncmp(line, "Mask shop:", 10) == 0)
	strcpy(maskshop, line+12);
	else if (strncmp(line, "Number of plates:", 17) == 0)
	sscanf(line, "%*s %*s %*s %d", &num_plates);
	else if (strncmp(line, "Plate names:", 12) == 0)
	break;
    } /* End while */
    /* Scan in the plate names */
    for (cnt = 0; cnt <num_plates; ++cnt)
    {
	fscanf(fp, "%s", tmp_plate);
	     plates[cnt] = malloc(strlen(tmp_plate)+1);
	    strcpy(plates[cnt], tmp_plate);
	} /* End for */
	/* Now put plates found on the list */
    plates[cnt] = NULL;
    /* sort into an ordered list */
     files_sort(plates);
    /* Now insert the records */
    for (cnt = 0; plates[cnt] != NULL; ++cnt)
   {
	/* Separate plate from revision */
	strcpy(tmp_plate, plates[cnt]);
	if ((ptr = strrchr(tmp_plate, '-')) != NULL)
	{
	ptr[0] = '\0';
	    ++ptr;
	}
	else ptr = revision;
    sprintf(query, "insert into common.common_maskshop_layers \
(common_maskid,common_layer_name,common_revision, common_layer_revision,common_maskshop,common_userid,common_datetime) \
values('%s', '%s',  '%s',  '%s',  '%s',  '%s',  '%s')",
	mask, tmp_plate, ptr, plates[cnt], maskshop, user_id, sql_date);
    /* do not insert if this is for the Reticle Technology Center */
    if (strcmp(maskshop, "Reticle Technology Center") != 0)
	insert_data(DTMS_DB, "common", query, DBNO);
    } /* End for */
    /* See if more history is present */
    /* If looking for multiple entries in the file, look for beginning of next entry */
    if (multiple_entries == DBYES)
    {
	while(line_get(fp, line) != EOF)
	{
	    if (strncmp(line, "----------", 10) == 0)
	    goto label;
	} /* End while */
    } /* End if */
    fclose(fp);
}

/* Add entry when new mask set created */
void sql_mask_create_add(char *mask, char *user_id)
{
    char query[1024];

	    /* Verify this is a valid mask set */
	    if (is_mask(mask) != DBYES) return;
    /* Now insert the records */
    sprintf(query, "insert into common.common_maskshop_layers \
(common_maskid, common_userid, common_event,common_datetime) \
values('%s', '%s',  '%s',  now())",
mask, user_id, "Create");
	insert_data(DTMS_DB, "common", query, DBNO);
} /* end sql_mask_create_add */

int separate_logs_bld(char *file_name)
{
    int index = 0;
    int write = DBNO;
    FILE *fi;
    FILE *fo;
    char line[260];
    char output_file[32];

    if ((fi = fopen(file_name, "r")) == NULL)
    return(FAILURE);
    sprintf(output_file, "file%d", index);
    fo = fopen(output_file, "w");
    while (line_get(fi, line) != EOF)
    {
	if (strncmp(line, "----------", 10) == 0)
	write = DBYES;
	else if ((strncmp(line, "Version", 7) == 0)
	&& (write == DBYES))
	{
	    write = DBNO;
	if (fo != NULL) fclose(fo);
	    ++index;
	sprintf(output_file, "file%d", index);
	    fo = fopen(output_file, "w");
	}
	if (write == DBYES)
	fprintf(fo, "%s\n", line);
    } /* End while */
    fclose(fi);
    fclose(fo);
} /* end */

/* Find the fab for this mask set */
void fab_find(char *fab, char *mask)
{
    FILE *fp;
    char create_file[32];

    /* build create file name */
    sprintf(create_file, "mask_data/%s/.db/create", mask);
    if ((fp = fopen(create_file, "r")) == NULL)
    {
	strcpy(fab, "unknown");
	return;
    }
    if (fscanf(fp, "%[^;]", fab) == 0)
    strcpy(fab, "unknown");
    fclose(fp);
} /* End fab_find */

/* Map the old fab name to the new name */
void die_fab_map(char *fab)
{
    char old_fab[32];
    char new_fab[32];
    char line[80];
    FILE *fp;

    if ((fp = fopen("db_admin/die_fab_mapping", "r")) == NULL)
    return;
    while (line_get(fp, line) != EOF)
    {
	sscanf(line, "%s %s", old_fab, new_fab);
    if (strcmp(old_fab, fab) == 0)
	{
	    strcpy(fab, new_fab);
	break;
	} /* End if */
    } /* End while */
    fclose(fp);
} /* end die_fab_map */


/* Update the fab name in the die_area_table */
void sql_die_area_fab_name_update(char *mask)
{
    char fab[32];
    char query[1024];

    /* First get the fab name */
    fab_find(fab, mask);
    /* Map the fab to its new name */
    die_fab_map(fab);
    /* Build the query to execute */
    sprintf(query, "update common.die_area set fab = '%s' where die_maskset = '%s'",
    fab, mask);
    insert_data(DTMS_DB, "common", query, DBNO);
}

/* Insert data into the mask_ccc table */
int mask_ccc_insert(MYSQL *sql, char   *device, char *layer_id,  char *plate_id,
char *fab, char *tech,
 float percent_clear)
{
    int sql_errno;
    char query[1024];

    sprintf(query, "insert into maskops.mask_ccc (device, layer_id, plate_id, percent_clear,\
    fab, technology, create_date, modification_date) \
    values ('%s', '%s', '%s', %f, '%s', '%s', now(), now())\
    on duplicate key update percent_clear=%f, modification_date = now()",
    device, layer_id, plate_id, percent_clear, fab, tech, percent_clear);
	/* Run the query */
	sql_query(sql, query);
    data_log("query", query);
	sql_errno = mysql_errno(sql);
	if (sql_errno != 0)
	{
	    const char *err_ptr = mysql_error(&sql);
	    sprintf(rtn.msg, sql_err, sql_errno, query, err_ptr);
	new_data_log("sql_errs", rtn.msg);
	} /* End if */
    return(sql_errno);

} /* End mask_ccc_insert */
void ccc_scan(char *file_name, char *mask)
{
    int cnt;
    FILE *fp;
    FILE *fo;
    char create_file[32];
    char layer[64];
    char plate_id[64];
    char line[260];
    float percent;
    MYSQL sql;
    char query[1024];
    char mail_file[64];
    char subject[32];
    char fab_email[64];

    /* Initialize the name of the mail file */
    strcpy2(mail_file, mask_msg.key1, tr.pid_string);
    if ((fo = fopen(mail_file , "w")) == NULL)
    close_down1m(wr_open_err, mail_file, WR_OPEN_ERR);
    /* Read the create file to get the fab and tech */
    strcpy2(create_file, mti.data_dir, "/.db/create");
    create_read(create_file);
    /* Open the file */
    if ((fp = fopen(file_name , "r")) == NULL)
    close_down1m(rd_open_err, file_name, RD_OPEN_ERR);
    /* Open the SQL connection */
    sql_connect(&sql, DTMS_DB, "maskops");
    /* Get lines of text until past header */
    for (cnt = 0; cnt < 6; ++cnt)
    {
    line_get(fp, line);
	if (strncmp(line, "-----", 5) == 0)
	break;
    }
    /* Write the first line to the mail file */
fprintf(fo, "CCC results for the following layers:\n");
    /* Now ready to scan in the information */
    while (line_get(fp, line) > 0)
    {
	sscanf(line, "%s %s %f", plate_id, layer, &percent);
    fprintf(fo, "%s %s %3.2f%%\n", mask_msg.key1, layer, percent);
	/* Insert a new record */
	mask_ccc_insert(&sql, mask, layer, plate_id, mask_msg.par1, mask_msg.par2, percent);
    }
    fprintf(fo, "\n");
    /* Close the files */
    fclose(fp);
    fclose(fo);
    /* Close the connection */
    mysql_close(&sql);
    mysql_library_end();
    /* Updating metrics also */
    /* Open the file */
    if ((fp = fopen(file_name , "r")) == NULL)
    close_down1m(rd_open_err, file_name, RD_OPEN_ERR);
    /* Open the SQL connection */

    /* Get lines of text until past header */
    for (cnt = 0; cnt < 6; ++cnt)
    {
    line_get(fp, line);
	if (strncmp(line, "-----", 5) == 0)
	break;
    }
    /* Now ready to scan in the information */
    while (line_get(fp, line) > 0)
    {
	sscanf(line, "%s %s %f", plate_id, layer, &percent);
	/* Insert a new record */
	mask_ccc_insert(&sql, mask, layer, plate_id, mask_msg.par1, mask_msg.par2, percent);
    }
    /* Close the files */
    fclose(fp);
    /* Close the connection */
    mysql_close(&sql);
    mysql_library_end();
    if (strcmp(mask_msg.key2, "N/A") != 0)
    {
	strcpy2(subject, mask_msg.key1, " ccc report");
	if (strchr(mask_msg.key2, '@') == NULL)
	strcpy2(fab_email, mask_msg.key2, "@freescale.com");
	else strcpy(fab_email, mask_msg.key2);
	email_file_send("ccc.update", fab_email, subject, mail_file);
    sprintf(rtn.msg, "Sending %s CCC results for %s", fab_email, mask_msg.key1);
	data_log(tr.log_file, rtn.msg);
    } /* end if for valid e-mail address */
/*
    unlink(mail_file);
*/
} /* end ccc_scan */

/* Map fab to fab ID */
int sql_fab_id_get(char *fab)
{
    int fab_id;
    int num_fields;
    int num_rows;
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[FILE_SIZE];

    strcpy3(query, "select common_id_fab from common.common_listFabsAll_v where common_fab = '",
    fab, "'");
    data_log("query", query);
    result = result_get(DTMS_DB, "common", query, NULL);
    /* Parse out needed information */
    /* Count the number of rows */
    num_rows = mysql_num_rows(result);
    num_fields = mysql_num_fields(result);
    if ((num_fields < 1) || (num_rows < 1))
    {
	printf("Bad record 15\n");
	return(0);
    }
    /* Get the row */
    row = mysql_fetch_row(result);
    fab_id = atoi(row[0]);
    return(fab_id);
}

/* Update the common_template_layers record */
void sql_common_template_layers_record_update(MYSQL *sql, int fab_id, int technology_id, char *technology, char *flow,
  char *process_step, int process_step_order, char *revision, char *description, char *approve_status)
{
    MYSQL_RES *result;
    int num_rows;
    int sql_errno;
    int ret;
    char query[1024];

	sprintf(query, "insert into common.common_template_layers ( \
common_id_fab, common_id_technology, common_mds_technology, common_flow,common_process_step_name, common_process_step_order,  \
common_revision, common_userid, common_description, common_approve_status,  common_datetime) \
 values(%d, %d, '%s', '%s', '%s', %d, \
'%s', '%s', '%s', '%s', now())",
    fab_id, technology_id, technology, flow, process_step,  process_step_order,
revision, st.user_id, description, approve_status);
    data_log("query", query);
	ret = mysql_query(sql, query);
	sql_errno = mysql_errno(sql);
	if (sql_errno != 0)
	{
	    const char *err_ptr = mysql_error(&sql);
	    sprintf(rtn.msg, sql_err, sql_errno, query, err_ptr);
	new_data_log("sql_errs", rtn.msg);
	} /* End if */
} /* end sql_common_template_layers_record_update */

void sql_common_maskset_layers_record_update(MYSQL *sql, char *maskset, int fab_id, int technology_id, char *technology, char *flow)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    int num_rows;
    int sql_errno;
    int ret;
    char *update_ptr;
    char *maskset_field_ptr;
    char maskset_id_value[12];
    char query[1024];

    /* Build query to see if entry is already present */
    sprintf(query, "select common_id from common.common_maskset_layers where common_maskset_name = '%s' \
and common_flow = '%s'",
    maskset, flow);
    data_log("query", query);
    /* If the record is already present, just update the current record */
    /* Otherwise add a new record */
    result = result_get(DTMS_DB, "common", query, NULL);
    /* count the number of rows */
    num_rows = mysql_num_rows(result);
    /* If replacing put in the current maskset_id field name and value */
    /* If inserting skip common_id and common_maskset_name value */
    if (num_rows >= 1)
    {
	update_ptr = "replace";
	row = mysql_fetch_row(result);
	strcpy2(maskset_id_value, row[0], ",");
	maskset_field_ptr = "common_id, ";
    } else {
	update_ptr = "insert";
	strcpy(maskset_id_value, " ");
	maskset_field_ptr = "";
    }
	sprintf(query, "%s into common.common_maskset_layers (%s  \
common_maskset_name, common_id_fab, common_id_technology, common_mds_technology, common_flow, \
    common_userid, common_datetime) \
 values(%s '%s', %d, %d, '%s', '%s', '%s', now())",
    update_ptr, maskset_field_ptr, maskset_id_value, maskset, fab_id, technology_id, technology, flow,
    st.user_id);
    data_log("query", query);
	ret = mysql_query(sql, query);
	sql_errno = mysql_errno(sql);
	if (sql_errno != 0)
	{
	    const char *err_ptr = mysql_error(&sql);
	    sprintf(rtn.msg, sql_err, sql_errno, query, err_ptr);
	data_log("query", rtn.msg);
	} /* End if */
    else data_log("query", "success");
} /* end sql_common_maskset_layers_record_update */

void sql_common_template_booleans_record_update(MYSQL  *sql, int fab_id, int tech_id,
    char *fracture_style, char *ps, char *booleans)
{
    MYSQL_RES *result;
    int sql_errno;
    int ret;
    char query[1024];

    sprintf(query, "insert into common.common_template_booleans \
(common_id_fab, common_id_technology,  \
common_fracture_style, common_process_step_name, common_booleans) \
values(%d, %d, '%s', '%s', '%s')",
    fab_id, tech_id, fracture_style, ps, booleans);
    data_log("query", query);
	ret = mysql_query(sql, query);
	sql_errno = mysql_errno(sql);
	if (sql_errno != 0)
	{
	    const char *err_ptr = mysql_error(&sql);
	    sprintf(rtn.msg, sql_err, sql_errno, query, err_ptr);
	new_data_log("sql_errs", rtn.msg);
	} /* End if */
} /* end sql_common_template_booleans_record_update */

void old_sql_common_maskset_booleans_record_update(MYSQL  *sql,
char * maskset, char * process_step_name, char * revision,
char *description, char * fracture_style, char * booleans)
{
    MYSQL_RES *result;
    int sql_errno;
    int ret;
    char query[1024];

    int num_rows;

    /* See if the record is already present */
    sprintf(query, "select * from common.common_maskset_booleans where common_maskset_name = '%s' \
and common_process_step_name = '%s' and common_revision = '%s'",
    maskset, process_step_name, revision);
    /* Run the query */
    data_log("query", query);
    result = result_get(DTMS_DB, "common", query, NULL);
    /* count the number of rows */
    num_rows = mysql_num_rows(result);
    if (num_rows <= 0)
    sprintf(query, "insert into common.common_maskset_booleans \
(common_maskset_name, common_process_step_name, common_revision,  \
common_description, common_fracture_style, common_booleans)  \
values('%s', '%s', '%s', '%s', '%s', '%s')",
maskset, process_step_name, revision,
description, fracture_style, booleans);
    else sprintf(query, "update common_maskset_booleans \
(set common_description = '%s', common_fracture_style = '%s', common_booleans = '%s'  \
    where common_maskset_name = '%s' and common_process_step_name = '%s' \
    and common_revision = '%s'",
description, fracture_style, booleans,
maskset, process_step_name, revision);
    data_log("query", query);
	ret = mysql_query(sql, query);
	sql_errno = mysql_errno(sql);
	if (sql_errno != 0)
	{
	    const char *err_ptr = mysql_error(&sql);
	    sprintf(rtn.msg, sql_err, sql_errno, query, err_ptr);
	new_data_log("sql_errs", rtn.msg);
	} /* End if */
} /* end sql_common_maskset_booleans_record_update */

/* Unique index now present */
void sql_common_maskset_booleans_record_update(MYSQL  *sql,
char * maskset, char * process_step_name, char * revision,
char *description, char * fracture_style, char * booleans)
{
    MYSQL_RES *result;
    int sql_errno;
    int ret;
    char query[1024];
    char *err_ptr;

    int num_rows;

    sprintf(query, "insert into common.common_maskset_booleans \
(common_maskset_name, common_process_step_name, common_revision,  \
common_description, common_fracture_style, common_booleans)  \
values('%s', '%s', '%s', '%s', '%s', '%s') \
on duplicate key \
update common_description = '%s', common_fracture_style = '%s', common_booleans = '%s'  ",
maskset, process_step_name, revision,
description, fracture_style, booleans,
description, fracture_style, booleans);
    data_log("query", query);
	ret = mysql_query(sql, query);
	sql_errno = mysql_errno(sql);
	if (sql_errno != 0)
	{
	    err_ptr = mysql_error(&sql);
	    sprintf(rtn.msg, sql_err, sql_errno, query, err_ptr);
	new_data_log("sql_errs", rtn.msg);
	} /* End if */
} /* end sql_common_maskset_booleans_record_update */

void sql_common_maskset_cell_record_update(MYSQL  *sql,
char * maskset, char * cell_name, char *cell_type, char *fracture_style, char *revision)
{
    MYSQL_RES *result;
    int sql_errno;
    int ret;
    char query[1024];

    sprintf(query, "insert into common.common_maskset_cells \
(common_maskset_name, common_cell, common_cell_type, common_fracture_style, common_revision)  \
values('%s', '%s', '%s', '%s', '%s') \
on duplicate key update  common_cell_type = '%s', common_fracture_style = '%s'",
    maskset, cell_name, cell_type, fracture_style, revision,
    cell_type, fracture_style);
    data_log("query", query);
	ret = mysql_query(sql, query);
	sql_errno = mysql_errno(sql);
	if (sql_errno != 0)
	{
	    const char *err_ptr = mysql_error(&sql);
	    sprintf(rtn.msg, sql_err, sql_errno, query, err_ptr);
	new_data_log("sql_errs", rtn.msg);
	} /* End if */
} /* end sql_common_maskset_cell_record_update */

/* Verify mask set not already present in MARS */
int sql_mars_mask_check(char *mask)
{
    int num_rows;
    int num_fields;
    MYSQL_RES *result;
     char query[1024];

    /* Build the query */
    strcpy3(query, "select mars_maskset from common.common_mars where mars_maskset = '",
    mask, "'");
    data_log("query", query);
    result = result_get(DTMS_DB, "common", query, NULL);
    /* count number of rows and fields */
    /* There should be one row */
    num_rows = mysql_num_rows(result);
    num_fields = mysql_num_fields(result);
    if ((num_rows <= 0) || (num_fields <= 0))return(0);
    else return(1);
} /* End sql_mars_mask_check */

/* Delete entries for a fab and technology from common_template_layers */
/* This is done before new data is added */
void sql_current_table_delete(MYSQL *sql, char *table, int fab_id, int tech_id)
{
    char query[1024];

    sprintf(query, "delete from %s where common_id_fab = %d and common_id_technology = %d",
    table, fab_id, tech_id);
    /* Run the query */
    sql_query(sql, query);
} /* End sql_current_common_template_layers_delete */
