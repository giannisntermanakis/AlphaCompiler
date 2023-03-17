#include "../headerfiles/avm_datatypes.h"

// toStrings
char* number_tostring( avm_memcell* m);
char* string_tostring( avm_memcell* m);
char* bool_tostring( avm_memcell* m);
char* table_tostring( avm_memcell* m);
char* userfunc_tostring( avm_memcell* m);
char* libfunc_tostring( avm_memcell* m);
char* nil_tostring( avm_memcell* m);
char* undef_tostring( avm_memcell* m);
char* avm_tostring( avm_memcell* m);

//toBool
unsigned char avm_tobool(avm_memcell *m);

//environment
void avm_memcellclear( avm_memcell* m);


//call
void avm_callsaveenviroment();
void avm_calllibfunc(char* id);

//avms
void avm_push_envvalue(unsigned int val);
void avm_assign( avm_memcell* lv,  avm_memcell* rv);
void avm_dec_top();
avm_memcell* avm_translate_operand( vmarg* arg,  avm_memcell* reg);
unsigned avm_totalactuals(void);


//execution

void execute_arithmetic( instruction* instr);
void execute_assign( instruction* instr);
void execute_call( instruction* instr);
void execute_funcexit( instruction* instr);
void execute_funcenter( instruction* instr);
void execute_pusharg( instruction* instr);
void execute_jeq( instruction* instr);
void execute_jne( instruction* instr);
void execute_jle( instruction* instr);
void execute_jge( instruction* instr);
void execute_jlt( instruction* instr);
void execute_jgt( instruction* instr);
void execute_tablesetelem( instruction* instr);
void execute_jump( instruction* instr);
void execute_newtable(instruction* instr);
void execute_tablegetelem(instruction* instr);
void execute_nop(instruction* instr);
void execute_cycle();

//read file

void read_abc_file(void);

// tables

void avm_tablesetelem( avm_table* table,  avm_memcell *index,  avm_memcell *content);
void avm_tablebucketsdestroy( avm_table_bucket** p);
void avm_tabledestroy( avm_table* t);
void avm_tableincrefcounter( avm_table* t);
void avm_tabledecrefcounter( avm_table* t);
void avm_tablebucketsinit(avm_table_bucket ** p);
avm_table* avm_tablenew(void);
avm_memcell* avm_tablegetelem(avm_table* table , avm_memcell* index);


//get 
unsigned int avm_get_envvalue(unsigned int i);
userfunc* avm_getfuncinfo(unsigned int address);
double consts_getnumber(unsigned int index);
char* consts_getstring(unsigned int index);
char* libfuncs_getused(unsigned int index);
avm_memcell* avm_getactual(unsigned i);
library_func_t avm_get_libraryfunc(char* id);


//libraries

void libfunc_print (void);
void libfunc_typeof (void);
void libfunc_sqrt (void);
void libfunc_cos (void);
void libfunc_sin (void);
void libfunc_input(void);
void libfunc_totalarguments (void);
void libfunc_objecttotalmembers(void);
void libfunc_argument(void);

void    libfunc_objectmemberkeys(void);
void    libfunc_strtonum(void);
void    libfunc_objectcopy(void);
void    libfunc_concat(void);


