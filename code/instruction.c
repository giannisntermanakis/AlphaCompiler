#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../headerfiles/tokens.h"
#include "../headerfiles/i_code_datatypes.h"
#include "../headerfiles/symtable.h"
#include "../headerfiles/i_code_functions.h"
#include "../headerfiles/stack.h"
#include "../headerfiles/t_code_datatypes.h"
#include "../headerfiles/t_code_functions.h"
#include "../headerfiles/create_abc_file.h"

#define     EXPAND_SIZE_INSTR 1024
#define     CURR_SIZE_INSTR   (instr_total*(sizeof(instruction)))
#define     NEW_SIZE_INSTR    (EXPAND_SIZE_INSTR * sizeof(instruction) + CURR_SIZE_INSTR)

extern quad*    quads;
extern unsigned total;
extern unsigned currQuad;
unsigned        procQuad = 0;


#define     EXPAND_SIZE_STR   64
#define     CURR_SIZE_STR     (totalStringCosnts*(sizeof(char*)))
#define     NEW_SIZE_STR      (EXPAND_SIZE_STR*sizeof(char*)+CURR_SIZE_STR)

#define     EXPAND_SIZE_NUM   64
#define     CURR_SIZE_NUM     (totalNumConsts*(sizeof(double)))
#define     NEW_SIZE_NUM      (EXPAND_SIZE_NUM*sizeof(double)+CURR_SIZE_NUM)

#define     EXPAND_SIZE_UFUNC   64
#define     CURR_SIZE_UFNC     (totalUserFuncs*(sizeof(userfunc)))
#define     NEW_SIZE_UFNC      (EXPAND_SIZE_UFUNC*sizeof(userfunc)+CURR_SIZE_UFNC)

#define     EXPAND_SIZE_FUNC_STACK   64
#define     CURR_SIZE_FUNC_STACK     (totalFuncStack*(sizeof(userfunc*)))
#define     NEW_SIZE_FUNC_STACK      (EXPAND_SIZE_FUNC_STACK*sizeof(userfunc*)+CURR_SIZE_FUNC_STACK)

#define     EXPAND_SIZE_LIB   64
#define     CURR_SIZE_LIB     (totalNameLibfuncs*(sizeof(char*)))
#define     NEW_SIZE_LIB      (EXPAND_SIZE_LIB*sizeof(char*)+CURR_SIZE_LIB)
incomplete_jump*    ij_head = (incomplete_jump*) 0;
unsigned            ij_total = 0;

returnList*         return_head = (returnList*)0;
unsigned            return_total = 0;


instruction*        instructions = (instruction*) 0;
unsigned            instr_total = 0 ;
unsigned            currInstr = 0 ;


double*     numConsts = (double*) 0;
unsigned    totalNumConsts = 0 ;
unsigned    currNum = 0;

char**      stringConsts = (char**) 0;
unsigned    totalStringCosnts = 0 ;
unsigned    currStr = 0 ;

char**      nameLibfuncs = (char**) 0;
unsigned    totalNameLibfuncs = 0 ;
unsigned    currLibFuncs = 0 ;

userfunc*   userFuncs = (userfunc*) 0;
unsigned    totalUserFuncs = 0 ;
unsigned    currUserFuncs = 0 ;

userfunc**  FuncStack = (userfunc**) 0;
unsigned    totalFuncStack = 0 ;
int         currFuncStack = 0 ;

static void print_tcode();
static void generate_tcode();

void expand_func_stack(){
    assert(totalFuncStack  == currFuncStack);
    userfunc** p =(userfunc**) malloc(NEW_SIZE_FUNC_STACK) ;
    if(FuncStack){
        memcpy(p , FuncStack , CURR_SIZE_FUNC_STACK);
        free(FuncStack);
    }
    FuncStack = p;
    totalFuncStack += EXPAND_SIZE_FUNC_STACK ;
}

