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

	printf("value of key1 : %s\n", getValue(store, key1));
	printf("value of key2 : %s\n", getValue(store, key2));
	printf("value of key3 : %s\n", getValue(store, "key3"));
	printf("value of key4 : %s\n", getValue(store, "key4"));

	printf("size of store : %i\n", storeSize(store));
	removeValue(store, key4);
	printf("size of store : %i\n", storeSize(store));
	closeStore(store);
	printf("close store\n");
	printf("size of store : %i\n", storeSize(store));
	printf("\n");

	char* com = "  GET bahfdowhe iwef d ioef w";
	Command* command;
	command = malloc(sizeof(Command));

	printf("%s\n", com);
	printf("%d\n", parseCommand(com, command));
	printf("%d\n", command->instruction);
	printf("%s\n", command->key);
	printf("%s\n", command->value);
}
