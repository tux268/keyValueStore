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

/* A connection was accepted, waiting for the cmd */
#define READING 0
/* Received the cmd, must send the answer */
#define WRITING 1
/* Sending the result (WRITING should be true) */
#define RESULT  2
/* Sending the result (WRITING should be true) */
#define ERROR   4
/* The connection can be closed */
#define DONE    8

typedef struct 
{
  char buffer[BUF_LEN];
  int end;
  int cur;
  int status;
} FDState;

FDState *states[FD_SETSIZE]; 
int listener; 
Store *store;

static void die(char *issue) 
{
  perror(issue);
  exit(EXIT_FAILURE);
}

void resetState( FDState *state ) 
{
	memset(state->buffer, 0, BUF_LEN);
	state->end = 0;
	state->cur = 0;
	state->status = READING;
}

FDState *allocFDState() 
{
	FDState *s = malloc(sizeof(FDState));
	if (!s)
	{
		return NULL;
	}

	resetState(s);

  	return s;
}

void freeFDState(FDState *state) 
{
	free(state);
}

void setNonBlocking(int fd) 
{
  int flags = fcntl (fd, F_GETFL);

  // If reading the flags fails return error (EAGAIN == EWOULDBLOCK)
  if (flags == EAGAIN) 
  {
    return -1;
  }
  // Set O_NONBLOCK Flag
  flags |= O_NONBLOCK;

  // Write the new flag (EAGAIN == EWOULDBLOCK)
  if (fcntl (fd, F_SETFL, flags) == EAGAIN) 
  {
    return -1;
  }
 
}

int receiveCommand(int fd, FDState *state)
{
	int nRead;
	nRead = read(fd, state->buffer, BUF_LEN);

	if(nRead <= 0) 
	{
		if(errno == EAGAIN)
		{
			return 0;
		}
		else
		{
			perror("[-] Failed to read cmd !");
			return -1;
		}
	}
	else
	{
		state->status = WRITING;
		state->end = nRead - 1;
		return 0;
	}
}

int sendResult( int fd, FDState *state, Store *store )
{
	int nWrite;

	// +1 for the null byte
	char* cmd = malloc (state->end + 1);
	strcpy(cmd, state->buffer);
	cmd[strlen(cmd)-1] = '\0';

	char* res = runCommand(store, cmd);

	nWrite = write(fd, res, strlen(res));
	if (nWrite != strlen(res))
	{
		if (errno == EAGAIN)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else 
	{
		resetState(state);
	}
}


void run( int port )
{
	int i, maxfd;
	struct sockaddr_in socketAddress;
	fd_set readset, writeset;
	Store* store = openStore("log.txt");

	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.s_addr = INADDR_ANY;
	socketAddress.sin_port = htons(port);

	// Clean sets to null
	for (i = 0; i < FD_SETSIZE; i++)
	{
		states[i] = NULL;
	}


	/* ---- INIT + NON-BLOCK ---- */

	listener = socket(AF_INET, SOCK_STREAM, 0);
	setNonBlocking(listener);


	/* ---- BINDING ---- */

	// Handles already bind address case
	int y = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y)) == -1) 
    {
        die("[-] setsockopt error ! ...");
	}

	if (bind(listener, (struct sockaddr*) &socketAddress, sizeof(socketAddress)) < 0){
		die("[-] Bind error ! ...");
	}


	/* ---- LISTENING ---- */

	// MAX 16 clients
	if (listen(listener, 16) < 0){
		die("[-] Listening error ! ...");
	}

	printf("[+] Listening for connections on port %d ...\n", port);
	
	while (1) 
	{

		maxfd = listener;
		FD_ZERO(&readset);
		FD_ZERO(&writeset);
		FD_SET(listener, &readset);

		// FD_SETS iteration
		for (i=0; i < FD_SETSIZE; ++i) 
		{
			if (states[i]) 
			{
				if (i > maxfd) 
				{	
					maxfd = i;
					FD_SET(i, &readset);
					if (FD_ISSET(i, &readset)) {
						printf("[+] File descriptor %d saved in readset\n", i);
					}
					if(states[i]->status == WRITING) 
					{	
						printf("[+] File descriptor %d saved in writeset\n", i);
						FD_SET(i, &writeset);
					}
				}
      		}
    	}
		
		if(select(maxfd+1, &readset, &writeset, NULL, NULL) < 0) {
			die("[-] Select error ! ...]");
		}


		/* ---- CONNECTIONS HANDLING ----*/

		if (FD_ISSET(listener, &readset)) {
			int fd;
			struct sockaddr_storage ss;
			socklen_t slen = sizeof(ss);
			fd = accept(listener,(struct sockaddr*) &ss, &slen);
			if (fd < 0)
			{
				die("[-] Accept error ! ...]");
			}
			else if (fd > FD_SETSIZE)
			{
				close(fd);
			}
			else
			{
				printf("[+] Accepted connection with file descriptor %d\n", fd);
				setNonBlocking(fd);
				states[fd] = allocFDState();
			}
		}


		/* ---- READ/WRITE OPS ---- */

		for(i=0; i <= maxfd; i++)
		{
			int r = 0;
      		if (i == listener) continue;
      		if (FD_ISSET(i, &readset))   
			{	
				printf("[+] Reading from file descriptor %d ...\n", i);
				r = receiveCommand(i, states[i]);
				printf("[+] Received CMD : %s\n", states[i]->buffer, i);
      		}

			if (r == 0 && FD_ISSET(i, &writeset)) 
			{
				printf( "[+] Writing to %d ... \n" , i);
				r = sendResult(i, states[i], store);

				if (states[i]->status == DONE) 
				{
					printf( "[+] Closing socket: %d\n", i);
					freeFDState(states[i]);
					states[i] = NULL;
					close(i);
				}
      		}
		}
	}
}

// -------- TO DO ! -------
void shutdownServer(int signum)
{
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
  int port = 9998;
  if(argc == 2)
  {
	  port = atoi(argv[1]);
  }

	run(port);
}