void pushFuncStack( userfunc* data){
    if( FuncStack == NULL || totalFuncStack  == currFuncStack){
        expand_func_stack();
    }
    FuncStack[currFuncStack] = (userfunc*) malloc(sizeof(userfunc));
    FuncStack[currFuncStack]->id = strdup(data->id) ;
    FuncStack[currFuncStack]->address = data->address;
    FuncStack[currFuncStack]->localSize = data->localSize;
    FuncStack[currFuncStack++]->retlist = data->retlist;
    
}

userfunc* popFuncStack(){
    userfunc* r ;
    if(FuncStack == NULL ||  currFuncStack == -1 ){
        return NULL;
    }
    else{
        r = (userfunc*) malloc(sizeof(userfunc));
        r->id = strdup(FuncStack[--currFuncStack]->id);
        r->address = FuncStack[currFuncStack]->address;
        r->localSize = FuncStack[currFuncStack]->localSize;
        r->retlist = FuncStack[currFuncStack]->retlist;
        free(FuncStack[currFuncStack]);
        return r;
    }
}
returnList* add_return_list(returnList *l,unsigned instrNo){
    returnList* tmp;
    if(l == NULL){
        l = malloc(sizeof(returnList));
        l->instrNo = instrNo;
        l->next = NULL ;
    }
    else{
        tmp = malloc(sizeof(returnList));
        tmp->next = l ;
        tmp->instrNo = instrNo;

        l = tmp;
    }    
    return l;
}
void reset_return_list(returnList* l){
    returnList* prev ,*tmp = l;

    while (tmp!= NULL)
    {   
        prev = tmp ;
        tmp = tmp->next;
        free(prev);
    }
    
}

void expand_instr(void){
    assert(instr_total  == currInstr);
    instruction* p = (instruction*) malloc(NEW_SIZE_INSTR);
    if(instructions){
        memcpy(p , instructions , CURR_SIZE_INSTR);
        free(instructions);
    }
    instructions = p;
    instr_total += EXPAND_SIZE_INSTR ;
}


void backpatch_returnList(returnList *l , unsigned u){
    returnList* tmp = l;   
    for(tmp ; tmp != NULL ; tmp = tmp->next){
        instructions[tmp->instrNo].result.val = u;
    }
}

void add_incomplete_jump(unsigned instrNo , unsigned iaddress){
    incomplete_jump* tmp;
    if(ij_head == NULL){
        ij_head = malloc(sizeof(incomplete_jump));
        ij_head->next = NULL ;
        ij_head->instrNo = instrNo;
        ij_head->iaddress = iaddress;
    }
    else{
        tmp = malloc(sizeof(incomplete_jump));
        tmp->next = ij_head ;
        tmp->instrNo = instrNo;
        tmp->iaddress = iaddress;  

        ij_head = tmp;
    }
    ++ij_total;
}
void patch_incomplete_jumps(){
    incomplete_jump* tmp = ij_head;
    for( tmp ; tmp!= NULL ; tmp = tmp->next){
        // not sure if means this
        if(tmp->iaddress == currQuad ){
            instructions[tmp->instrNo].result.val = currInstr;
        }else{
            instructions[tmp->instrNo].result.val = quads[tmp->iaddress].taddress;
        }

    }
}

void emit_instr(instruction i ){
    if(currInstr == instr_total)
        expand_instr();
        
    instructions[currInstr++] = i ; 
}

/*
string array done
*/
void expand_str(){
    assert(totalStringCosnts  == currStr);
    char** p = (char**) malloc(NEW_SIZE_STR);
    if(stringConsts){
        memcpy(p , stringConsts , CURR_SIZE_STR);
        free(stringConsts);
    }
    stringConsts = p;
    totalStringCosnts += EXPAND_SIZE_STR ;
}
unsigned consts_newstring(char*s){
    if(totalStringCosnts == currStr)
        expand_str();

    stringConsts[currStr] = strdup(s);
    return currStr++;
}

