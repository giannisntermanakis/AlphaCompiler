//csd4393,csd4225,csd4226
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../headerfiles/avm_datatypes.h"
#include "../headerfiles/avm_functions.h"


// lib func dispatch table insert new libs here
library_func_t libraryfuncs[]={
    libfunc_print,  //0
    libfunc_typeof, //1
    libfunc_input,  //2
    libfunc_sin,    //..
    libfunc_cos,
    libfunc_sqrt,
    libfunc_totalarguments,
    libfunc_objecttotalmembers,
    libfunc_argument,
    libfunc_objectmemberkeys,
    libfunc_strtonum,
    libfunc_objectcopy,
    libfunc_concat,
};

// ok
avm_memcell* avm_translate_operand( vmarg* arg,  avm_memcell* reg){
    assert(arg);
    switch(arg->type){
        case global_a: return &STACK[AVM_STACK_SIZE - 1 - arg->val];

        case local_a: return &STACK[topsp - arg->val];

        case formal_a: return &STACK[topsp + AVM_STACKENV_SIZE + 1 + arg->val];

        case retval_a: return &retval;

        case number_a:
            reg->type = number_m;
            reg->data.numVal = consts_getnumber(arg->val);
            return reg;

        case string_a:
            reg->type = string_m;
            reg->data.strVal = strdup(consts_getstring(arg->val));
            return reg;

        case bool_a:
            reg->type = bool_m;
            if(arg->val == 0)   reg->data.boolVal = 0;
            else                reg->data.boolVal = 1;
            return reg;

        case nil_a: reg->type = nil_m; return reg;

        case userfunc_a:
            reg->type = userfunc_m;
            reg->data.funcVal = arg->val;
            return reg;

        case libfunc_a:
            reg->type = libfunc_m;
            reg->data.libfuncVal = libfuncs_getused(arg->val);
            return reg;

        default:
         assert(0);
    }
}
// ok
double consts_getnumber(unsigned int index){
    assert(index < numConsts_array_size);
    return numConsts_array[index];
}
// ok
char* consts_getstring(unsigned int index){
    assert((index < stringConsts_array_size) && (stringConsts_array[index]));
    return stringConsts_array[index];
}
// ok
unsigned avm_totalactuals(void){
    return avm_get_envvalue(topsp + AVM_NUMACTUALS_OFFSET);
}
// ok
avm_memcell* avm_getactual(unsigned i){
    assert(i < avm_totalactuals());
    return &STACK[topsp + AVM_STACKENV_SIZE + 1 + i];
}

// convert_tobool for relop
// switch for dispatching
unsigned char avm_tobool(avm_memcell *m){
    assert(m->type >= 0 && m->type < undef_m);
    switch (m->type)
    {
    case number_m:  return m->data.numVal != 0;
    case string_m : if(m->data.strVal == NULL)return 0; return 1;
    case bool_m:    return m->data.boolVal ;    
    case table_m:   return 1;   
    case userfunc_m:return 1;
    case libfunc_m: return 1;
    case nil_m:     return 0;
    default:        assert(0);
    }
}

char* libfuncs_getused(unsigned int index){
    assert((index < libFuncs_array_size) && (libFuncs_array[index]));
    return libFuncs_array[index];
}
// ok
userfunc* avm_getfuncinfo(unsigned int address){
    unsigned i;

    for(i = 0; i < userFuncs_array_size; ++i){
        if(userFuncs_array[i].address == address){
            return &userFuncs_array[i];
        }
    }
}

// ok avm_warning
void avm_assign(avm_memcell* lv, avm_memcell* rv){
    if(lv == rv)
        return;
    if( (lv->type == table_m) && 
        (rv->type == table_m) &&
        lv->data.tableVal == rv->data.tableVal ){
        return;
    }
    if(rv->type == undef_m){
        fprintf(avm_warnings,"warning, assigning from \'undef\' content!, at line: %d\n",srcline);
    }
    avm_memcellclear(lv);

    memcpy(lv, rv, sizeof(avm_memcell));

    if(lv->type == string_m){
        lv->data.strVal = strdup(rv->data.strVal);
    }
    else if(lv->type == table_m){
        avm_tableincrefcounter(lv->data.tableVal);
    }
}

// ok ,return the fucntion in the libraryfuncs table ,insert new libs here too
library_func_t avm_get_libraryfunc(char* id){
    if ( ! strcmp(id , "print")){
        return libraryfuncs[0];
    }
    else if(! strcmp(id , "typeof")){
        return libraryfuncs[1];
    }
    else if(! strcmp(id , "input")){
        return libraryfuncs[2];
    }
    else if(! strcmp(id , "sin")){
        return libraryfuncs[3];
    }
    else if(! strcmp(id , "cos")){
        return libraryfuncs[4];
    }
    else if(! strcmp(id , "sqrt")){
        return libraryfuncs[5];
    }
    else if(! strcmp(id , "totalarguments")){
        return libraryfuncs[6];
    }
    else if(! strcmp(id , "objecttotalmembers")){
        return libraryfuncs[7];
    }
    else if(! strcmp(id , "argument")){
        return libraryfuncs[8];
    }
    else if(! strcmp(id , "objectmemberkeys")){
        return libraryfuncs[9];
    }
    else if(! strcmp(id , "strtonum")){
        return libraryfuncs[10];
    }
    else if(! strcmp(id , "objectcopy")){
        return libraryfuncs[11];
    }
    else if(! strcmp(id , "concat")){
        return libraryfuncs[12];
    }
    else{
        return NULL;
    }
}

// ok
void avm_calllibfunc(char* id){
    library_func_t f = avm_get_libraryfunc(id);
    if(!f){
        fprintf(avm_errors,"error, unsupported lib function \'%s\' called!, at line: %d\n",id,srcline);
        executionFinished = 1 ;
        return;
    }
    topsp = top;
    totalActuals = 0;
    (*f)();
    if( !executionFinished){
        execute_funcexit((instruction*) 0);
    }
}

// ok
unsigned int avm_get_envvalue(unsigned int i){
    unsigned val ;
    //assert(STACK[i].type == number_m);
    val = (unsigned) STACK[i].data.numVal;
    //assert(STACK[i].data.numVal == ((double) val));
    return val;
}
