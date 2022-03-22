#ifndef socket_includes_h
#define socket_includes_h
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifndef SOLARIS10
#include <netdb.h>
#endif
#include <signal.h>
#include <stdio.h>
#include <sys/fcntl.h>

#endif /* ifdef for socket_includes_h */
