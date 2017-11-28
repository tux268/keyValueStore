#ifndef SERVER_H
#define SERVER_H

#include "store.h"

#define MAX_PENDING 256

void prepare_address( struct sockaddr_in *address,  int port );
int makeSocket( int port );
int handleClient( int clientSock, Store* store);
void run( int serverSock);

#endif
