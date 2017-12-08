#include "store.h"
#include "command_parser.h"
#include "server.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



int main( int argc, char **argv ) {
  
}

/*
int main()
{

	Store* store = openStore("log.txt");

	char *answer;
	do {
		char *command = malloc(MAX_KEY_LEN + MAX_VALUE_LEN + 7);
		printf("> ");
		command = fgets(command, MAX_KEY_LEN + MAX_VALUE_LEN + 7, stdin);
		command[strlen(command)-1] = '\0';
		answer = runCommand(store, command);
		printf("%s\n", answer);
	} while(strcmp(answer, "BYE") != 0);

}
*/
