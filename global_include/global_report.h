#ifndef global_report_h
#define global_report_h

#define FILTER_NUM 11
#ifdef M2
#define DATE_FIELD 8
#define REPORT_ARRAY_SIZE 9
#define USERID_FIELD 5
#define NAME_FIELD 7
#define REPORT_DIR "event_log/"
#define REPORT_SCAN_SIZE 6
#else
#define REPORT_ARRAY_SIZE 20
#define DATE_FIELD 17
#define REPORT_DIR "report_log/"
#define REPORT_SCAN_SIZE 17
#endif


/* General function for building report lists */
void report_list(
    int req_type,
    char *date1, 
    char *date2, 
    char *keep_filter1, 	/* Holds keep_filter index and keep_filter value */
    char *keep_filter2,	/* Holds keep_filter index and keep_filter value */
    char *discard_filter1, 	/* Holds discard_filter index and discard_filter value */
    char *discard_filter2,	/* Holds discard_filter index and discard_filter value */
 char *scan_fields);


void report_header_print(FILE *fo, char *report_title, char *date1, char *date2);


#ifdef TRANSWEB
int report_fields_print(char *buffer, char rep[REPORT_ARRAY_SIZE][256], int fields[]);
#endif

void elapsed_time_convert(char *string);
void size_convert(char *size_string);

void long_time_convert(char *time_string);


/* Pad with spaces a single field */
void report_field_trim(char *field, int index);

int report_fields_scan(char *scan_fields, int fields[]);

int report_title_build(char *string, char *report_title, char *date1, char *date2);

#endif
