/****************************************************************************
 *
 * Function:	strcpy
 *
 * Description:	Collection of functions used to copy one or more source
 *		strings into a destination string.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#define DBYES 1
#define DBNO 2
#include <alloca.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities

/* Copies source string into destination string returning the number */
/* of characters returned. */
int strcpy1(char *result, char *string)
{
    DBG("result='%s' and string='%s'", result, string);

    int length;

    for (length = 0; string[length] != 0; ++length)
    {
        result[length] = string[length];
    }

    result[length] = 0;

    return(length);
} /* End strcpy1 */


/* Copy contents of two strings into destination string */
int strcpy2(char *result, char *str1, char *str2)
{
    DBG("result='%s', str1='%s' and str2='%s'", result, str1, str2);

    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    result[length] = 0;
    return(length);
} /* End strcpy2 */


/* Copy contents of three strings into destination string */
int strcpy3(char *result, char *str1, char *str2, char *str3)
{
    DBG("result='%s', str1='%s', str2='%s' and str3='%s'", result, str1, str2, str3);

    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }
    result[length] = 0;
    return(length);
} /* End strcpy3 */


/* Copy contents of four strings into destination string */
int strcpy4(char *result, char *str1, char *str2, char *str3, char *str4)
{
    DBG("result='%s', str1='%s', str2='%s', str3='%s' and str4='%s'", result, str1, str2, str3, str4);

    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    result[length] = 0;
    return(length);
} /* End strcpy4 */


/* Copy contents of 5 strings into destination string */
int strcpy5(char *result, char *str1, char *str2, char *str3, char *str4, char *str5)
{
    DBG("result='%s', str1='%s', str2='%s', str3='%s', str4='%s' and str5='%s'", result, str1, str2, str3, str4, str5);

    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    for (length2 = 0; str5[length2] != 0; ++length, ++length2)
    {
        result[length] = str5[length2];
    }

    result[length] = 0;

    return(length);
} /* End strcpy5 */


/* Copy contents of six strings into destination string */
int strcpy6(char *result, char *str1, char *str2, char *str3, char *str4, char *str5, char *str6)
{
    DBG("result='%s', str1='%s', str2='%s', str3='%s', str4='%s', str5='%s' and str6='%s'", result, str1, str2, str3, str4, str5, str6);

    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    for (length2 = 0; str5[length2] != 0; ++length, ++length2)
    {
        result[length] = str5[length2];
    }
    for (length2 = 0; str6[length2] != 0; ++length, ++length2)
    {
        result[length] = str6[length2];
    }
    
    result[length] = 0;

    return(length);
} /* End strcpy6 */


/* Copy contents of seven strings into destination string */
int strcpy7(char *result, char *str1, char *str2, char *str3, char *str4, char *str5, char *str6, char *str7)
{
    DBG("result='%s', str1='%s', str2='%s', str3='%s', str4='%s', str5='%s', str6='%s' and str7='%s'", result, str1, str2, str3, str4, str5, str6, str7);

    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    for (length2 = 0; str5[length2] != 0; ++length, ++length2)
    {
        result[length] = str5[length2];
    }
    for (length2 = 0; str6[length2] != 0; ++length, ++length2)
    {
        result[length] = str6[length2];
    }
    for (length2 = 0; str7[length2] != 0; ++length, ++length2)
    {
        result[length] = str7[length2];
    }

    result[length] = 0;

    return(length);
} /* End strcpy7 */


/* Copy contents of eight strings into destination string */
int strcpy8(char *result, char *str1, char *str2, char *str3, char *str4, char *str5, char *str6, char *str7, char *str8)
{
    DBG("result='%s', str1='%s', str2='%s', str3='%s', str4='%s', str5='%s', str6='%s', str7='%s' and str8='%s'", result, str1, str2, str3, str4, str5, str6, str7, str8);

    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    for (length2 = 0; str5[length2] != 0; ++length, ++length2)
    {
        result[length] = str5[length2];
    }
    for (length2 = 0; str6[length2] != 0; ++length, ++length2)
    {
        result[length] = str6[length2];
    }
    for (length2 = 0; str7[length2] != 0; ++length, ++length2)
    {
        result[length] = str7[length2];
    }
    for (length2 = 0; str8[length2] != 0; ++length, ++length2)
    {
        result[length] = str8[length2];
    }

    result[length] = 0;

    return(length);
} /* End strcpy8 */