/*
number array done
*/
void expand_num(){
    assert(totalNumConsts  == currNum);
    double* p = (double*) malloc(NEW_SIZE_NUM);
    if(numConsts){
        memcpy(p , numConsts , CURR_SIZE_NUM);
        free(numConsts);
    }
    numConsts = p;
    totalNumConsts += EXPAND_SIZE_NUM ;
}
unsigned consts_newnumber(double n){
    /*return the index saved in number array*/
    if(totalNumConsts == currNum)
        expand_num();

    numConsts[currNum] = n;
    return currNum++;
}
/*
funcs array done
*/
void expand_ufunc(){
    assert(totalUserFuncs  == currUserFuncs);
    userfunc* p = (userfunc*) malloc(NEW_SIZE_UFNC);
    if(userFuncs){
        memcpy(p , userFuncs , CURR_SIZE_UFNC);
        free(userFuncs);
    }
    userFuncs = p;
    totalUserFuncs += EXPAND_SIZE_NUM ;
}
unsigned userfuncs_newfunc(char* id , unsigned addr , returnList* list , unsigned size , unsigned formals){
    if(totalUserFuncs == currUserFuncs)
        expand_ufunc();
    userFuncs[currUserFuncs].address= addr;
    userFuncs[currUserFuncs].id= strdup(id);
    userFuncs[currUserFuncs].retlist= list;
    userFuncs[currUserFuncs].localSize= size;
    userFuncs[currUserFuncs].totalFormals= formals;
    return currUserFuncs++;
}

/*
libs array done
*/
void expand_lib(){
    assert(totalNameLibfuncs  == currLibFuncs);
    char** p = (char**) malloc(NEW_SIZE_LIB);
    if(nameLibfuncs){
        memcpy(p , nameLibfuncs , CURR_SIZE_LIB);
        free(nameLibfuncs);
    }
    nameLibfuncs = p;
    totalNameLibfuncs += EXPAND_SIZE_LIB ;
}
unsigned libfuncs_newused(char* s){
    if(totalNameLibfuncs == currLibFuncs)
        expand_lib();

    nameLibfuncs[currLibFuncs] = strdup(s);

    return currLibFuncs++;    
}

void make_numberoperand (vmarg* arg , double val){

    arg -> val = consts_newnumber(val);
    arg -> type = number_a;
}

void make_booloperand (vmarg* arg , unsigned val) {
    arg -> val = val;
    arg -> type = bool_a;
}

void make_retvaloperand (vmarg* arg){

    arg->type = retval_a;
}

void make_operand(expr* e , vmarg* arg){
    switch (e->type){

        case var_e:
        case tableitem_e:
        case arithexpr_e:
        case boolexpr_e:
        case newtable_e:{
            assert(e->sym);
            arg->val = e->sym->offset ;
            switch (e->sym->space)
            {
            case program_var :      arg->type = global_a;   break;
            case function_local :   arg->type = local_a;    break;
            case formal_arg :       arg->type = formal_a;    break;       
            default:
                assert(0);
            }
            break;
        }
        case constbool_e:{
            arg->val = e->boolConst;
            arg->type = bool_a;
            break;
        }   
        case conststring_e:{
            arg->val = consts_newstring(e->strConst);
            arg->type = string_a;
            break;
        }
        case constnum_e:{
            arg->val = consts_newnumber(e->numConst);
            arg->type = number_a;
            break;
        }
        case nil_e:{
            arg->type = nil_a;
            break ;
        }
        case programfunc_e:{
            arg->type = userfunc_a;
            arg->val = e->sym->taddress;    
            break;
        }
        case libraryfunc_e : {
            arg->type = libfunc_a;
            arg->val = libfuncs_newused(e->sym->value->name);
            break;
        }
        
        default:    assert(0);
    }
}

unsigned nextinstructionlabel(){
    return currInstr;
}

