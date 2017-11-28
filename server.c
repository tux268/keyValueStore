#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "store.h"
#include "server.h"


static void die(char *issue) {
  perror(issue);
  exit(EXIT_FAILURE);
}

void prepare_address( struct sockaddr_in *address,  int port ) {
  size_t addrSize = sizeof( address );
  memset(address, 0, addrSize);
  address->sin_family = AF_INET;
  address->sin_addr.s_addr = htonl(INADDR_ANY);
  address->sin_port = htons(port);
}

int makeSocket( int port ) {
  struct sockaddr_in address;
  int sock = socket(PF_INET, SOCK_STREAM, 0);
  if( sock < 0 ) {
    die("Failed to create socket");
  }
  prepare_address( &address, port );
  if( bind( sock,
	    (struct sockaddr *) &address,
	    sizeof(address)
	    ) < 0  )
    {
      die("Failed to bind the server socket");
    }
  if (listen(sock, MAX_PENDING) < 0) {
    die("Failed to listen on server socket");
  }
  return sock;
}

int handleClient( int clientSock, Store* store) {
	char *command = NULL;
	command =  malloc(MAX_KEY_LEN+MAX_VALUE_LEN+7);
	char *rep;
	int nRead, nWrite;
	nRead = read( clientSock, command, MAX_KEY_LEN+MAX_VALUE_LEN+7);
	if (nRead <= 0) {
		die("fucked R !!!!");
	}
	command[strlen(command)-1] = '\0';
	/*for (size_t i = 0; i < strlen(command); i++) {
		printf("%x, ", command[i]);
	}*/
	rep = runCommand(store, command);
	nWrite = write( clientSock, rep, strlen(rep)+1);
	if (strcmp(rep, "BYE") == 0){
		close(clientSock);
		return 1;
	}
	if (nWrite <= 0) {
		die("fucked W !!!!");
	}
  close(clientSock);
	return 0;
}

void run( int serverSock) {
	Store* store = openStore("log.txt");
  while( 1 ) {
    struct sockaddr_in clientAddress;
    unsigned int clientLength = sizeof(clientAddress);
    int clientSock;
    printf( "Waiting for incoming connections\n");
    clientSock = accept(serverSock, (struct sockaddr *) &clientAddress, &clientLength );
    if( clientSock < 0 ) {
      die("Failed to accept client connection");
    }
    printf( "Client connected: %s\n", inet_ntoa(clientAddress.sin_addr));
		char* rep = "Connected bitch\n";
		write(clientSock, rep, strlen(rep)+1);
    if(handleClient(clientSock, store) == 1){
			closeStore(store);
			store = openStore("log.txt");
		}
  }
}