int strcpy9(char *result, char *str1, char *str2, char *str3, char *str4, char *str5, char *str6, char *str7, char *str8, char *str9)
{
    DBG("result='%s', str1='%s', str2='%s', str3='%s', str4='%s', str5='%s', str6='%s', str7='%s', str8='%s' and str9='%s'", result, str1, str2, str3, str4, str5, str6, str7, str8, str9);

    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    for (length2 = 0; str5[length2] != 0; ++length, ++length2)
    {
        result[length] = str5[length2];
    }
    for (length2 = 0; str6[length2] != 0; ++length, ++length2)
    {
        result[length] = str6[length2];
    }
    for (length2 = 0; str7[length2] != 0; ++length, ++length2)
    {
        result[length] = str7[length2];
    }
    for (length2 = 0; str8[length2] != 0; ++length, ++length2)
    {
        result[length] = str8[length2];
    }
    for (length2 = 0; str9[length2] != 0; ++length, ++length2)
    {
        result[length] = str9[length2];
    }

    result[length] = 0;

    return(length);
} /* End strcpy9 */

int strcpy10(char *result, char *str1, char *str2, char *str3, char *str4, char *str5, char *str6, char *str7, char *str8, char *str9, char *str10)
{
    DBG("result='%s', str1='%s', str2='%s', str3='%s', str4='%s', str5='%s', str6='%s', str7='%s', str8='%s', str9='%s' and str10='%s'", result, str1, str2, str3, str4, str5, str6, str7, str8, str9, str10);

    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    for (length2 = 0; str5[length2] != 0; ++length, ++length2)
    {
        result[length] = str5[length2];
    }
    for (length2 = 0; str6[length2] != 0; ++length, ++length2)
    {
        result[length] = str6[length2];
    }
    for (length2 = 0; str7[length2] != 0; ++length, ++length2)
    {
        result[length] = str7[length2];
    }
    for (length2 = 0; str8[length2] != 0; ++length, ++length2)
    {
        result[length] = str8[length2];
    }
    for (length2 = 0; str9[length2] != 0; ++length, ++length2)
    {
        result[length] = str9[length2];
    }
    for (length2 = 0; str10[length2] != 0; ++length, ++length2)
    {
        result[length] = str10[length2];
    }

    result[length] = 0;

    return(length);
} /* End strcpy10 */

int strcpy12(char *result, char *str1, char *str2, char *str3, char *str4, char *str5, char *str6, char *str7, char *str8, char *str9, char *str10, char *str11, char *str12)
{
    DBG("result='%s', str1='%s', str2='%s', str3='%s', str4='%s', str5='%s', str6='%s', str7='%s', str8='%s', str9='%s', str10='%s', str11='%s' and str12='%s'", result, str1, str2, str3, str4, str5, str6, str7, str8, str9, str10, str11, str12);

    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    for (length2 = 0; str5[length2] != 0; ++length, ++length2)
    {
        result[length] = str5[length2];
    }
    for (length2 = 0; str6[length2] != 0; ++length, ++length2)
    {
        result[length] = str6[length2];
    }
    for (length2 = 0; str7[length2] != 0; ++length, ++length2)
    {
        result[length] = str7[length2];
    }
    for (length2 = 0; str8[length2] != 0; ++length, ++length2)
    {
        result[length] = str8[length2];
    }
    for (length2 = 0; str9[length2] != 0; ++length, ++length2)
    {
        result[length] = str9[length2];
    }
    for (length2 = 0; str10[length2] != 0; ++length, ++length2)
    {
        result[length] = str10[length2];
    }
    result[length] = 0;
    for (length2 = 0; str11[length2] != 0; ++length, ++length2)
    {
        result[length] = str11[length2];
    }
    result[length] = 0;
    for (length2 = 0; str12[length2] != 0; ++length, ++length2)
    {
        result[length] = str12[length2];
    }
    result[length] = 0;
    return(length);
} /* End strcpy12 */

