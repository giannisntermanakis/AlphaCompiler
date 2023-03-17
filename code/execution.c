#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../headerfiles/avm_datatypes.h"
#include "../headerfiles/avm_functions.h"

// dispatching table
execute_func_t executeFuncs[]={
    execute_assign,
    execute_arithmetic,
    execute_arithmetic,
    execute_arithmetic,
    execute_arithmetic,
    execute_arithmetic,
    execute_arithmetic,
    execute_jump,
    execute_jeq,
    execute_jne,
    execute_jle,
    execute_jge,
    execute_jlt,
    execute_jgt,
    execute_call,
    execute_pusharg,
    execute_funcenter,
    execute_funcexit,
    execute_newtable,
    execute_tablegetelem,
    execute_tablesetelem,
    execute_nop
};


// ok use switch for dispatching
void execute_arithmetic( instruction* instr){
    avm_memcell *lv, *rv1, *rv2;
    unsigned l1 , l2 ,i;
    lv = avm_translate_operand(&instr->result, ( avm_memcell*) 0);
    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);

    assert(lv && ( &STACK[AVM_STACK_SIZE-1] >= lv && lv > &STACK[top] || lv == &retval ));
    assert(rv1 && rv2);

    

    if(rv1->type != number_m || rv2->type != number_m)
    {
        if(rv1->type == string_m && rv2->type == string_m){
            avm_memcellclear(lv);
            lv->type = string_m;
            if(instr->opcode == add_v){
                l1 = strlen(rv1->data.strVal);
                l2 = strlen(rv2->data.strVal);
		        lv->data.strVal = (char*)malloc(sizeof(char)* (l1+l2+1-2)); //-2 cause of the removed ""
                memset(lv->data.strVal , 0 , (l1+l2+1-2)*sizeof(char));
    
                for(i = 0 ; i < l1-1 ; ++i){
                    lv->data.strVal[i] = rv1->data.strVal[i];
                }
                for(i = 1 ; i < l2 ; ++i){
                    lv->data.strVal[i+l1-1-1] = rv2->data.strVal[i];
                }
                lv->data.strVal[i+l1] = '\0';
            }else{
                executionFinished = 1;
                fprintf(avm_errors,"error, invalid operation with strings!, at line: %d\n",srcline);
            }
        }
        else{
            executionFinished = 1;
            fprintf(avm_errors,"error, not a number in arithmetic!, at line: %d\n",srcline);
        }
    }
    else{
        avm_memcellclear(lv);
        lv->type = number_m;
        switch(instr->opcode){
            case add_v:
                lv->data.numVal = rv1->data.numVal + rv2->data.numVal;
                break;
            case sub_v:
                lv->data.numVal = rv1->data.numVal - rv2->data.numVal;
                break;
            case mul_v:
                lv->data.numVal = rv1->data.numVal * rv2->data.numVal;
                break;
            case div_v:
                if(rv2->data.numVal != 0)
                {
                    lv->data.numVal = rv1->data.numVal / rv2->data.numVal;
                    break;
                }else{
                    fprintf(avm_errors, "error, can't div with 0, at line: %d\n",srcline);
                    executionFinished = 1;
                    return;
                }
            case mod_v:
                if(rv2->data.numVal != 0)
                {
                    lv->data.numVal = (int)rv1->data.numVal % (int)rv2->data.numVal ;
                    break;    
                }
                else{
                    // avm_error("can't mod with 0",NULL);
                    fprintf(avm_errors, "error, can't mod with 0, at line: %d\n",srcline);
                    executionFinished = 1;
                    return;
                }
            default:
                fprintf(avm_errors,"error, execute_Arithmetic assert, at line: %d\n",srcline);
                assert(0);
        }
    }
}
// ok need some assert
void execute_assign( instruction* instr){
    avm_memcell *lv, *rv;
    lv = avm_translate_operand(&instr->result, ( avm_memcell*) 0);
    rv = avm_translate_operand(&instr->arg1, &ax);
    
    assert(lv && ( &STACK[AVM_STACK_SIZE-1] >= lv && lv> &STACK[top] || lv == &retval));

    assert(rv); // similar assertions

    avm_assign(lv,rv);
    
}
// ok ,
void execute_call( instruction* instr){
    // printf("types in res: %d arg1 :%d ar2 : %d\n",instr->result.type,instr->arg1.type,instr->arg2.type);
    char* s;
    avm_memcell* func = avm_translate_operand(&instr->arg1, &ax);
    assert(func);
    avm_callsaveenviroment();
    switch(func->type){
        case userfunc_m:
            pc = func->data.funcVal;
            assert(pc < AVM_ENDING_PC);
            assert(t_code[pc].opcode == funcenter_v);
            break;
        case string_m:
            avm_calllibfunc(func->data.strVal);
            break;
        case libfunc_m:
            avm_calllibfunc(func->data.libfuncVal);
            break;
        default:
            s = avm_tostring(func);
            printf("name %s\n",func->data.libfuncVal);
            fprintf(avm_errors, "error, call: cannot bind: \'%s\' to function!, at line: %d\n", s,srcline);
            free(s);
            executionFinished = 1;
    }
}
// ok ,throw warning when incorrect amount of formals 
void execute_funcenter( instruction* instr){
    // printf("fstart types in res: %d arg1 :%d ar2 : %d\n",instr->result.type,instr->arg1.type,instr->arg2.type);
    avm_memcell* func = avm_translate_operand(&instr->result, &ax);

    userfunc* funcInfo;
    assert(func);
    assert(pc == func->data.funcVal);
    //check for 
    funcInfo = avm_getfuncinfo(pc);
    if(funcInfo->totalFormals != STACK[top+4].data.numVal){
        fprintf(avm_warnings,"warning, calling %s with incorrect amount of formal arguments,expected %d ,got %d, at line: %d\n",
                funcInfo->id,funcInfo->totalFormals,totalActuals,srcline);
    }    
    // calee actions
    totalActuals = 0;
    topsp = top;
    top = top - funcInfo->localSize;
}
// ok
void execute_funcexit( instruction* instr){
    unsigned int oldTop = top;

    top = avm_get_envvalue(topsp + AVM_SAVEDTOP_OFFSET);
    pc = avm_get_envvalue(topsp + AVM_SAVEDPC_OFFSET);
    topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);

    while(++oldTop <= top){
        avm_memcellclear(&STACK[oldTop]);
    }
}
// ok
void execute_pusharg( instruction* instr){
    avm_memcell* arg = avm_translate_operand(&instr->arg1, &ax);
    assert(arg);

    avm_assign(&STACK[top], arg);
    ++totalActuals;
    // printf("[top=%d] = %f,totalacuals = %d \n",top , STACK[top].data.numVal,totalActuals);
    avm_dec_top();
}
// ok 
void execute_jeq( instruction* instr){  //CHECK AGAIN
    avm_memcell *rv1, *rv2;
    unsigned char result = 0;

    assert(instr->result.type == label_a);      //sigoureuoume oti einai label

    rv1 = avm_translate_operand(&instr->arg1, &ax); //kanoume translate ta dio operants
    rv2 = avm_translate_operand(&instr->arg2, &bx);
    assert(rv1 && rv2);

    if(rv1->type == undef_m || rv2->type == undef_m)    //check if undefined
    {
        fprintf(avm_errors, "error, \'undef\' involved in equality!, at line: %d\n",srcline);
        executionFinished = 1;
    }
    else if(rv1->type == nil_m || rv2->type == nil_m)
        result = (rv1->type == nil_m) && (rv2->type == nil_m);
    else if(rv1->type == bool_m || rv2->type == bool_m)
        result = avm_tobool(rv1) == avm_tobool(rv2);
    else if(rv1->type != rv2->type){
        fprintf(avm_errors, "error, %s == %s is illegal!, at line: %d\n", typeString[rv1->type], typeString[rv2->type],srcline);
        executionFinished = 1;
    }
    else{
        switch(rv1->type){
        case number_m:
            result = rv1->data.numVal == rv2->data.numVal;
            break;
        case string_m:
            result = strcmp(rv1->data.strVal, rv2->data.strVal) == 0;
            break;
        case table_m:
            result = rv1->data.tableVal == rv2->data.tableVal;
            break;
        case userfunc_m:
            result = rv1->data.funcVal == rv2->data.funcVal;
            break;
        case libfunc_m:
            result = strcmp(rv1->data.strVal, rv2->data.strVal) == 0;
            break;
        default:
            assert(0);
        }
    }

    if(!executionFinished && result) 
        pc = instr->result.val;
}

