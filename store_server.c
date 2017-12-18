#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/select.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

//#include "common.h"
#include "store.h"

#define BUF_LEN ( 6 + MAX_KEY_LEN + MAX_VALUE_LEN + 1 )
#define BLANK(buf,len) memset( (buf), 0, (len) )

/* A connection was accepted, waiting for the command */
#define READING 0
/* Received the command, must send the answer */
#define WRITING 1
/* Sending the result (WRITING should be true) */
#define RESULT  2
/* Sending the result (WRITING should be true) */
#define ERROR   4
/* The connection can be closed */
#define DONE    8


//===========================//
//file descriptor state management.
//remember, INDEX of the FDState
//container is the magic word.
typedef struct {
  char buffer[BUF_LEN];
  int end;
  int cur;
  int status;
} FDState;

static void die(char *issue) {
  perror(issue);
  exit(EXIT_FAILURE);
}

void resetState( FDState *state ) {
//initialize state.
//default status is READING
memset(state->buffer, 0, BUF_LEN);
state->end = 0;
state->cur = 0;
state->status = READING;
}

FDState *allocFDState() {
   //allocate wrapper
	FDState *state = malloc(sizeof(FDState));
	if (!state){
		return NULL;
	}

	resetState(state);
  return state;
}

void freeFDState(FDState *state) {
  //clean up wrapper
	free(state);
}

FDState *states[FD_SETSIZE]; //state container as described in TP7 giveaway
//===========================//




int listener; //the main listener file descriptor for the server socket - non blocking property for all connections derived from this one after you set it.
Store *store;

void setNonBlocking(int fd){
  //wrap around fcntl here
  //1) get the file descriptor flags
  //2) update the file descriptor to support non-blocking access - refer to manual for return values (EAGAIN EWOULDBLOCK)
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

int receiveCommand(int fd, FDState *state)
{
	//read from socket here
	//return 0 for success
	//return -1 for failure
	int nread;
	nread = read(fd, state->buffer, BUF_LEN);
	/*if(nread  0){
		die("wow something went wrong...");
	}*/
	if(nread <= 0){
		if(errno == EAGAIN){
			return 0;
		}
		else{
			perror("Read error");
			return -1;
		}
	}
	else{
		state->status = WRITING;
		state->end = nread; // wtf ??
		return 0;
	}
}

int sendResult( int fd, FDState *state, Store *store )
{
	//write to socket here
	// return 0 for success
	//return -1 for failure
	int nwrite;
	char * command = malloc (state->end);
	strncpy(command, state->buffer, state->end);
	char * rep = runCommand(store, state->buffer);
	//state->status = RESULT;
	nwrite = write(fd, rep, sizeof(rep));
	if (nwrite != sizeof(rep)){
		if (errno == EAGAIN){
			return 0;
		}
		else{
			//state->status = ERROR;
			return -1;
		}
	}
	else{
		state->status = DONE;
	}
}


void run( int port )
{
	//here is where the magic happens.
	//handle ALL ERRORS
	int listener;
	int i, maxfd;
	struct sockaddr_in sin;
	fd_set readset, writeset;
	Store* store = openStore("log.txt");

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);

	// init all states to null
	for (i = 0; i < FD_SETSIZE; i++){
		states[i] = NULL;
	}

	// make listener socket, set it non blocking and bind it
	listener = socket(AF_INET, SOCK_STREAM, 0);
	setNonBlocking(listener);
	if (bind(listener, (struct sockaddr*) &sin, sizeof(sin))<0){
		die("Bind error");
	}
	// begin to listen for incomming sockets, set limit to 16
	if (listen(listener, 16)<0){
		die("Listen error");
	}

	printf("Listening for connections\n");

	while (1) {
		maxfd = listener;
		FD_ZERO(&readset);
		FD_ZERO(&writeset);

		FD_SET(listener, &readset);

		// update FD sets
		for (i=0; i < FD_SETSIZE; ++i) {
      if (states[i]) {
        if (i > maxfd)
	   			maxfd = i;
	 			FD_SET(i, &readset);
	 			if( states[i]->status == WRITING) {
	   			FD_SET(i, &writeset);
	 			}
      }
    }

		// performing the select operation
		if(select(maxfd+1, &readset, &writeset, NULL, NULL)<0) {
			die("Select error");
		}

		// Accept new connections
		if (FD_ISSET(listener, &readset)){
			int fd;
      struct sockaddr_storage ss;
      socklen_t slen = sizeof(ss);
			fd = accept(listener, (struct sockaddr*)&ss, &slen);

			if (fd < 0){
				die("Accept error");
			}
			else if (fd > FD_SETSIZE){
				close(fd);
			}
			else{
				printf("Accepted connection %d\n", fd);
				setNonBlocking(fd);
				states[fd] = allocFDState;
			}
		}

		// do things ...
		for(i=0; i<maxfd; i++){
			int r = 0;
      if (i == listener) continue;
      if (FD_ISSET(i, &readset)) {
				printf( "Reading from %d\n" , i );
				r = receiveCommand(i, states[i]);
      }

			if (r == 0 && FD_ISSET(i, &writeset)) {
				printf( "Writing to %d\n" , i );
				r = sendResult(i, states[i], store);

				if ( states[i]->status == DONE ) {
		  		printf( "Closing socket: %d\n", i );
		  		freeFDState(states[i]);
		  		states[i] = NULL;
		  		close(i);
				}
      }
		}
	}
}

//																	-------- TO DO ! -------
void shutdownServer( int signum )
{
	//signal callback
	//do your clean-up here.

	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
  int port = 9998;
  if( argc == 2 )
  {
	  port = atoi( argv[1] );
  }

	run(port);
  //1) prepare sigaction and callback
  //2) open the store
  //3) run the server
}