/* Copy string exchanging old character for new character */
int strcpy_ex(char *new_string, char *old_string, char new_char, char old_char)
{
    DBG("new_string='%s', old_string='%s'", new_string, old_string);

    int cnt;

    for (cnt = 0; old_string[cnt] != '\0'; ++cnt)
    {
	    if (old_string[cnt] == old_char) new_string[cnt] = new_char;
	    else new_string[cnt] = old_string[cnt];
    } /* End for */

    /* Put null at end of string */
    new_string[cnt] = '\0';

    return(cnt);
} /* End strcpy_ex */


/* Looks for pattern at end of string */
/* Strip off everything after specified character */
void str_strip_char(char *string, char character)
{
    DBG("string='%s' and character='%s'", string, character);

    char *ptr;
    if ((ptr = strchr(string, character)) != NULL) ptr[0] = '\0';
} /* end str_strip_char */


/* If the pattern is found, it is stripped off */
void str_strip(char *string, char *pattern)
{
    DBG("string='%s' and pattern='%s'", string, pattern);

    register int len;

    len = strlen(string) -strlen(pattern);
    if (len > 0)
    {
	    if (strcmp(pattern, string+len) == 0) string[len] = '\0';
    } /* End for */
} /* End str_strip */


/* Strip off leading spaces */
void str_strip_leading_spaces(char *string)
{
    register int cnt;
    
    DBG("string='%s'", string);

    for (cnt = '\0'; string[cnt] != 0; ++cnt)
    {
	    if (string[cnt] != ' ') break;
    }

    strcpy(string, string+cnt);
} /* End str_strip_leading_spaces */


/* Strip leading path off of file name */
/* It strips off paths with / or \ in it */
void str_path_strip(char *file_name)
{
    DBG("file_name='%s'", file_name);

    char *ptr;

    if ((ptr = strrchr(file_name, '\\')) != NULL) strcpy(file_name, ptr+1);
    if ((ptr = strrchr(file_name, '/')) != NULL) strcpy(file_name, ptr+1);
} /* end str_path_strip */


/* Convert the specified string to lower case */
void str_lower(char *string)
{
    DBG("string='%s'", string);
    register int cnt;

    for (cnt = 0; string[cnt] != '\0'; ++cnt)
    {
	    if ((string[cnt] >= 65) && (string[cnt] <= 90)) string[cnt] += 32;
    } /* end for */
} /* End str_lower */


void str_upper(char *string)
{
    DBG("string='%s'", string);
    register int cnt;

    for (cnt = 0; string[cnt] != '\0'; ++cnt)
    {
	    if ((string[cnt] >= 97) && (string[cnt] <= 122)) string[cnt] -= 32;
    } /* end for */
} /* End str_upper */


/* Copies subset of source string to destination string */
/* Stops when specified character is found or buffer size for */
/* destination string has been reached */
/* Returns the length of the string if copy is successful */
int strcpy_substr(char *dest_string, char *src_string, char c, int len)
{
    DBG("dest_string='%s', src_string='%s', c='%c' and len='%d'", dest_string, src_string, c, len);

    register short cnt = 0;

    while((src_string[cnt] != c) && (src_string[cnt] != '\0') && (cnt < len))
    {
	    dest_string[cnt] = src_string[cnt];
	    ++cnt;
    } /* End while */

    /* Put null at the end of the string */
    dest_string[cnt] = '\0';
    if (src_string[cnt] == c) return(cnt);
    else return(-1);
} /* End strcpy_substr */


/* Finds position of specified character in string */
/* Returns -1 if character not found */
int strfind_char(char *string, char c, int upper_bound)
{
    DBG("string='%s', c='%c' and upper_bound='%d'", string, c, upper_bound);
    int cnt = 0;

    while ((string[cnt] != '\0') && (cnt < upper_bound))
    {
	    if (string[cnt] == c) return(cnt);
	    ++cnt;
    }

    /* If reached this point, did not find character */
    return(-1);
} /* End strfind_char */


/* Finds specified substring in main string */
/* Returns a pointer to the substring if it is found */
/* Returns NULL if substring is not found in main string */
char *strfind_substring(char *string, char *substring)
{
    DBG("string='%s' and substring='%s'", string, substring);

    int cnt;
    int num;
    int sublen;
    char *ptr;

    /* Calculate number of searches to be done */
    /* It it the length of the main string - length of substring + 1 */
    sublen = strlen(substring);
    num = strlen(string) - sublen+1;

    /* Do searches in a for loop */
    for (cnt = 0; cnt <num; ++cnt)
    {
	    if (strncmp(string+cnt, substring, sublen) == 0)
	    {
	        ptr = string+cnt;
	        return(ptr);
	    } /* End if */
	} /* End for */

    /* If reached this point, substring was not found */
    return(NULL);
} /* End strfind_substring */


