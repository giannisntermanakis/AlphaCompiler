//csd4393,csd4225,csd4226

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "../headerfiles/avm_datatypes.h"
#include "../headerfiles/avm_functions.h"

static void printtables();
static void avm_initstack();

char* typeString[] = {"number","string",
                    "bool","table","userfunc",
                    "libfunc","nil","undefined"};
unsigned top , topsp , GLOBAL_TEMP;
unsigned pc;
unsigned totalActuals , srcline;
unsigned char executionFinished;

avm_memcell ax , bx , cx , retval ;
avm_memcell STACK[AVM_STACK_SIZE];

FILE* avm_warnings , *avm_errors ,*outfile;


int main(){
    avm_warnings = stderr;//fopen("outputs/avm_warnings.out","w");
    avm_errors = stderr; //fopen("outputs/avm_errors.out","w");
    outfile = stdout; //fopen("outputs/avm_output.out","w");   
    // FILE* print_tables = fopen("outputs/avm_tables.out","w");         
    read_abc_file();
    // printtables(print_tables);

    if (topsp = GLOBAL_TEMP +1);
    top = AVM_STACK_SIZE  - total_globals - 1;
    topsp = top ;
    totalActuals = 0;
    executionFinished = 0;
    avm_initstack();
    
    GLOBAL_TEMP = top;

    while (executionFinished == 0)
    {
        execute_cycle();
    }
    
    return 0;
}

static void avm_initstack(){
    unsigned i;
    for(i = 0; i < AVM_STACK_SIZE; ++i){
        AVM_WIPEOUT(STACK[i]);
        STACK[i].type = undef_m;
    }
}

static void printtables(FILE* print_tables){
    int i;
    fprintf(print_tables,"total globals %d\n",total_globals);
    if(numConsts_array != NULL){
        fprintf(print_tables,"nums size %d :\n",numConsts_array_size);
    }
    for(i = 0 ; i < numConsts_array_size ; ++i){
        fprintf(print_tables,"numConsts_array[%d] = %f\n",i , numConsts_array[i]);
    }
    if(stringConsts_array != NULL){
        fprintf(print_tables,"strings size %d :\n",stringConsts_array_size);
    }
    for(i = 0 ; i < stringConsts_array_size ; ++i){
        fprintf(print_tables,"stringConsts_array[%d] = %s\n",i , stringConsts_array[i]);
    }
    if(libFuncs_array != NULL){
        fprintf(print_tables,"Lib funcs size %d :\n",libFuncs_array_size);
    }
    for(i = 0 ; i < libFuncs_array_size ; ++i){
        fprintf(print_tables,"libFuncs_array[%d] = %s\n",i , libFuncs_array[i]);
    }
    if(userFuncs_array != NULL){
        fprintf(print_tables,"User funcs size %d :\n",userFuncs_array_size);
    }
    for(i = 0 ; i < userFuncs_array_size ; ++i){
        fprintf(print_tables,"userFuncs_array[%d].id = %s , args %d\n",i , userFuncs_array[i].id,userFuncs_array[i].totalFormals);
    }
    if(t_code != NULL){
        fprintf(print_tables,"t_code size %d :\n",t_code_size);
    }
    for(i = 0 ; i < t_code_size ; ++i){
        fprintf(print_tables,"t_code[%d] op: %d\n",i , t_code[i].opcode);
    }
    
}

