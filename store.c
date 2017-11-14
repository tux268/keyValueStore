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
	FILE* journalptr;
	int updateJournal;
};

static void updateStore(Store *store){
	char* line = malloc(MAX_KEY_LEN + MAX_VALUE_LEN + 7);

	while (fgets(line, MAX_KEY_LEN + MAX_VALUE_LEN + 7, store->journalptr)) {
		line[strlen(line)-1] = '\0';
		runCommand(store, line);
	}
	free(line);
}


Store* openStore(const char* journal) {
//your code here
	Store* s;
	s = malloc(sizeof(Store));
	s->hashmap = NULL;
	s->journalptr = fopen(journal, "a+");
	s->updateJournal = 0;
	if (s->journalptr == NULL){
		perror("unable to open storeLog");
	}
	updateStore(s);
	s->updateJournal = 1;
	return s;
}

void closeStore( Store *store ) {
	//your code here
	fclose(store->journalptr);
	Record *cur, *tmp;
	HASH_ITER(hh, store->hashmap, cur, tmp){
		HASH_DEL(store->hashmap, cur);
	};
	free(store);
}


void setValue( Store *store, char* key, char* value ) {
	char* logstr = malloc(strlen(key)+strlen(value)+7);
	strcpy(logstr, "set ");
	strcat(logstr, key);
	strcat(logstr, " ");
	strcat(logstr, value);
	strcat(logstr, "\n");
	if (store->updateJournal) fputs(logstr, store->journalptr);
	Record* new = newRecord(key, value);
	HASH_ADD_KEYPTR(hh, store->hashmap, new->key, strlen(new->key), new);
}

void removeValue( Store *store, char *key ) {
	//your code here
	char* logstr = malloc(strlen(key)+6);
	strcpy(logstr, "del ");
	strcat(logstr, key);
	strcat(logstr, "\n");
	if (store->updateJournal) fputs(logstr, store->journalptr);
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
				ret = malloc(strlen("UND ")+strlen(toRun->key)+strlen(" not found in store!")+1);
				strcpy(ret, "UND ");
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