/* Make passed in string specified length */
/* If string not long enough, pad with spaces */
/* If string too long, truncate it */
void str_pad(char *string, int len)
{
    DBG("string='%s' and len='%d'", string, len);

    int cnt;

    for (cnt = 0; cnt <len; ++cnt)
    {
	    if (string[cnt] == 0) 
	    {
	        while (cnt < len) 
	        {
		        string[cnt] = ' ';
		        ++cnt;
	        } /* End while */

	        break;
	    } /* End if */
    } /* End for */

    string[len] = 0;
} /* end str_pad */


/* Pad string with specified number of spaces to the left */
void strlpad(char *string, int len)
{
    int pad;
    int old_len;

    DBG("string='%s' and len='%d'", string, len);

    /* Get length of string */
    old_len = strlen(string);

    if (old_len >= len) return;

    /* Put null at end of new string */
    string[len] = '\0';
    pad = len-old_len;

    for (len = len-1, old_len = old_len-1; len >= pad; --len, --old_len)
        string[len] = string[old_len];

    /* Now put in the white space */
    for (pad = pad-1; pad >= 0; --pad)
        string[pad] = ' ';
} /* End strlpad */


/* Exchanges all occurrences of old character to new character in string */
/* Returns length of string parsed */
int strexc(char *string, char old_char, char new_char)
{
    DBG("string='%s', old_char='%c' and new_char='%c'", string, old_char, new_char);

    int cnt;

    for (cnt = 0; string[cnt] != '\0'; ++cnt)
    {
	    if (string[cnt] == old_char)
	        string[cnt] = new_char;
    } /* End for */

    DBG("will return cnt='%d'", cnt);
    return(cnt);
} /* End strexc */


int strexc2(char *new_string, char *old_string, char old_char, char *new_char)
{
    DBG("new_string='%s', old_string='%s', old_char='%s' and new_char='%s'", new_string, old_string, old_char, new_char);

    int cnt;
    int cnt1;

    for (cnt = 0, cnt1 = 0; old_string[cnt] != '\0'; ++cnt, ++cnt1)
    {
	    if (old_string[cnt] == old_char)
        {
	        new_string[cnt1++] = new_char[0];
	        new_string[cnt1] = new_char[1];
        }
	    else new_string[cnt1] = old_string[cnt];
    } /* End for */

    new_string[cnt1] = '\0';

    return(cnt1);
} /* End strexc2 */


/* Reads in line from buffer and returns number of characters found */
int str_line_get(char *buffer, char *line)
{
    DBG("buffer='%s' and line='%s'", buffer, line);

    int cnt;
    cnt = 0;

    while ((buffer[cnt] != '\n') && (buffer[cnt] != '\0') && (cnt < 260))
    {
	    line[cnt] = buffer[cnt];
        ++cnt;
    } /* end while */

    /* Put null at end of line */
    line[cnt] = '\0';

    /* Return the number of characters found */
    /* Return EOF if end of file reached and no characters found */
    if ((buffer[cnt] == '\0') && (cnt == 0)) return(EOF);
    else return(cnt);
} /* End str_line_get */


/* Copies string1 into string2 */
/* Does not check for the null string */
void str_bcopy(char *string1, char *string2, int len)
{
    DBG("string1='%s', string2='%s' and len='%d'", string1, string2, len);
    int cnt;

    for (cnt =0; cnt <len; ++cnt)
        string2[cnt] = string1[cnt];
}


/* converts string to a long long */
long long str2ll(char *string)
{
    DBG("string='%s'", string);

    long long total = 0;
    int cnt;
    long long multiple = 1;

    for (cnt = strlen(string)-1;  cnt >= 0; --cnt)
    {
	    total += (string[cnt]-48)*multiple;
	    multiple *= 10;
    }

    return(total);
} /* end str2ll */


