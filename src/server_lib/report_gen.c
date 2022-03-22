#ifndef M2
/****************************************************************************
 *
 * Functions:	report_usage
 *
 * Description:	Various report functions
 *
 *		key1 holds list of fields to print for list requests
 *		par1 holds starting date
 *		par2 holds ending date
 * Author:	Renee Carter
 *
 ***************************************************************************/
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <alloca.h>
#include <unistd.h>
#include <time.h>
#include "close_down.h"
#include "files.h"
#include "global_server_errs.h"
#include "global_report.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_server_prototypes.h"
#include "user_params.h"
#include "global_debug.h" // Mazieri's debug facilities

#define USAGE   1
#define SUMMARY 2

struct record 
{
    char fields[REPORT_ARRAY_SIZE][256];
    int cnt;
    struct record *next_ptr;
};


#ifndef TRANSWEB
int report_fields_print(char *buffer, char rep[REPORT_ARRAY_SIZE][256], int fields[])
{
    DBG();

    int cnt;
    int len = 0;

    for (cnt = 0; fields[cnt]!= -1; ++cnt)
    {
	    len += strcpy1(buffer+len, rep[fields[cnt]]);
    } /* End for */

    /* Add carriage return to line */
    len += strcpy1(buffer+len, "\n");

    return(len);
} /* End report_fields_print */
#endif


#ifdef TRANSWEB
int report_fields_print(char *buffer, char rep[REPORT_ARRAY_SIZE][256], int fields[])
{
    DBG();

    int cnt;
    int len = 0;

    for (cnt = 0; cnt < REPORT_ARRAY_SIZE; ++cnt)
    {
	    if (fields[cnt] == -1) break;
	    len += strcpy2(buffer+len, rep[fields[cnt]], ";");
    } /* End for */

    /* Add carriage return to line */
    /* Add carriage return and chop off last semicolon */
    strcpy1(buffer+len-1, "\n");

    return(len);
} /* End report_fields_print */
#endif


/* Add new record to join list */
void record_add(struct record *root_ptr, int field_num, char fields[REPORT_ARRAY_SIZE][256])
{
    DBG();

    int num;
    int ret;
    int fnd = DBNO;
    struct record *new_record;
    struct record *ptr = root_ptr;
    struct record *old_ptr = root_ptr;

    /* Traverse the list looking for a record like this one */
    while (1)
    {
    	for (num = 0; num <field_num; ++num)
    	{
    	    ret = strcasecmp(fields[num], ptr->fields[num]);
    	    if (ret > 0) break;
    	} /* End for */

	    /* If ret = 0, equivalent record found */
	    /* Increment number of records found and return */
	    if (ret == 0)
	    {
	        ++ptr->cnt;
	        return;
	    }
	    /* If current record less than new record, insert record here */
	    else if (ret < 0)
	    {
	        new_record = malloc(sizeof(struct record));
	        old_ptr->next_ptr = new_record;
	        new_record->next_ptr = ptr;
	        /* Change the links to insert the new record */
	        fnd = DBYES;
	        break;
	    }
	    if (ptr->next_ptr != NULL)
	    {
	        old_ptr = ptr;
	        ptr = ptr->next_ptr;
	    }
	    else break;
    } /* end while */

     /* If reached this point add the record at the end */
    if (fnd == DBNO)
    {
	    new_record = malloc(sizeof(struct record));
	    ptr->next_ptr = new_record;
	    new_record->next_ptr = NULL;
    }

    /* Now put in the rest of the data */
    for (num = 0; num < field_num; ++num)
        strcpy(new_record->fields[num], fields[num]);

    new_record->cnt = 1;
} /* End add_record */


