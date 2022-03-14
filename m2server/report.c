#include <stdio.h>
#include <strings.h>
#include "global_reg_prototypes.h"
#include "global_report.h"
#include "strcpy.h"

void report_field_convert(char *field, int cnt)
{
    char type[REPORT_ARRAY_SIZE] =
{
    's', 's', 's', 's', 't', 's', 's', 's',
};

    switch (type[cnt])
    {
	case 's': return;
	case 'i': size_convert(field); break;
	case 'e': elapsed_time_convert(field); break;
	case 't':  long_time_convert(field); break;
    } /* End switch */
} /* End report_field_convert */

int trim_size[9] = { 10,  8, 20, 10, 20, 10, 20, 20, 10, };

int report_input_scan(char *line, char rep[REPORT_ARRAY_SIZE][256])
{
    int ret = sscanf(line, "%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%s",
    rep[1], rep[2], rep[3], rep[4], rep[5], rep[6]);
    return(ret);
} /* End report_input_scan */

void report_fields_pad(char rep[REPORT_ARRAY_SIZE][256])
{
    int cnt;

    for (cnt = 0; cnt <REPORT_ARRAY_SIZE; ++cnt)
    {
	if (cnt == 7)ldapfsl_get_user_full_name(rep[5], rep[7]);
	else report_field_convert(rep[cnt], cnt);
	str_pad(rep[cnt], trim_size[cnt]);
	/* Make sure at least one blank space at end of name */
	rep[cnt][trim_size[cnt]-1] = ' ';
    } /* End for */
} /* End report_fields_pad */

void report_header_build(char *header, int fields[])
{
    char report_header[REPORT_ARRAY_SIZE][32] =
{
    "Date    ",
    "Mask set",
    "Tool",
    "Action",
    "Time",
    "User ID",
    "Data",
    "User name",
    "Date    ",
};

    int cnt;
    char tmp_field[32];
    int len = 0;

    for (cnt = 0; fields[cnt] != -1; ++cnt)
    {
	strcpy(tmp_field, report_header[fields[cnt]]);
	str_pad(tmp_field, trim_size[fields[cnt]]);
    len += strcpy1(header+len, tmp_field);
     } /* End for */
} /* report_header_build */
