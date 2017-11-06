#ifndef STORE_H
#define STORE_H

#define MAX_KEY_LEN    127
#define MAX_VALUE_LEN  4095

struct Store_t;

typedef struct Store_t Store;

Store* openStore();

void closeStore( Store *store );

void setValue( Store *store, char* key, char* value );

void removeValue( Store *store, char *key );

/* The caller does not own the result */
char *getValue( Store *store, char *key );

unsigned int storeSize( Store *store );

char *runCommand( Store *store, char* command );

#endif
