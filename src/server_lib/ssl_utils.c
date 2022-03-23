#ifdef SSL_ENCRYPTION
#include <openssl/ssl.h>
#include "global_defines.h"
#include "global_server_prototypes.h"
#include "global_debug.h" // Mazieri's debug facilities
#include <stdio.h>
#include <stdlib.h>
#include "global_server_errs.h"
#include "socket_includes.h"

int tcp_listen(int db_socket)
{
    int sock;
    struct sockaddr_in sin;
    int val=1;

    DBG();
    
    if ((sock = socket(AF_INET,SOCK_STREAM,0)) < 0)
        shut_down(bad_file_sock, BAD_FILE_SOCK);
    
    memset(&sin, 0, sizeof(sin));

    sin.sin_addr.s_addr=INADDR_ANY;
    sin.sin_family=AF_INET;
    sin.sin_port=htons(db_socket);

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    
    if (bind(sock,(struct sockaddr *)&sin, sizeof(sin)) < 0)
        berr_exit("Unable to bind");
    else 
        PRINTF("Finished bind ready to listen\n");

    listen(sock, 5);  

    return(sock);
}

#endif
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