void execute_jne( instruction* instr){ //SAME WITH ABOVE
    avm_memcell *rv1, *rv2;
    unsigned char result = 0;

    assert(instr->result.type == label_a);

    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);
    assert(rv1 && rv2);

    if(rv1->type == undef_m || rv2->type == undef_m)
    {
        fprintf(avm_errors, "error, \'undef\' involved in not equal!, at line: %d\n",srcline);
        executionFinished = 1;
    }
    else if(rv1->type == nil_m || rv2->type == nil_m){ 
        result = !(rv1->type == nil_m && rv2->type == nil_m);
    }
    else if(rv1->type == bool_m || rv2->type == bool_m) 
        result = !(avm_tobool(rv1) == avm_tobool(rv2));
    else if(rv1->type != rv2->type){
        fprintf(avm_errors, "error, %s == %s is illegal!, at line: %d\n", typeString[rv1->type], typeString[rv2->type],srcline);
        executionFinished = 1;
    }
    else{
        switch(rv1->type){
        case number_m:
            result = (rv1->data.numVal != rv2->data.numVal);
            break;
        case string_m:
            result = (strcmp(rv1->data.strVal, rv2->data.strVal));
            break;
        case table_m:
            result = (rv1->data.tableVal != rv2->data.tableVal);
            break;
        case userfunc_m:
            result = (rv1->data.funcVal != rv2->data.funcVal);
            break;
        case libfunc_m:
            result = (strcmp(rv1->data.strVal, rv2->data.strVal));
            break;
        default:
            assert(0);
        }
    }
    if(!executionFinished && result) pc = instr->result.val;
}

