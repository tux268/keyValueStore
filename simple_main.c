#include "store.h"
#include "command_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>




int main()
{
	char key1[] = {'k','e','y','1','\0'};
	char key2[] = "key2";
	char key3[] = "key3";
	char key4[] = "key4";
	char value1[] = {'v','a','l','u','e','1','\0'};
	char value2[] = "value2";
	char value3[] = "value3";
	char value4[] = "value4";

	Store* store = openStore();

	setValue(store, key1, value1);
	setValue(store, key2, value2);
	setValue(store, key3, value3);
	setValue(store, key4, value4);
	setValue(store, key4, value3);

	char *answer;
	do {
		char *command = malloc(MAX_KEY_LEN + MAX_VALUE_LEN + 7);
		printf("> ");
		command = fgets(command, MAX_KEY_LEN + MAX_VALUE_LEN + 7, stdin);
		command[strlen(command)-1] = '\0';
		answer = runCommand(store, command);
		printf("%s\n", answer);
	} while(strcmp(answer, "BYE") != 0);

	/*com = "SET sfwe wefwef";
	printf("%s\n", com);
	printf("%s\n", runCommand(store, com));

	com = "GET sfwe";
	printf("%s\n", com);
	printf("%s\n", runCommand(store, com));

	com = "GET sfwee";
	printf("%s\n", com);
	printf("%s\n", runCommand(store, com));

	com = "GET key4";
	printf("%s\n", com);
	printf("%s\n", runCommand(store, com));

	com = "DEL sfwe";
	printf("%s\n", com);
	printf("%s\n", runCommand(store, com));

	com = "DEL sfddwe";
	printf("%s\n", com);
	printf("%s\n", runCommand(store, com));

	com = "BYE";
	printf("%s\n", com);
	printf("%s\n", runCommand(store, com));
	*/
}
