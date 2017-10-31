#include "uthash.h"
#include "store.h"


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
	if (rec) printf("remove key : %s from hashmap\n", key);
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
