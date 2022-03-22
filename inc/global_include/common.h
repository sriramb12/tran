#ifndef _common_h
#define _common_h

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#ifndef SOLARIS10
#include <signal.h>
#endif
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include <openssl/ssl.h>

#define CA_LIST "ssl.pem"
#ifdef TRANSCEND
#define KEYFILE "trans.ssl.pem"
#endif
#ifdef INTERCOM
#define KEYFILE "icom.ssl.pem"
#endif
#define RANDOM  "random.pem"
#define PASSWORD "password"
#define BUFSIZZ 1024

extern BIO *bio_err;
int berr_exit (char *string);
int err_exit(char *string);

SSL_CTX *initialize_ctx(char *keyfile, char *password);
void destroy_ctx(SSL_CTX *ctx);

#ifndef ALLOW_OLD_VERSIONS
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
#error "Must use OpenSSL 0.9.6 or later"
#endif
#endif

#endif
