#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "command_parser.h"

static char *parseInst(const char *command, Command *result);
static char *parseKey(const char *command, Command *result);
static char *parseValue(const char *command, Command *result);
static char *removeBeginingSpaces(char *p);
static void lower(const char *str);

int parseCommand( const char *command, Command *result ) {
	//your code here
	//return -1
	//return 0;
	printf("CMD IN PARSE : --%s--", command);
	result->instruction = 0;
	result->key = "";
	result->value = "";
	result->error = NULL;
	char *p = command;
	p = parseInst(p, result);
	if (!p){
		result->error = "Unknown instruction!";
		return -1;
	}
	switch (result->instruction){
		case 1:
			p = parseKey(p, result);
			if (!p){
				if(!result->error) result->error = "GET : missing key";
				return -1;
			}
			if(parseValue(p,result)){
				result->error = "GET : too many arguments";
				return -1;
			}
			break;
		case 2:
			p = parseKey(p, result);
			if (!p){
				if(!result->error) result->error = "SET : missing key";
				return -1;
			}
			p = parseValue(p, result);
			if (!p){
				if(!result->error) result->error = "SET : missing value";
				return -1;
			}
			break;
		case 3:
			p = parseKey(p, result);
			if (!p){
				if(!result->error) result->error = "DEL : missing key";
				return -1;
			}
			if(parseValue(p,result)){
				result->error = "DEL : too many arguments";
				return -1;
			}
			break;
		case 4:
		if(parseValue(p,result)){
			result->error = "BYE : too many arguments";
			return -1;
		}
			break;
		default:
			result->error = "Unknown error";
			return -1;
			break;
	}
	return 0;
}

static char *parseInst(const char *command, Command *result){
	char *p = command;
	printf("Cmd in patseInst --%s--", command);
	p = removeBeginingSpaces(p);
	printf("Cmd in parseInst p --%s--", p);

	int len = 0;
	while (*(p+len) != ' ' && *(p+len) != '\0') {
		len++;
	}
	printf("len : %d\n", len);
	char *inst = malloc(len+1);
	strncpy(inst, p, len);
	inst[3] = '\0';
	lower(inst);
	printf("INST : --%s--", inst);
	p +=len;
	//if (strlen(inst)>3)  return NULL;

	//strncpy(inst, inst, 3);
	/*printf("\n******\n");
	for (size_t i = 0; i < strlen(inst); i++) {
		printf("%x, ", inst[i]);
	}*/

	if (!strcmp(inst, "get")){
		result->instruction = GET;
	}
	else if (!strcmp(inst, "set")) {
		result->instruction = SET;
	}
	else if (!strcmp(inst, "del")) {
		result->instruction = DEL;
	}
	else if (!strcmp(inst, "bye")) {
		result->instruction = BYE;
	}
	else {

		return NULL;
	}
	return p;
}

static char *parseKey(const char *command, Command *result){
	char *p = command;
	p = removeBeginingSpaces(p);

	if (*p == '\0') {
		return NULL;
	}
	int len = 0;
	while (*(p+len) != ' ' && *(p+len) != '\0') {
		len++;
	}
	if(len > MAX_KEY_LEN){
		result->error = "KEY too big, max 127 characters";
		return NULL;
	}
	result->key = malloc(len+1);
	strncpy(result->key, p, len);
	p += len;
	return p;
}

static char *parseValue(const char *command, Command *result){
	char *p = command;
	p = removeBeginingSpaces(p);
	if (*p == '\0') {
		return NULL;
	}
	if(strlen(p) > MAX_VALUE_LEN){
		result->error = "VALUE too big, max 4095 characters";
		return NULL;
	}
	result->value = malloc(strlen(p)+1);
	strncpy(result->value, p, strlen(p));
	return p + strlen(p);
}

static char *removeBeginingSpaces(char *p){
	while (*p == ' ') {
		++p;
	}
	return p;
}

static void lower(const char *str){
	for (char *p = str; *p; ++p) *p = tolower(*p);
}
