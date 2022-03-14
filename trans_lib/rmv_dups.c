#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "close_down.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "global_debug.h"

/* This function takes all duplicate lines out of a file */
void rmv_dups(char *input_file, char *output_file)
{
    FILE *fi;
    FILE *fo;
    char old_line[260];
    char line[260];

    DBG();

    if ((fi = fopen(input_file, "r")) == NULL)
    close_down1f(rd_open_err, input_file, RD_OPEN_ERR);
    if ((fo = fopen(output_file, "w")) == NULL)
    close_down1f(wr_open_err, output_file, WR_OPEN_ERR);
    old_line[0] = 0;
    while (line_get(fi, line) > 0)
    {
	if (strcmp(line, old_line) != 0)
	{
	    fprintf(fo, "%s\n", line);
	    strcpy(old_line, line);
	}
    }
    fclose(fi);
    fclose(fo);
}