void execute_jle( instruction* instr){
    avm_memcell *rv1, *rv2;
    unsigned char result = 0;

    assert(instr->result.type == label_a);

    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);
    assert(rv1 && rv2);

    if(rv1->type != number_m || rv2->type != number_m){
        fprintf(avm_errors, "error, not a number in less equal!, at line: %d\n",srcline);
        executionFinished = 1;

    }else{
        result = (rv1->data.numVal <= rv2->data.numVal);
    }
    if(!executionFinished && result) pc = instr->result.val; 
}

void execute_jge( instruction* instr){
    avm_memcell *rv1, *rv2;
    unsigned char result = 0;

    assert(instr->result.type == label_a);

    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);

    assert(rv1 && rv2);

    if(rv1->type != number_m || rv2->type != number_m){
        fprintf(avm_errors, "error, not a number in greater equal!, at line: %d\n",srcline);
        executionFinished = 1;
    }else{
        result = (rv1->data.numVal >= rv2->data.numVal);
    }

    if(!executionFinished && result)
        pc = instr->result.val;
}

void execute_jlt( instruction* instr){
    avm_memcell *rv1, *rv2;
    unsigned char result = 0;

    assert(instr->result.type == label_a);

    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);

    assert(rv1 && rv2);

    if(rv1->type != number_m || rv2->type != number_m){
        fprintf(avm_errors, "error, not a number in less than!, at line: %d\n",srcline);
        executionFinished = 1;
    }else{
        result = (rv1->data.numVal < rv2->data.numVal);
    }

    if(!executionFinished && result)
        pc = instr->result.val;
    
}