void generate(vmopcode op,quad* q){
    instruction t;
    memset(&t , 0 , sizeof(instruction));
    t.opcode = op;

    switch(op){
        
        case add_v: 
        case sub_v:
        case mul_v:
        case div_v:
        case mod_v:
            {
                if(q->op == uminus_v){
                    make_operand(q->result, &t.result );
                    make_operand(q->arg1, &t.arg1 );
                    make_numberoperand(&t.arg2, -1);
                }else{
                    make_operand(q->result, &t.result );
                    make_operand(q->arg1, &t.arg1);
                    make_operand(q->arg2, &t.arg2);
                }
                q->taddress = nextinstructionlabel();
                t.srcLine = q->line;
                emit_instr(t);
            }
            break ;
        case assign_v:
            {
                make_operand(q->result, &t.result );
                make_operand(q->arg1, &t.arg1);
                t.arg2.type = ignore_a;
                q->taddress = nextinstructionlabel();
                t.srcLine = q->line;
                emit_instr(t);
            }  
            break;
        case newtable_v:{
                    make_operand(q->arg1 , &t.arg1);
                    q->taddress = nextinstructionlabel();
                    t.result.type = ignore_a;
                    t.arg2.type = ignore_a;
                    t.srcLine = q->line;
                    emit_instr(t);
                    }
                break;

        case tablegetelem_v:
        case tablesetelem_v:
                {
                    make_operand(q->arg1 , &t.arg1);
                    make_operand(q->arg2 , &t.arg2);
                    make_operand(q->result , &t.result);
                    q->taddress = nextinstructionlabel();
                    t.srcLine = q->line;
                    emit_instr(t);
                }
                break;
        default:    assert(0);
    }
}
void generate_relational(vmopcode op,quad* q){
    instruction t;
    t.opcode = op;
    if(op != jump_v){
        make_operand(q->arg1 , &t.arg1);
        make_operand(q->arg2 , &t.arg2);
    }
    t.result.type = label_a;
    if (q->label< curr_process_quad()){
        t.result.val = quads[q->label].taddress;

    }else{
        add_incomplete_jump(nextinstructionlabel() , q->label);
    }
    q->taddress = nextinstructionlabel();
    emit_instr(t);
}
void generate_ADD(quad* q){
    generate(add_v, q);
}                        
void generate_SUB(quad* q){
    generate(sub_v, q);
}                        
void generate_MUL(quad* q){
    generate(mul_v, q);
}                        
void generate_DIV(quad* q){
    generate(div_v, q);
}                        
void generate_MOD(quad* q){
    generate(mod_v, q);
}
void generate_UMINUS(quad* q){
    generate(mul_v , q );
}
void generate_NEWTABLE(quad* q){
    generate(newtable_v,q);
}
void generate_TABLEGETELEM(quad* q){
    generate(tablegetelem_v,q);
}
void generate_TABLESETELEM(quad* q){
    generate(tablesetelem_v,q);
}
void generate_ASSIGN(quad* q){
    generate(assign_v,q);
}
void generate_NOP(){
    instruction t;
    memset(&t,0,sizeof(instruction));
    t.opcode = nop_v;
    t.result.type = ignore_a;
    t.arg1.type = ignore_a;
    t.arg2.type = ignore_a;
    emit_instr(t);
}                        
void generate_JUMP(quad* q){
    generate_relational(jump_v,q);
}
void generate_IF_EQ(quad* q){
    generate_relational(jeq_v,q);
}
void generate_IF_NOTEQ(quad* q){
    generate_relational(jne_v,q);
}
void generate_IF_GREATER(quad* q){
    generate_relational(jgt_v,q);
}
void generate_IF_GREATEREQ(quad* q){
    generate_relational(jge_v,q);
}
void generate_IF_LESS(quad* q){
    generate_relational(jlt_v,q);
}
void generate_IF_LESSEQ(quad* q){
    generate_relational(jle_v,q);
}

