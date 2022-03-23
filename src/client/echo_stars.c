/****************************************************************************
 *
 * Function:	echo_stars
 *
 * Description:	Routines to echo stars to the screen when customer enters his password.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "trans_prototypes.h"

#include "global_debug.h"


#ifndef WINDOWS_NT4
int no_echo(char *line)
{
    DBG();
    int cnt = 0;

    /* Turn keyboard echo off */
    system("stty -echo");
    /* Flush previous input */
    /* Get the carriage return from previous scanf */
    getchar();
	line[0] = getchar();
    while (line[cnt] != '\n')
    {
	++cnt;
	line[cnt] = getchar();
    }
    line[cnt] = '\0';
    /* Turn keyboard echo on */
    system("stty echo");
    return(cnt);
} /* end no_echo */

int echo__stars(char *line)
{
    DBG();
    int cnt = 0;

    /* Turn keyboard echo off */
    system("stty -echo");
    /* Flush previous input */
	line[0] = getchar();
    while (line[cnt] != '\n')
    {
	++cnt;
	line[cnt] = getchar();
	fflush(stdout);
    }
    line[cnt] = '\0';
    /* Turn keyboard echo on */
    system("stty echo");
    return(cnt);
} /* end echo__stars */

/* Turns off echo for unix */
int echo_stars(char *line, int parse_cr)
{
    DBG();
    int cnt = 0;

    /* Turn keyboard echo off */
    system("stty -echo");
    /* Flush previous input */
    /* Get the carriage return from previous scanf */
    if(parse_cr == DBYES)
    getchar();
    else fflush(stdout);
	line[0] = getchar();
    putchar('*');
    while (line[cnt] != '\n')
    {
	++cnt;
	line[cnt] = getchar();
	putchar('*');
	fflush(stdout);
    }
    line[cnt] = '\0';
    /* Turn keyboard echo on */
    system("stty echo");
    return(cnt);
} /* End echo_stars */


#else

/* Following code used for turning off echo on Windows */
int no_echo(char *buffer)
{
    DBG();
    static int stdin_fd = 0;
  struct termios config;
  int c;
    int cnt = 0;

  tcgetattr(stdin_fd, &config);
  config.c_lflag &= ~ECHO;
  tcsetattr(stdin_fd, TCSANOW, &config);

  fflush(stdout);

/*     getchar(); */
    for(cnt = 0;cnt < BUFFER_SIZE && (c = (int)read(stdin_fd, &buffer[cnt], (size_t)1)) != -1; cnt++) 
    {
    fflush(stdout);
    if((buffer[cnt] == '\n') || (buffer[cnt] == '\r')) 
      break;
    } /* End for */
  buffer[cnt] = '\0';
  fflush(stdout); 
  tcflush(stdin_fd, TCIOFLUSH);
  config.c_lflag = 35387;
  tcsetattr(stdin_fd, TCSANOW, &config);
    getchar();
    putchar('\n');
  return (cnt);
} /* End echo_stars */
int echo_stars(char *buffer, int parse_cr)
{
    DBG();
    static int stdin_fd = 0;
  struct termios config;
  int c;
    int cnt = 0;

  tcgetattr(stdin_fd, &config);
  config.c_lflag &= ~ECHO;
  tcsetattr(stdin_fd, TCSANOW, &config);
    /* Flush previous input */
    fflush(stdout);
    for(cnt = 0;cnt < BUFFER_SIZE && (c = (int)read(stdin_fd, &buffer[cnt], (size_t)1)) != -1; cnt++) 
    {
    putchar('*');
    fflush(stdout);
    if((buffer[cnt] == '\n') || (buffer[cnt] == '\r')) 
      break;
    } /* End for */
  buffer[cnt] = '\0';
  fflush(stdout); 
  tcflush(stdin_fd, TCIOFLUSH);
  config.c_lflag = 35387;
  tcsetattr(stdin_fd, TCSANOW, &config);
    getchar();
    putchar('\n');
  return (cnt);
} /* End echo_stars */

int echo__stars(char *buffer)
{
    DBG();
    static int stdin_fd = 0;
  struct termios config;
  int c;
    int cnt = 0;

  tcgetattr(stdin_fd, &config);
  config.c_lflag &= ~ECHO;
  tcsetattr(stdin_fd, TCSANOW, &config);

  fflush(stdout);

/*     getchar(); */
    for(cnt = 0;cnt < BUFFER_SIZE && (c = (int)read(stdin_fd, &buffer[cnt], (size_t)1)) != -1; cnt++) 
    {
    putchar('*');
    fflush(stdout);
    if((buffer[cnt] == '\n') || (buffer[cnt] == '\r')) 
      break;
    } /* End for */
  buffer[cnt] = '\0';
  fflush(stdout); 
  tcflush(stdin_fd, TCIOFLUSH);
  config.c_lflag = 35387;
  tcsetattr(stdin_fd, TCSANOW, &config);
    getchar();
    putchar('\n');
  return (cnt);
} /* End echo__stars */
#endif /* For turning off echo on Windows */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
