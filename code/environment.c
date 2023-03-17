//csd4393,csd4225,csd4226

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../headerfiles/avm_datatypes.h"
#include "../headerfiles/avm_functions.h"

// ok
void avm_memcellclear(avm_memcell* m){
    //instead of dispatch table
    switch (m->type)
    {
    case table_m:
        assert(m->data.tableVal);
        avm_tabledecrefcounter(m->data.tableVal); 
        m->type = undef_m;
        break;
    case string_m:
        assert(m->data.strVal);
        free(m->data.strVal);
        m->type = undef_m;
    default:
        break;
    }
}
// ok
void avm_dec_top(){
    if(!top){
        fprintf(avm_errors,"Stack Overflow!\n");
        executionFinished = 1;
    }else{
        --top;
    }
}
// ok
void avm_push_envvalue(unsigned val){
    STACK[top].type = number_m;
    STACK[top].data.numVal = val;
    avm_dec_top();
}
// ok
void avm_callsaveenviroment(){
    avm_push_envvalue(totalActuals);
    //retaddr = next instr
    avm_push_envvalue(pc+1);
    avm_push_envvalue(top + totalActuals + 2);
    avm_push_envvalue(topsp);
}