int list2filter_convert(FILTER_DATA *fi[], char *list[])
{
    DBG();

    FILTER_DATA local;
    int cnt = 0;
    int cnt1 = 0;
    int ret;

    /* Initialize all records to NULL */
    for (cnt = 0; cnt < FILTER_NUM; ++cnt)
        fi[cnt] = NULL;

    /* If no filter present, return */
    if (list[0] == NULL) return(0);
    if (strlen(list[0]) < 3) return(0);
    
    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
        ret = sscanf(list[cnt], "%d%c%[^,]",
	    &local.index, &local.type, local.value);
	{
    if (ret == 3)
	    fi[local.index] = malloc(sizeof(FILTER_DATA)+1);

	    fi[local.index]->type = local.type;
	    strcpy(fi[local.index]->value, local.value);
	    printf("local.value is %s\n", local.value);
	    ++cnt1;
    } /* End if */
    } /* End while */
    return(cnt1);
} /* End list2filter_convert */


/* Determines if this data needs to be filtered */
/* that is not included in the report generated */
/* Passes a two dimensional array with values of the current record */
/* Passes in the filter criteria as an array of pointers */
int filter_record(char data[REPORT_ARRAY_SIZE][256], FILTER_DATA *fi[])
{
    DBG();

    int len;
    int cnt;

    DBG("It may be a dangerous BUG ... se this !!!");
    return(DBNO);
    for (cnt = 0; cnt <FILTER_NUM; ++cnt)
    {
	    /* If no filter criteria present, skip this field */
	    if (fi[cnt] == NULL) continue;
	    len = strlen(fi[cnt]->value);

        if (fi[cnt]->type == EQUAL)
        {
	        if (strncmp(fi[cnt]->value, data[cnt], len) != 0) 
	        return(DBYES);
        } /* End if */
        else if (fi[cnt]->type == NOT_EQUAL)
        {
	        if (strncmp(fi[cnt]->value, data[cnt], len) == 0) 
	        return(DBYES);
        } /* End else */
    } /* End for */

  /* If reach this point, data is not filter */
  return(DBNO);
}
/* General function for building report lists */


