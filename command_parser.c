#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "command_parser.h"

static char *parseInst(const char *command, Command *result);
static char *parseKey(const char *command, Command *result);
static char *parseValue(const char *command, Command *result);
static char *removeBeginingSpaces(char *p);

int parseCommand( const char *command, Command *result ) {
	//your code here
	//return -1
	//return 0;
	char *p = command;
	p = parseInst(p, result);
	if (!p) return -1;
	p = parseKey(p, result);
	if (!p) return 0;
	p = parseValue(p, result);
	return 0;
}

static char *parseInst(const char *command, Command *result){
	char *p = command;
	p = removeBeginingSpaces(p);
	char *inst = malloc(4);
	strncpy(inst, p, 3);
	p +=3;
	if (!strcmp(inst, "GET")){
		result->instruction = GET;
	}
	else if (!strcmp(inst, "SET")) {
		result->instruction = SET;
	}
	else if (!strcmp(inst, "DEL")) {
		result->instruction = DEL;
	}
	else if (!strcmp(inst, "BYE")) {
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
