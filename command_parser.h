typedef unsigned short Inst;

#define MAX_KEY_LEN    127
#define MAX_VALUE_LEN  4095

#define GET ((Inst)1)
#define SET ((Inst)2)
#define DEL ((Inst)3)
#define BYE ((Inst)4)

typedef struct Command_t{
//Create your structure here.
	Inst instruction;
	char* key;
	char* value;
	char* error;
} Command;


typedef struct Command_t Command;
int parseCommand( const char* input, Command *result );
