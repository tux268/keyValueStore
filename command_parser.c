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
				result->error = "GET : missing key";
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
				result->error = "SET : missing key";
				return -1;
			}
			p = parseValue(p, result);
			if (!p){
				result->error = "SET : missing value";
				return -1;
			}
			break;
		case 3:
			p = parseKey(p, result);
			if (!p){
				result->error = "DEL : missing key";
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
	p = removeBeginingSpaces(p);
	int len = 0;
	while (*(p+len) != ' ' && *(p+len) != '\0') {
		len++;
	}
	char *inst = malloc(len+1);
	strncpy(inst, p, len);
	p +=len;
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