void execute_jgt( instruction* instr){
    avm_memcell *rv1, *rv2;
    unsigned char result = 0;

    assert(instr->result.type == label_a);

    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);

    assert(rv1 && rv2);

    if(rv1->type != number_m || rv2->type != number_m){
        fprintf(avm_errors, "error, not a number in greater than!, at line: %d\n",srcline);
        executionFinished = 1;
    }else{
        result = (rv1->data.numVal > rv2->data.numVal);
    }

    if(!executionFinished && result)
        pc = instr->result.val;
    
}

void execute_jump( instruction* instr){

    if(instr->result.val > t_code_size){
        fprintf(avm_errors, "error, jump not to an instruction!, at line: %d\n",srcline);
        executionFinished = 1;
    }
    else 
        pc = instr->result.val;
}

void execute_nop(instruction* instr){
    return ;
}
// ok
void execute_newtable(instruction* instr){
    avm_memcell* lv = avm_translate_operand(&instr->arg1, (avm_memcell*) 0);
    assert(lv && ( &STACK[AVM_STACK_SIZE-1] >=  lv && lv > &STACK[top] || lv == &retval));
    avm_memcellclear(lv);

    lv->type = table_m;
    lv->data.tableVal = avm_tablenew();
    avm_tableincrefcounter(lv->data.tableVal);
}
// ok new file for warnings
void execute_tablegetelem(instruction* instr){
    avm_memcell *lv, *t, *i, *content;
    char *ts, *is;
    // printf("inside , instr op %d,res %d , arg1 %d , arg2 %d\n",instr->opcode,instr->result.type,instr->arg1.type,instr->arg2.type);

    lv = avm_translate_operand(&instr->result, (avm_memcell*) 0);
    t = avm_translate_operand(&instr->arg1, (avm_memcell*) 0);
    i = avm_translate_operand(&instr->arg2, &ax);

    assert(lv && &STACK[AVM_STACK_SIZE-1] >= lv && lv> &STACK[top] || lv == &retval);
    assert(t && &STACK[AVM_STACK_SIZE-1] >= t && t > &STACK[top]);
    assert(i);

    avm_memcellclear(lv);
    lv->type = nil_m;

    if(t->type != table_m){
        fprintf(avm_errors, "error,illegal use of type %s as table!, at line: %d\n", typeString[t->type],srcline);
        executionFinished = 1;
    } 
    else{
        content = avm_tablegetelem(t->data.tableVal, i);

        if(content)
            avm_assign(lv, content);
        else{
            ts = avm_tostring(t);
            is = avm_tostring(i);
            // avm warning
            fprintf(avm_warnings, "warning, %s[%s] not found!, at line: %d\n", ts, is,srcline);
            free(ts);
            free(is);
        }
    }
    
}
// ok
void execute_tablesetelem(instruction* instr){
    avm_memcell *t, *i, *c;

    t = avm_translate_operand(&instr->arg1, ( avm_memcell*) 0);
    i = avm_translate_operand(&instr->arg2, &ax);
    c = avm_translate_operand(&instr->result, &bx);
    assert(t && &STACK[AVM_STACK_SIZE-1] >= t && t > &STACK[top]);
    assert( i && c);

    if(t->type != table_m)
    {
        fprintf(avm_errors, "error, illegal use of type %s as table!, at line: %d\n", typeString[t->type],srcline);
        executionFinished = 1;
    }
    else 
        avm_tablesetelem(t->data.tableVal, i, c);
}

void execute_cycle(){
    instruction* instr;
    unsigned oldPC;

    if(executionFinished) return;
    else
    if(pc == AVM_ENDING_PC){
        executionFinished = 1;
        return;
    }
    else{
        assert(pc < AVM_ENDING_PC);
        instr = t_code + pc;
        assert(
            instr->opcode >= 0 &&
            instr->opcode <= AVM_MAX_INSTRUCTIONS
        );
        if(instr->srcLine)
            srcline = instr->srcLine;
        oldPC = pc;
        (*executeFuncs[instr->opcode])(instr);
        if(pc == oldPC)
            ++pc;
    }

}