/* General function for building report lists */
void report_usage(int req_type, char *date1, char *date2, char *scan_fields)
{
    DBG();

    char filter_string[BUFFER_SIZE]= "";
    FILTER_DATA *filter_array[FILTER_NUM];
    char *list_array[FILTER_NUM];
    int fieldnum;
    char values[REPORT_ARRAY_SIZE][256];
    struct record *root_ptr = malloc(sizeof(struct record));
    int ret;
    char buffer[BUFFER_SIZE];
    int fields[REPORT_ARRAY_SIZE];
    char line[260];
    char header[512];
    char rep[REPORT_ARRAY_SIZE][256];
    char file_name[FILE_SIZE];
    char tmp_file[64];
    char *file_ptr;
    int cnt;
    int total_cnt;
    char *files[5000];
    FILE *fi;
    FILE *fo;

    /* first get the filter string from the client */
    /* first build list of files to retrieve information from */
    if (names_build(date1, date2, files) <= 0)
        shut_down(bad_dates, BAD_DATES);

    /* Build first part of file name */
    file_ptr = file_name + strcpy1(file_name, REPORT_DIR);

    /* Build name for temporary file */
    strcpy(tmp_file, "/tmp/report_file");

    /* Open temporary file */
    if ((fo = fopen(tmp_file, "w")) == NULL)
        close_down1m(wr_open_err, tmp_file, WR_OPEN_ERR);

    /* Print header for file */
    /* Scan out fields to print */
    fieldnum =  report_fields_scan(scan_fields, fields);

#ifndef TRANSWEB
    if (req_type == USAGE)
        report_title_build(header, "Usage Report", date1, date2);
    else report_title_build(header, "Summary Report", date1, date2);
        fprintf(fo, "%s\n", header);
#endif

    report_header_build(header, fields);
    fprintf(fo, "%s\n", header);
    
    /* If doing a join, initialize first record */
    if (req_type == SUMMARY)
    {
	    root_ptr->next_ptr = NULL;
	    for (cnt = 0; cnt < fieldnum; ++cnt)
	        strcpy(root_ptr->fields[cnt], "!");
    } /* End if for doing a join */
    /* If valid filter string present, convert it */
    {
	/* convert to list format */
	string2list2convert(filter_string, list_array);
	/* Build the filter array */
	list2filter_convert(filter_array, list_array);
    } /* End if for valid filter string */
    /* Go through all the files found */
    for (cnt = 0,total_cnt = 0; files[cnt] != NULL; ++cnt)
    {
	/* build complete file name */
        strcpy(file_ptr, files[cnt]);
	/* Free file name in array no longer needed */
        free(files[cnt]);
        /* Open specified file */
        if ((fi = fopen(file_name, "r")) == NULL) 
        continue;
	/* Build date field for following loop */
	str_pad(file_ptr, 10);
	file_ptr[2] = file_ptr[5] = '/';
	/* copy value into fields array */
	strcpy(rep[DATE_FIELD], file_ptr);
	strcpy(rep[0], file_ptr);
        while((ret = line_get(fi, line)) > 0)
        {
	if ((ret =   report_input_scan(line, rep)) != REPORT_SCAN_SIZE)  
	{
	printf("ret is %d and line is %s\n", ret, line);
	    continue;
	}
	    /* See if this record should be included */
	if (filter_record(rep, filter_array) == DBNO) 
	{
	  if (req_type == USAGE)
	  {
	        /* Pad all the fields */
#ifndef TRANSWEB
	        report_fields_pad(rep);
#endif
	        report_fields_print(buffer, rep, fields);
	        fprintf(fo, buffer);
	    ++total_cnt;
	/* This is for a summary report */
	} else {
	    /* Put values from line into the values structure */
	    /* If looking for name set it */
	for (cnt = 0; cnt < fieldnum; ++cnt)
	    strcpy(values[cnt], rep[fields[cnt]]);
	    /* See if this record is already on the list */
	    /* If it is not on the list, add it */
	    record_add(root_ptr, fieldnum, values);
	} /* End else */
	} /* End if for record not filtered out */
        } /* End while */
        fclose(fi);
    } /* End for */
    /* If doing join, write out records to tmp file */
    if (req_type == SUMMARY)
    {
	struct record *ptr = root_ptr->next_ptr;
	while (ptr != NULL)
	{
	    for (cnt = 0; cnt < fieldnum; ++cnt)
	{
	    report_field_convert( ptr->fields[cnt], fields[cnt]);
	    fprintf(fo, "%s    ", ptr->fields[cnt]);
	}
	/* Printing percent this record is */
	fprintf(fo, "%d    %d%%\n", ptr->cnt, 100*(ptr->cnt)/total_cnt);
	    ptr = ptr->next_ptr;
	} /* End for */
    } /* End if for doing a join */
    /* If records found, write to file */
    if (total_cnt > 0) fprintf(fo, "\nTotal records:  %d\n", total_cnt);
    /* Close the temp file */
    fclose(fo);
     if (total_cnt == 0) 
    {
        unlink(tmp_file);
        shut_down(no_weekly_data, NO_WEEKLY_DATA);
    } /* End if */
    /* Otherwise send file */
    else
    {
        /* Indicate ready to send data */
	put_short(SUCCESS);
        file_put("/tmp", tmp_file+5);
        /* Send return message */
        strcpy(rtn.msg, "Report shown below:");
        return_success(rtn.msg, DBNO);
     } /* End else */
    /* Delete file no longer needed */
/*
    unlink(tmp_file);
*/
    exit(0);
} /* end function report_usage */


void report_header_print(FILE *fo, char *report_title, char *date1, char *date2)
{
    DBG();

    fprintf(fo, "%s\n\n", report_title);
    fprintf(fo, "Date Generated:  %s", ctime(&tr.upd_time));
    
    if (strcmp(date1, date2) != 0)
        fprintf(fo, "Dates covered:  %s to %s\n\n", date1, date2);
    else if (strcmp(date1, NONE) == 0)
        fprintf(fo, "Report for:  %s\n\n", tr.log_file+strlen(tr.log_file)-8);
    else fprintf(fo, "Report for:  %s\n\n", date1);
} /* report_header_print */


