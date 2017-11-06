#include "uthash.h"
#include "store.h"
#include "command_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


struct Record_t {
//define members here.
	const char *key;
	const char *value;

	UT_hash_handle hh;
};

typedef struct Record_t Record;


static Record *newRecord( const char* key, const char* value) {
//your code here

	Record* rec;
	rec = malloc(sizeof(Record));
	rec->key = key;
	rec->value = value;
	return rec;
}

static void deleteRecord( Record *record ) {
	//your code here
	free(record);
	record = NULL;
}

//gift from assistant
struct Store_t {
  Record *hashmap;
};


Store* openStore() {
//your code here
	Store* s;
	s = malloc(sizeof(Store));
	s->hashmap = NULL;
	return s;
}

void closeStore( Store *store ) {
	//your code here
	Record *cur, *tmp;
	HASH_ITER(hh, store->hashmap, cur, tmp){
		HASH_DEL(store->hashmap, cur);
	};
	free(store);
}


void setValue( Store *store, char* key, char* value ) {
	Record* new = newRecord(key, value);
	HASH_ADD_KEYPTR(hh, store->hashmap, new->key, strlen(new->key), new);
}

void removeValue( Store *store, char *key ) {
	//your code here
	Record *rec = NULL;

	HASH_FIND_STR(store->hashmap, key, rec);
	//if (rec) printf("remove entry %s from hashmap\n", key);
	HASH_DEL(store->hashmap, rec);
	deleteRecord(rec);
}

char* getValue( Store *store, char *key ) {
	//your code here
	Record *rec = NULL;
	HASH_FIND_STR(store->hashmap, key, rec);
	if (rec) return rec->value;
	return NULL;
}

unsigned int storeSize( Store *store ) {
 //your code here
 return HASH_COUNT(store->hashmap);
}

char *runCommand( Store *store, char* command ) {
	//your code here
	Command* toRun = malloc(sizeof(Command));
	char *ret, *value;
	if (parseCommand(command, toRun) == -1){
		ret = malloc(strlen("ERR invalide command\n") + strlen(toRun->error)+1);
		strcpy(ret, "ERR invalide command\n");
		strcat(ret, toRun->error);
		return ret;
	}

	switch (toRun->instruction) {
		case 1:
			value = getValue(store, toRun->key);
			if (!value){
				ret = malloc(strlen("ERR ")+strlen(toRun->key)+strlen(" not found in store!")+1);
				strcpy(ret, "ERR ");
				strcat(ret, toRun->key);
				strcat(ret, " not found in store!");
			}
			else{
				ret = malloc(strlen(value)+5);
				strcpy(ret, "VAL ");
				strcat(ret, value);
			}
			return ret;
			break;
		case 2:
			setValue(store, toRun->key, toRun->value);
			return "DON";
			break;
		case 3:
		  value = getValue(store, toRun->key);
			if (!value){
				ret = malloc(strlen("ERR ")+strlen(toRun->key)+strlen(" not found in store!")+1);
				strcpy(ret, "ERR ");
				strcat(ret, toRun->key);
				strcat(ret, " not found in store!");
				return ret;
			}
			removeValue(store, toRun->key);
			return "DON";
			break;
		case 4:
			return "BYE";
			break;
		default:
			return "ERR Invalide commande !";
			break;
	}
  return NULL;
}