void generate_PARAM(quad* q){
    instruction t;
    memset(&t,0,sizeof(instruction));
    q->taddress = nextinstructionlabel();
    t.opcode = pusharg_v ;
    make_operand(q->arg1 , &t.arg1);
    t.result.type = ignore_a;
    t.arg2.type = ignore_a;
    t.srcLine = q->line;
    emit_instr(t);
}
void generate_CALL(quad* q){
    instruction t;
    memset(&t,0,sizeof(instruction));
    q->taddress = nextinstructionlabel();
    t.opcode = call_v ;
    make_operand(q->arg1 , &t.arg1);
    t.arg2.type = ignore_a;
    t.result.type = ignore_a;
    t.srcLine = q->line;
    emit_instr(t);
}
void generate_GETRETVAL(quad* q){
    instruction t;
    memset(&t,0,sizeof(instruction));
    q->taddress = nextinstructionlabel();
    t.opcode = assign_v ;
    t.arg2.type = ignore_a;
    make_operand(q->arg1 , &t.result);
    make_retvaloperand(&t.arg1);
    t.srcLine = q->line;
    emit_instr(t);
}


void generate_FUNCSTART(quad* q){
    instruction t , j;
    userfunc f;
    memset(&t,0,sizeof(instruction));
    memset(&j,0,sizeof(instruction));
    memset(&f,0,sizeof(userfunc));


    //jump end
    j.opcode = jump_v ;
    j.result.type = label_a;
    j.arg1.type = ignore_a;
    j.arg2.type = ignore_a;

    emit_instr(j);
    //funcstart
    f.id = q->result->sym->value->name;
    f.address = nextinstructionlabel();
    f.retlist = NULL;
    f.localSize =q->result->sym->totallocals;
    f.totalFormals = q->result->sym->funcargs;
    
    // also have the number of formal arguments in the func->sym to check call params

    q->result->sym->taddress = f.address;
    q->taddress = nextinstructionlabel();

    pushFuncStack( &f);
    userfuncs_newfunc(f.id,f.address,f.retlist,f.localSize , f.totalFormals);

    t.opcode = funcenter_v ;
    t.arg1.type = ignore_a;
    t.arg2.type = ignore_a;
    make_operand(q->result , &t.result);

    t.srcLine = q->line;

    emit_instr(t);
}

void generate_RETURN(quad* q){
    instruction t , r;
    userfunc* f;
   
    memset(&t,0,sizeof(instruction));
    memset(&r,0,sizeof(instruction));

    q->taddress = nextinstructionlabel();
    t.opcode = assign_v;
    make_retvaloperand(&t.result);
    make_operand(q->arg1 , &t.arg1);
    t.arg2.type = ignore_a;
    t.srcLine = q->line;
    emit_instr(t);
    
    f = popFuncStack();
    f->retlist = add_return_list(f->retlist , nextinstructionlabel());
    pushFuncStack(f);
    r.opcode = jump_v;
    r.result.type = label_a;
    r.arg1.type = ignore_a;
    r.arg2.type = ignore_a;
    //line has to be 0
    emit_instr(r);
}

void generate_FUNCEND(quad* q){
    instruction t;
    userfunc *f;

    memset(&t,0,sizeof(instruction));

    f = popFuncStack();
    //f.address-1 ,jumps here
    instructions[f->address - 1].result.val = nextinstructionlabel()+1;
    backpatch_returnList(f->retlist , nextinstructionlabel());

    q->taddress = nextinstructionlabel();

    t.opcode = funcexit_v;
    t.arg1.type = ignore_a;
    t.arg2.type = ignore_a;
    
    make_operand(q->result , &t.result);
    
    t.srcLine = q->line;
    emit_instr(t);
}
unsigned curr_process_quad(){
    return procQuad;
}