/* Convert long long size to size string */
void str2size(char *size_string, long long size)
{
    DBG("size_string='%s'", size_string);

    if (size < 1024 * 1024) sprintf (size_string, "%lld bytes", size);
    else if (size < 1024 * 1024 * 1024) sprintf (size_string, "%0.2f MB", 1.0 * size / 1024 / 1024);
	else sprintf (size_string, "%0.4f GB", 1.0 * size / 1024/ 1024/1024);
} /* End str2size */


/* Version of strncpy which puts NULL at end of string */
void strn_cpy(char *result, char *string, int num)
{
    DBG("result='%s', string='%s' and num='%d'", result, string, num);
    register int cnt;

    for (cnt = 0; cnt <num; ++cnt) result[cnt] = string[cnt];

    result[cnt] = '\0';
} /* End strn_cpy */


/* Determine if string is an alphabetical string */
int str_is_alpha(char *string)
{
    DBG("string='%s'", string);
    int cnt;

    for (cnt = 0; string[cnt] != '\0'; ++cnt)
    {
	    if ((string[cnt] < 'A') || (string[cnt] > 'z')) return(-1);
	    if ((string[cnt] > 'Z') && (string[cnt] < 'a')) return(-1);
    }

    return(0);
}


/* Determines if string only contains digits */
/* Returns SUCCESS if all numeric characters */
/* Otherwise returns -1 */
int str_digits(char *string)
{
    DBG("string='%s'", string);
    int cnt;

    for (cnt = 0; string[cnt] != '\0'; ++cnt)
    {
	    if ((string[cnt]> '9') || (string[cnt] < '0')) return(-1);
    }

    /* If reached this point, string is all digits */
    return(0);
} /* end str_digits */


/* Strip off quotes, and brackets from beginning and end */
int str_quotes_strip(char *string)
{
    DBG("string='%s'", string);

    int len = strlen(string);
    char *tmp_string;

    if ((string[0] == 34) || (string[0] == 39) || (string[0] == 40) || (string[0] == 41) || (string[0] == 91) || (string[0] == 93))
    {
	    tmp_string = malloc(len);
	    strcpy1(tmp_string, string+1);
	    strcpy1(string, tmp_string);
	    free(tmp_string);
	    --len;
    }

    /* Now check for trailing characters */
    --len;
    
    if ((string[len] == 34) || (string[len] == 39) || (string[len] == 40) || (string[len] == 41) || (string[len] == 91) || (string[len] == 93))
        string[len--] = '\0';

    return(len);
} /* End str_quotes_strip */


/* Strips leading and trailing quotes on string */
void str_strip_quotes(char *new_string, char *old_string)
{
    DBG("new_string='%s' and old_string='%s'", new_string, old_string);
    int len;

    if ((old_string[0] == '"') || (old_string[0] == '\''))
        len = strcpy1(new_string, old_string+1);
    else len = strcpy1(new_string,old_string);
        --len;

    if ((new_string[len] == '"') || (new_string[len] == '\''))
        new_string[len] = '\0';
} /* End str_strip_quotes */


/* Strips quotes and other characters out of the string */
int str_strip_all_quotes(char *string)
{
    DBG("string='%s'", string);

    char *tmp_string = malloc(strlen(string) + 1);
    int fnd = DBNO;
    int cnt;
    int cnt1;

    for (cnt = 0, cnt1 = 0; string[cnt] != '\0'; ++cnt)
    {
	    if ((string[cnt] != '\'') && (string[cnt] != '\\') && (string[cnt] != '"'))
	        tmp_string[cnt1++] = string[cnt];
	    else 
	        fnd = DBYES;
    } /* end for */

    tmp_string[cnt1] = '\0';
    
    if (fnd == DBYES) strcpy(string, tmp_string);
    
    free(tmp_string);
    
    return(cnt1);
} /* end str_strip_all_quotes */


