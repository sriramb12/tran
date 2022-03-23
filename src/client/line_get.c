/****************************************************************************
 *
 * Function:    line_get
 *
 * Description:    Retrieves a line of text from a file using a stream pointertrieves the user's 
 *
 *Returns:	Returns the number of characters retrieved.
 *		Returns EOF if end of file reached and no text found
 *
 * Author:    Renee Carter
 *
 ***************************************************************************/

#include "global_defines.h"
#include <stdio.h>
#include "trans_prototypes_2.h"


#include "global_debug.h"


int line_get(FILE *fp, char *line)
{
    DBG();
    char eof_fnd = DBNO;
    int cnt;

    cnt = 0;
    while (((line[cnt] = getc(fp)) != '\n') && (line[cnt] != EOF)
    && (cnt < 1024)) ++cnt;
    /* Set flag if end of file reached */
    if (line[cnt] == EOF) eof_fnd = DBYES;
    /* Put null at end of line */
    line[cnt] = '\0';
    /* Return the number of characters found */
    /* Return EOF if end of file reached and no characters found */
    if (eof_fnd == DBYES) return(EOF);
    else return(cnt);
} /* End line_get */

int long_line_get(FILE *fp, char *line)
{
    DBG();
    char eof_fnd = DBNO;
    int cnt;

    cnt = 0;
    while (((line[cnt] = getc(fp)) != '\n') && (line[cnt] != EOF)
    && (cnt < 2048)) ++cnt;
    /* Set flag if end of file reached */
    if (line[cnt] == EOF) eof_fnd = DBYES;
    /* Put null at end of line */
    line[cnt] = '\0';
    /* Return the number of characters found */
    /* Return EOF if end of file reached and no characters found */
    if (eof_fnd == DBYES) return(EOF);
    else return(cnt);
} /* End long_line_get */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