/* quads ----> instructions */
void create_tcode(){
    generate_tcode();
    patch_incomplete_jumps();
    print_tcode();
    create_abc_file();
}
static void generate_tcode(){
    procQuad = 0;
    while(procQuad != currQuad){
        switch(quads[procQuad].op){
            case uminus:    generate_UMINUS(&quads[procQuad]);
                            break;
            case assign:    generate_ASSIGN(&quads[procQuad]);
                            break;
            case add:       generate_ADD(&quads[procQuad]);
                            break;
            case sub:       generate_SUB(&quads[procQuad]);
                            break;
            case mul:       generate_MUL(&quads[procQuad]);
                            break;
            case dev:       generate_DIV(&quads[procQuad]);
                            break;
            case mod:       generate_MOD(&quads[procQuad]);
                            break;
            case jump:      generate_JUMP(&quads[procQuad]);
                            break;
            case if_eq:     generate_IF_EQ(&quads[procQuad]);
                            break;
            case if_noteq:  generate_IF_NOTEQ(&quads[procQuad]);
                            break;
            case if_less:   generate_IF_LESS(&quads[procQuad]);
                            break;
            case if_lesseq: generate_IF_LESSEQ(&quads[procQuad]);
                            break;
            case if_greater: generate_IF_GREATER(&quads[procQuad]);
                            break;
            case if_greatereq: generate_IF_GREATEREQ(&quads[procQuad]);
                            break;
            case param:     generate_PARAM(&quads[procQuad]);
                            break;
            case call:      generate_CALL(&quads[procQuad]);
                            break;
            case getretval: generate_GETRETVAL(&quads[procQuad]);
                            break; 
            case funcstart: generate_FUNCSTART(&quads[procQuad]);
                            break;
            case funcend:   generate_FUNCEND(&quads[procQuad]);
                            break;
            case ret:       generate_RETURN(&quads[procQuad]);
                            break;
            case tablecreate:generate_NEWTABLE(&quads[procQuad]);
                            break;
            case tablegetelem:generate_TABLEGETELEM(&quads[procQuad]);
                            break;
            case tablesetelem:generate_TABLESETELEM(&quads[procQuad]);
                            break;
            default:
                assert(0);
        }
        ++procQuad;
   }
}
static void print_tcode(){
    int i ;
    fprintf(stderr,"---target code---\n");
    for(i = 0 ; i  <currInstr ; ++i){
        fprintf(stderr,"%d\t",i);
        switch (instructions[i].opcode)
        {
        case assign_v:
            fprintf(stderr,"assign_v\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t\t\t[line: %d]",instructions[i].result.type,instructions[i].result.val,instructions[i].arg1.type,instructions[i].arg1.val,instructions[i].srcLine);
            break;
        case add_v:
            fprintf(stderr,"add_v\t\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t\t\t[line: %d]",instructions[i].result.type,instructions[i].result.val,instructions[i].arg1.type,instructions[i].arg1.val,instructions[i].arg2.type,instructions[i].arg2.val,instructions[i].srcLine);
            break;
        case sub_v:
            fprintf(stderr,"sub_v\t\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t\t\t[line: %d]",instructions[i].result.type,instructions[i].result.val,instructions[i].arg1.type,instructions[i].arg1.val,instructions[i].arg2.type,instructions[i].arg2.val,instructions[i].srcLine);
            break;
        case mul_v:
            fprintf(stderr,"mul_v\t\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t\t\t[line: %d]",instructions[i].result.type,instructions[i].result.val,instructions[i].arg1.type,instructions[i].arg1.val,instructions[i].arg2.type,instructions[i].arg2.val,instructions[i].srcLine);
            break;
        case div_v:
            fprintf(stderr,"div_v\t\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t\t\t[line: %d]",instructions[i].result.type,instructions[i].result.val,instructions[i].arg1.type,instructions[i].arg1.val,instructions[i].arg2.type,instructions[i].arg2.val,instructions[i].srcLine);
            break;
        case mod_v:
            fprintf(stderr,"mod_v\t\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t\t\t[line: %d]",instructions[i].result.type,instructions[i].result.val,instructions[i].arg1.type,instructions[i].arg1.val,instructions[i].arg2.type,instructions[i].arg2.val,instructions[i].srcLine);
            break;
        case jump_v:
            fprintf(stderr,"jump_v\t\t%d\t\t\t\t\t\t\t\t[line: %d]",instructions[i].result.val,instructions[i].srcLine);
            break;
        case jle_v:
            fprintf(stderr,"jle_v\t\t%d\t\t\t\t\t\t\t\t[line: %d]",instructions[i].result.val,instructions[i].srcLine);
            break;
        case jlt_v:
            fprintf(stderr,"jlt_v\t\t%d\t\t\t\t\t\t\t\t[line: %d]",instructions[i].result.val,instructions[i].srcLine);
            break;
        case jge_v:
            fprintf(stderr,"jge_v\t\t%d\t\t\t\t\t\t\t\t[line: %d]",instructions[i].result.val,instructions[i].srcLine);
            break;
        case jgt_v:
            fprintf(stderr,"jgt_v\t\t%d\t\t\t\t\t\t\t\t[line: %d]",instructions[i].result.val,instructions[i].srcLine);
            break;
        case jeq_v:
            fprintf(stderr,"jeq_v\t\t%d\t\t\t\t\t\t\t\t[line: %d]",instructions[i].result.val,instructions[i].srcLine);
            break;
        case jne_v:
            fprintf(stderr,"jne_v\t\t%d\t\t\t\t\t\t\t\t[line: %d]",instructions[i].result.val,instructions[i].srcLine);
            break;
        case funcenter_v:
            fprintf(stderr,"funcenter_v\t(vmarg_t)%d,(offset)%d\t\t\t\t\t\t[line: %d]",instructions[i].result.type,instructions[i].result.val,instructions[i].srcLine);
            break;
        case funcexit_v:
            fprintf(stderr,"funcexit_v\t(vmarg_t)%d,(offset)%d\t\t\t\t\t\t[line: %d]",instructions[i].result.type,instructions[i].result.val,instructions[i].srcLine);
            break;
        case call_v:
            fprintf(stderr,"call_v\t\t(vmarg_t)%d,(offset)%d\t\t\t\t\t\t[line: %d]",instructions[i].arg1.type,instructions[i].arg1.val,instructions[i].srcLine);
            break;
        case pusharg_v:
            fprintf(stderr,"pusharg_v\t(vmarg_t)%d,(offset)%d\t\t\t\t\t\t[line: %d]",instructions[i].arg1.type,instructions[i].arg1.val,instructions[i].srcLine);
            break;
        case newtable_v:
            fprintf(stderr,"newtable_v\t(vmarg_t)%d,(offset)%d\t\t[line: %d]",instructions[i].arg1.type,instructions[i].arg1.val,instructions[i].srcLine);
            break;
        case tablesetelem_v:
            fprintf(stderr,"tablesetelem_v\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t\t[line: %d]",instructions[i].result.type,instructions[i].result.val,instructions[i].arg1.type,instructions[i].arg1.val,instructions[i].arg2.type,instructions[i].arg2.val,instructions[i].srcLine);
            break;
        case tablegetelem_v:
            fprintf(stderr,"tablegetelem_v\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t(vmarg_t)%d,(offset)%d\t\t[line: %d]",instructions[i].result.type,instructions[i].result.val,instructions[i].arg1.type,instructions[i].arg1.val,instructions[i].arg2.type,instructions[i].arg2.val,instructions[i].srcLine);
            break;
        default:
            fprintf(stderr,"assertion for op = %d , to line %d\n",instructions[i].opcode,instructions[i].result.val);
            assert(0);
            break;
        }
        fprintf(stderr,"\n");
    }
}