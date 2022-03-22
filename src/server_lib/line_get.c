#define DBYES 1
#define DBNO 2
/****************************************************************************
 *
 * Function:    line_get
 *
 * Description:    Retrieves a line of text from a file using a
 *                 stream pointertrieves the user's
 *
 *Returns:	Returns the number of characters retrieved.
 *		    Returns EOF if end of file reached and no text found
 *
 ***************************************************************************/

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "global_debug.h" // Mazieri's debug facilities


int line_get(FILE *fp, char *line)
{
    /*DBG("fp='%p', line='%s'", fp, line);*/

    char eof_fnd = DBNO;
    int cnt;

    cnt = 0;
    while (((line[cnt] = getc(fp)) != '\n') && (line[cnt] != EOF) && (cnt < 260)) 
        ++cnt;

    /* Set flag if end of file reached */
    if (line[cnt] == EOF) eof_fnd = DBYES;

    /* Put null at end of line */
    line[cnt] = '\0';

    /* Return the number of characters found */
    /* or Return EOF if end of file reached and no characters found */
    if ((eof_fnd == DBYES) && (cnt <= 0)) return(EOF);
    else return(cnt);
} /* End line_get */


int long_line_get(FILE *fp, char *line)
{
    /*DBG("fp='%p', line='%s'", fp, line);*/

    char eof_fnd = DBNO;
    int cnt;

    cnt = 0;
    while (((line[cnt] = getc(fp)) != '\n') && (line[cnt] != EOF) && (cnt < 2048)) 
        ++cnt;

    /* Set flag if end of file reached */
    if (line[cnt] == EOF) eof_fnd = DBYES;

    /* Put null at end of line */
    line[cnt] = '\0';

    /* Return the number of characters found */
    /* Return EOF if end of file reached and no characters found */
    if (eof_fnd == DBYES) return(EOF);
    else return(cnt);
} /* End long_line_get */


/* Return the last line in a file */
int last_line_get(char *file_name, char *line)
{
    /*DBG("file_name='%p', line='%s'", file_name, line);*/

    FILE *fp;
    char tmp_line1[2048];
    char tmp_line2[2048];

    if ((fp = fopen(file_name, "r")) == NULL)
    {
	    line[0] = '\0';
	    return(-1);
    }

    while (long_line_get(fp, tmp_line1) != EOF)
    {
	    strcpy(line, tmp_line2);
	    strcpy(tmp_line2, tmp_line1);
    }

    fclose(fp);

    return(0);
} /* End last_line_get */


/* Get line from open stream pointer */
int line_get_fp(FILE *fp, char *line, int len)
{
    /*DBG("fp='%p', line='%s' and len='%d'", fp, line, len);*/

    char eof_fnd = DBNO;
    int cnt;

    cnt = 0;
    while (((line[cnt] = getc(fp)) != '\n') && (line[cnt] != EOF) && (cnt < len))
        ++cnt;

    /* Set flag if end of file reached */
    if (line[cnt] == EOF) eof_fnd = DBYES;

    /* Put null at end of line */
    line[cnt] = '\0';

    /* Return the number of characters found */
    /* or Return EOF if end of file reached and no characters found */
    if (eof_fnd == DBYES) return(EOF);
    else return(cnt);
} /* End line_get */