/* converts months i.e. convert Jan to 01 */
void str_month_convert(char *new_month, char *old_month)
{
    DBG("new_month='%s' and old_month='%s'", new_month, old_month);

    /* Set default value for first position */
    new_month[0] = '0';
    new_month[2] = '\0';

    switch (old_month[0])
    {
	    /* Fill in numeric values for January, June, or July */
	    case 'J':
	    {
	        if (old_month[1] == 'a') new_month[1] = '1';
	        else {
	    	    if (old_month[2] == 'n') new_month[1] = '6';
	    	    else new_month[1] = '7';
	        } /* end if */
	    }
	    break;

	    case 'A':
	    {
	        if (old_month[1] == 'p') new_month[1] = '4';
	        else new_month[1] = '8';
	    }
	    break;

	    /* Fill in numeric value for March and May */
	    case 'M':
	    {
	        if (old_month[2] == 'r') new_month[1] = '3';
	        else new_month[1] = '5';
	    }
	    break;
	    /* Fill in numeric value for February */
	    case 'F':  new_month[1] = '2'; break;

	    /* Fill in numeric value for September */
	    case 'S':  new_month[1] = '9'; break;

	    /* October */
	    case 'O':  
	    {
	        new_month[0] = '1'; 
	        new_month[1] = '0'; 
	    }
	    break;

	    /* November */
	    case 'N': 
	    {
	        new_month[0] = '1';
	        new_month[1] = '1';
	    }
	    break;

	    /* December */
	    case 'D': 
	    {
	        new_month[0] = '1';
	        new_month[1] = '2';
	    }
	    break;
    } /* end switch statement */
} /* End str_month_convert */


/* Count number of occurrences of character in string */
int strcnt_char(char *string, char c)
{
    DBG("string='%s' and c='%c'", string, c);

    int cnt;
    int char_cnt = 0;
    for (cnt = 0; string[cnt] != '\0'; ++cnt)
    {
	    if (string[cnt] == c)
	        ++char_cnt;
    } /* End for */

    return(char_cnt);
} /* end strcnt_char */


/* See if file is a zipped file */
int is_zipped_file(char *file_name)
{
    DBG("file_name='%s'", file_name);

    int len = strlen(file_name);
    
    if (len <3) return(DBNO);

    if ((strcasecmp(file_name+len-3, ".gz") == 0) || (strcasecmp(file_name+len-2, ".Z") == 0) || (strcasecmp(file_name+len-3, ".rz") == 0) || (strcasecmp(file_name+len-3, ".lz") == 0))
    return(DBYES);
    /* Make sure length not less than 4 for next checks */
    else if (len < 4) return(DBNO);
    
    if ((strcasecmp(file_name+len-4, ".zip") == 0) || (strcasecmp(file_name+len-4, ".taz") == 0) || (strcasecmp(file_name+len-4, ".tgz") == 0) || (strcasecmp(file_name+len-4, ".bz2") == 0) || (strcasecmp(file_name+len-4, ".dmp") == 0))
        return(DBYES);
    else return(DBNO);
} /* End is_zipped_file */


/* Reverse the characters in a string, must be NULL terminated */
int str_reverse(char *result, char *string)
{
    DBG("result='%s' and string='%s'", result, string);

    int len;
    int cnt1;
    int cnt2;

    if (( len = strlen(string)) <= 0)
        return(-1);

    for (cnt1 = 0, cnt2 = len-1; cnt2 >= 0; ++cnt1, --cnt2)
        result[cnt1] = string[cnt2];
    
    result[len] = '\0';
    
    return(len);
} /* End str_reverse */


/* Verify name contains valid characters for file name */
int str_is_valid_filename(char *string)
{
    DBG("string='%s'", string);
    int cnt;

    /* do not allow the first character to be a '.' */
    if (string[0] == '.') return(-1);

    /* Check all characters in the string */
    for (cnt = 0; string[cnt] != '\0'; ++cnt)
    {
	    /* Do not allow control characters */
	    /* - is 45, . is 46, and / is 47 */
	    if ((string[cnt] < '-') || (string[cnt] == '/')) return(-1);
	    /* do not allow characters between numbers and capital letter */
	    else if ((string[cnt] > '9') && (string[cnt] <'A')) return(-1);
	    /* do not allow characters between capital and small letters */
	    else if ((string[cnt] > 'Z') && (string[cnt] < 'a') && (string[cnt] != '_'))
	    return(-1);
	    else if (string[cnt] > 'z') return(-1);
    } /* End for */

    /* If reached this point, valid string found */
    return(0);
} /* end str_is_valid_filename */


char *root_find(char *root_ptr, char c)
{
    DBG("root_ptr='%s' and c='%c'", root_ptr, c);

    char *ptr;

	ptr = strrchr(root_ptr, c);

	if (ptr == NULL) ptr = root_ptr;
	else ++ptr;
    
    return(ptr);
} /* End root_find */
