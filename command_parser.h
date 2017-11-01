typedef unsigned char Inst;

#define GET ((Inst)1)
#define SET ((Inst)2)
#define DEL ((Inst)3)
#define BYE ((Inst)4)

typedef struct Command_t{
//Create your structure here.
	Inst instruction;
	char* key;
	char* value;
} Command;


typedef struct Command_t Command;
int parseCommand( const char* input, Command *result );
