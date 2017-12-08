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

void resetState( FDState *state ) {
//initialize state.
//default status is READING
}

FDState *allocFDState() {
   //allocate wrapper
  return state;
}

void freeFDState(FDState *state) {
  //clean up wrapper
}

FDState *states[FD_SETSIZE]; //state container as described in TP7 giveaway
//===========================//




int listener; //the main listener file descriptor for the server socket - non blocking property for all connections derived from this one after you set it.
Store *store;

void setNonBlocking(int fd){
  //wrap around fnctl here
  //1) get the file descriptor flags
  //2) update the file descriptor to support non-blocking access - refer to manual for return values (EAGAIN EWOULDBLOCK)
}

int receiveCommand(int fd, FDState *state)
{
	//read from socket here
	// return 0 for success
	//return -1 for failure
}

int sendResult( int fd, FDState *state, Store *store )
{
	//write to socket here
	// return 0 for success
	//return -1 for failure
}


void run( int port )
{
//here is where the magic happens.
//handle ALL ERRORS
}

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
  //1) prepare sigaction and callback
  //2) open the store
  //3) run the server
}