/* Report specified fields */
void elapsed_time_convert(char *string)
{
    DBG("string='%s'", string);

    long time = atol(string);
    char time_string[64];
    long new_time ;
    int len= 0;

    /* Calculate the number of hours */
    new_time = time/3600;
    if (new_time > 0)
    {
        sprintf(time_string, "%ld hour", new_time);
        len = strlen(time_string);

        if (new_time > 1) time_string[len++] = 's';
            time_string[len++] = ' ';
    }
    
    new_time = time/60%60;
    if (new_time > 0)
    {
        sprintf(time_string+len, "%ld minutes", new_time);
        len += strlen(time_string+len);
        
        if (new_time == 1) time_string[--len] = '\0';
            time_string[len++] = ' ';
    }
    
    new_time = time%3600%60;
    if (new_time > 0)
    {
        sprintf(time_string+len, "%ld seconds", new_time);
        len += strlen(time_string+len);
        if (new_time == 1) time_string[len-1] = '\0';
    }

    if (len == 0) strcpy(time_string, "0 seconds");
    
    /* Copy calculated time over original time */
    strcpy(string, time_string);
} /* end elapsed_time_convert */


void size_convert(char *size_string)
{
    DBG("size_string='%s'", size_string);

    long long size = atoll(size_string);
    
    if (size < 1000 * 1000)
	    sprintf (size_string, "%lld bytes", size);
    else if (size < 1000*1000 * 1000)
	    sprintf (size_string, "%0.2f MB", 1.0 * size / 1000 / 1000);
	else 
	    sprintf (size_string, "%0.4f GB", 1.0 * size / 1000/ 1000/1000);
} /* End size_convert */


void long_time_convert(char *time_string)
{
    DBG("time_string='%s'", time_string);

    long time = atol(time_string);
    char *time_ptr = ctime(&time);
    strncpy(time_string, time_ptr+4, 15);
}


/* Pad with spaces a single field */
void report_field_trim(char *field, int index)
{
    DBG("field='%s' and index='%d'", field, index);

    int trim_size[8] = { 10, 8, 20, 10, 20, 10, 20, 20 };
    str_pad(field, trim_size[index]);
}


int report_fields_scan(char *scan_fields, int fields[])
{
    DBG();

    char tmp_fields[4];
    int scan_cnt;
    int field_cnt;

    for (scan_cnt = 0, field_cnt = 0; scan_fields[scan_cnt] != 0; ++field_cnt)
    {
	    tmp_fields[0] = scan_fields[scan_cnt];
	    ++scan_cnt;
	
	    if (scan_fields[scan_cnt] == ',') 
	    {
	        tmp_fields[1] = 0;
	        ++scan_cnt;
	    } else {
	        tmp_fields[1] = scan_fields[scan_cnt];
	        tmp_fields[2] = 0;
	        scan_cnt += 2;
	    } /* End else */
	
	    fields[field_cnt] = atoi(tmp_fields);
    } /* End for */

    fields[field_cnt] = -1;

    return(field_cnt);
} /* End report_fields_scan */


int report_title_build(char *string, char *report_title, char *date1, char *date2)
{
    DBG();
    
    int len;
    sprintf(string, "%s\n\n", report_title);
    len = strlen(string);
    sprintf(string+len, "Date Generated:  %s", ctime(&tr.upd_time));
    len += strlen(string+len);
    
    if (strcmp(date1, date2) != 0)
        sprintf(string+len, "Dates covered:  %s to %s\n\n", date1, date2);
    else sprintf(string+len, "Report for:  %s\n\n", date1);
        len += strlen(string+len);

    return(len);
} /* report_title_build */
#endif
