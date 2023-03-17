/**
 * csd4393 , csd4225 , csd4226
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../headerfiles/tokens.h"
#include "../headerfiles/i_code_datatypes.h"
#include "../headerfiles/symtable.h"
#include "../headerfiles/i_code_functions.h"
#include "../headerfiles/stack.h"


#define     EXPAND_SIZE 1024
#define     CURR_SIZE   (total*(sizeof(quad)))
#define     NEW_SIZE    (EXPAND_SIZE * sizeof(quad) + CURR_SIZE)

extern FILE* quadfile , *errors;
extern int yylineno , scope ,syntax_errors_counter;

int tempcounter = 1 ;

quad*   quads = (quad*) 0;

unsigned total = 0;
unsigned currQuad = 0;
unsigned program_var_offset = 0;
unsigned function_local_offset = 0;
unsigned formal_arg_offset = 0;
unsigned scope_space_counter = 1;
unsigned *last_function_local_offset;
unsigned last_flo_count = 0;

void expand(void){
    assert(total  == currQuad);
    quad* p = (quad*) malloc(NEW_SIZE);
    if(quads){
        memcpy(p , quads , CURR_SIZE);
        free(quads);
    }
    quads = p;
    total += EXPAND_SIZE ;
}

void emit(
        iopcode     op,
        expr*       result,
        expr*       arg1,
        expr*       arg2,
        unsigned    label,
        unsigned    line
        )
{
    quad* p;
    if(currQuad == total)
        expand();
        
    p     = quads +currQuad++;
    p->op       = op;
    p->arg1     = arg1;
    p->arg2     = arg2;
    p->result   = result;
    p->label    = label;
    p->line     = line;  
}
void patchlabel(unsigned quadNo, unsigned label){
    assert(quadNo < currQuad && !quads[quadNo].label) ;
    quads[quadNo].label = label ;
}

void make_stmt(stmt_t* s){
    s->breakList = 0 ;
    s->contList = 0 ;
}

void patchlist(int list , int label){
    int next ;
    while(list){
        next = quads[list].label;
        quads[list].label = label ;
        list = next ;
    }
}

void resettemp(){
    tempcounter = 1 ;
}

void patch_label(unsigned quadNo , unsigned label){
    assert(quadNo < currQuad);
    quads[quadNo].label = label ;
}

void print_quads(){
    int i = 0 ;
    for( i = 0 ; i < currQuad ; ++i ){
        fprintf(quadfile,"%d:\t",i);
        print_details(quads[i].arg1 , quads[i].arg2 , quads[i].result , quads[i].op , quads[i].label , quads[i].line);
    }
}

void enter_scope_space(){
    ++scope_space_counter;
    }

void exits_scope_space(){
    assert(scope_space_counter > 1);
    --scope_space_counter;
    }

void reset_formalarg_offset(){
    formal_arg_offset = 0 ;
}

void reset_functionlocal_offset(){
    function_local_offset = 0 ;
}

void restore_currscope_offset(unsigned n){
    switch(curr_scope_space()){
        case program_var        :   program_var_offset = n ; break;
        case function_local       :   function_local_offset = n ; break;
        case formal_arg         :   formal_arg_offset = n ; break;
        default:
            assert(0);
    }
}



unsigned char check_arithm(expr* e){
    if(e->type == programfunc_e ||
       e->type == libraryfunc_e ||
       e->type == boolexpr_e    ||
       e->type == newtable_e    ||
       e->type == constbool_e   ||
       e->type == conststring_e ||
       e->type == nil_e )
       {
        fprintf(errors,"Syntax error, wrong type, arithmetic expr at line %d\n",yylineno);
        ++syntax_errors_counter;
           return 0;
       }
       return 1 ;
}

unsigned char check_bool(expr* e){
    if(e->type == programfunc_e ||
       e->type == libraryfunc_e ||
       e->type == arithexpr_e    ||
       e->type == newtable_e    ||
       e->type == constnum_e   ||
       e->type == conststring_e ||
       e->type == nil_e )
       {
        fprintf(errors,"Syntax error, wrong type, bool expr at line %d\n",yylineno);
        ++syntax_errors_counter;
           return 0;
       }
       return 1 ;
}

unsigned char typecheck(expr* e1 , expr* e2 , char* op){
    
    if ( !strcmp(op,"incr/decr")){
        assert(e1);
        if( e1->type == constbool_e || e1->type == conststring_e || e1->type == nil_e){
            return 0;
        }
        else{
            return 1;
        }
    }
}


unsigned int is_tempname(char* s){
    return *s == '_';
}

unsigned int is_tempexpr(expr* e){
    return e->sym && is_tempname(e->sym->value->name) ;
}


unsigned curr_scope_offset(){
    switch(curr_scope_space()){
        case program_var    : return program_var_offset;
        case function_local   : return function_local_offset;
        case formal_arg     : return formal_arg_offset;
        default: assert(0);
    }
}

unsigned in_curr_scope_offset(){
    switch(curr_scope_space()){
        case program_var    : ++program_var_offset;break;
        case function_local   : ++function_local_offset;break;
        case formal_arg     : ++formal_arg_offset;break;
    }
}

unsigned next_quad_label(){
    return currQuad   ;
}

list_t* newnode(){
    list_t* n = malloc(sizeof(list_t));
    n->data = 0;
    n->next = 0;
    return n;
}

list_t* merge_list(list_t*l1 , list_t* l2){
    list_t *head , *prev ,*curr;
    
    if(l1 == NULL && l2 == NULL){
        return NULL;
    }
    curr = newnode();
    if(l1 == NULL){
        curr->data == l2->data;
        l2 = l2->next;
    }
    else{
        curr->data = l1->data;
        l1 = l1->next;
    }

    prev = curr;
    head = curr;

    while(l1 != NULL){
        curr = newnode();
        curr->data = l1->data;
        prev->next = curr;
        l1 = l1->next;
    }
    while(l2 != NULL){
        curr = newnode();
        curr->data = l2->data;
        prev->next = curr;
        l2 = l2->next;
    }
    return head;
}

void backpatch(list_t* l1 , unsigned label){
    while(l1 != NULL){
        quads[l1->data].label = label;
        l1 = l1->next;
    }
}


char* newtempname(){
	int i , tmp = tempcounter , count = 0;
	char * name , *num ;
	
	while (tmp != 0) {
		tmp /= 10;    
		++count;
    }
	num = malloc(count * sizeof(char));
	itoa(tempcounter , num);
	name = malloc ( (count + 2 + 1) * sizeof(char) );
	name[0] = '_';
	name[1] = 't';
	for(i = 0 ; i < count ; i ++){
		name[i+2] = num[i];
	}
	name[i+2] = '\0';
	free(num);
	return name ;
}


int newlist(int i){
    quads[i].label = 0;
    return i ;
}

int mergelist(int l1 , int l2){
    if(!l1) 
        return l2 ;
    else if(!l2)
        return l1 ;
    else{
        int i = l1 ;
        while (quads[i].label)
            i = quads[i].label ;
        quads[i].label = l2 ;
        return l1 ;
    }
}

int currscope(){
	return scope ;
}


scopespace_t curr_scope_space(){
    if(scope_space_counter == 1){
        return program_var;
    }else if(scope_space_counter % 2 == 0){
        return formal_arg;
    }else{
        return function_local;
    }
}


symbol* newtemp(){
    symbol* new;
    char* name = newtempname();
    symbol* sym =  lookup_tmp(name , currscope());
    if(sym == NULL){
        tempcounter++ ;
        new = insert_tmp(name);
        new->value->scope = currscope();
        if(currscope() == 0 ){
            new->type1 = GLOBAL ;
        }
        else{
            new->type1 = LOCAL ;
        }
        new->type2 = match_type1_to_type2(new->type1);
        new->offset = curr_scope_offset();
        in_curr_scope_offset();
        new->space = curr_scope_space();
        return new ;
    }
    else{
        return sym ;
    }
}

expr* complete(expr* e1 ){
    expr* tmp;
    // e1->sym = newtemp();
    tmp = newexpr(e1->type);
    tmp->sym = newtemp();
    tmp->truelist = e1->truelist;
    tmp->falselist = e1->falselist;
    tmp->index = e1->index;
    tmp->next = e1->next;
    tmp->notexpr = e1->notexpr;
    tmp->numConst = e1->numConst;
    tmp->strConst = e1->strConst;
    tmp->boolConst = e1->boolConst;
    backpatch(tmp->truelist , next_quad_label());
    emit(assign , tmp , newexpr_constbool(1) , NULL , 0 , yylineno);
    emit(jump , NULL , NULL , NULL , next_quad_label()+2 , yylineno);
    backpatch(tmp->falselist , next_quad_label());
    emit(assign , tmp , newexpr_constbool(0) , NULL , 0 , yylineno);
    return tmp;
}

expr* emit_iftableitem(expr* e){
    expr* result ;
    if(e->type == tableitem_e){
        return e ;
    }
    else{
        result = newexpr(var_e);
        result->sym = newtemp();
        result->index = e->index;
        emit(tablegetelem ,result ,e ,e->index ,0 ,yylineno );
        return result ;
    }
}

expr* reverse_expr(expr* og){

    expr*current = og;
    expr *prev = NULL, *next = NULL;

    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
   }
        og = prev;
        return og ;
} 

expr* newexpr(expr_t t){

    expr* e = (expr*) malloc(sizeof(expr)) ;
    memset(e , 0 , sizeof(expr));
    e->type = t ;
    return e ;
}

expr* newexpr_constnum(int i){
    expr *t = newexpr(constnum_e);
    t->numConst = i;
    return t;
}

expr* newexpr_constbool(int i){
    expr *t = newexpr(constbool_e);
    t->boolConst = !!i;
    return t;
}

expr* newexpr_conststring(char* s){

    expr* e = newexpr(conststring_e) ;
    e->strConst= strdup(s) ;
    return e ;
}

expr* uminus_expr(expr* e){
    expr* n ;

    assert(e) ;

    if(e->type == error_e){
        fprintf(errors,"warning,undefined variable %s ,at line %d (uminus error type)\n",e->sym->value->name,yylineno);// ,e->sym->value->line );
        return e;
    }
    else{
        check_arithm(e);
        n = newexpr(arithexpr_e);
        n->sym = is_tempexpr(e) ? e->sym : newtemp() ;
        // n->numConst = (-1)* e->numConst ;
        emit(uminus , n , e , NULL , 0 , yylineno);
        
        return n;
    }
}

expr* assign_expr(expr* lv , expr* rv){
    expr* result;
    assert(lv);
    assert(rv);
    if(lv->type == error_e){

        fprintf(errors,"warning,undefined variable %s ,at line %d (lvalue error type)\n",lv->sym->value->name,yylineno);// ,lv->sym->value->line );
        return lv; 
    }
    else if (rv->type == error_e ){
        fprintf(errors,"warning,undefined variable %s ,at line %d (rvalue error type)\n",rv->sym->value->name,rv->sym->value->line);
        return lv;
    }
    else if(lv->type == tableitem_e){
        emit(tablesetelem , rv , lv , lv->index , 0 , yylineno);
		lv = emit_iftableitem(lv);
        if(lv->type == tableitem_e){
            result = newexpr(tableitem_e);
            result->sym = newtemp() ;
            emit(tablegetelem,result,lv,lv->index,0,yylineno);
        }
    }
    else{
        
        lv->type = var_e;
        
        emit(assign , lv ,rv ,  NULL , 0 , yylineno);

        result = newexpr(lv->type);
        result->sym = newtemp();
   
        emit(assign , result , lv , NULL , 0 ,yylineno);
    }
    
    return result ;
}

expr* expr_arithm_op_expr(expr* lv , expr* rv , char* op){
    expr* result ;
    assert(lv);
    assert(rv);
    if(lv->type == error_e ){
        fprintf(errors,"Syntax error, try to do %s operation with undefined symbol at line %d\n",op,yylineno);
        ++syntax_errors_counter;
        return lv ;
    }
    else if( rv->type == error_e){
        fprintf(errors,"Syntax error, try to do %s operation with undefined symbol at line %d\n",op,yylineno);
        ++syntax_errors_counter;
        return rv ;
    }
    
    if(check_arithm(lv) && check_arithm(rv) ){
        result = newexpr(arithexpr_e) ; 
        result->sym = newtemp() ;
        if( strcmp(op , "+") == 0 ){
            emit(add , result , lv , rv , 0 , yylineno );
            
        }
        else if(strcmp(op , "-") == 0) {
            emit(sub , result , lv , rv , 0 , yylineno );
            
        }
        else if(strcmp(op , "*") == 0) {
            emit(mul , result , lv , rv , 0 , yylineno );
            
        }
        else if(strcmp(op , "/") == 0) {
            emit(dev , result , lv , rv , 0 , yylineno );
            
        }
        else if(strcmp(op , "%") == 0) {
            emit(mod , result , lv , rv , 0 , yylineno );
            
        }
        else{
            fprintf(errors,"UNDEFINED ARITHM OP ASSERT ,line %d\n" , yylineno);
            assert(0);
        }
    }
    else{
        result = newexpr(error_e) ;
    }   
    return result ; 
    
}

expr* expr_bool_op_expr(expr* lv , expr* rv , char* op , unsigned label){
    expr* result , *tmp ;
    assert(lv);
    assert(rv);
    tmp = malloc(sizeof(expr));

    if(lv->type == error_e ){
        fprintf(errors,"Syntax error,try to do bool op with undefined symbol at line %d\n",yylineno);
        ++syntax_errors_counter;
        return lv ;
    }
    else if( rv->type == error_e){
        fprintf(errors,"Syntax error,try to do bool op with undefined symbol at line %d\n",yylineno);
        ++syntax_errors_counter;
        return rv ;
    }


    if(strcmp(op , "and") == 0 ){
        result = newexpr(boolexpr_e);
        // olikh apotimhsh
        // result->sym = newtemp();
        // result->boolConst = lv->boolConst && rv->boolConst  ;
        // emit(and , result , lv , rv , 0 ,yylineno);

        if(lv->type != boolexpr_e){
            lv->truelist = newnode();
            lv->truelist->data = next_quad_label();
            lv->truelist->next = 0;
            lv->falselist = newnode();
            lv->falselist->data = next_quad_label()+1;
            lv->falselist->next = 0 ;
            emit(if_eq , NULL , lv , newexpr_constbool(1) , 0 , yylineno);
            emit(jump , NULL , NULL , NULL , 0 , yylineno);

            label = next_quad_label();

            if(lv->notexpr){
                tmp->truelist = lv->truelist ;
                tmp->falselist = lv->falselist ;
                lv->truelist = tmp->falselist;
                lv->falselist = tmp->truelist; 
            }
        }
        if(rv->type != boolexpr_e){
            rv->truelist = newnode();
            rv->truelist->data = next_quad_label();
            rv->truelist->next = 0;
            rv->falselist = newnode();
            rv->falselist->data = next_quad_label()+1;
            rv->falselist->next = 0 ;
            emit(if_eq , NULL , rv , newexpr_constbool(1) , 0 , yylineno);
            emit(jump , NULL , NULL , NULL , 0 , yylineno);
            if(rv->notexpr){
                tmp->truelist = rv->truelist ;
                tmp->falselist = rv->falselist ;
                rv->truelist = tmp->falselist;
                rv->falselist = tmp->truelist; 
            }
        }

        backpatch(lv->truelist , label);
        result->falselist = merge_list(lv->falselist , rv->falselist);
        result->truelist = rv->truelist;
    }
    else if( strcmp(op , "or") == 0 ){
        result = newexpr(boolexpr_e);
        
        // olikh apotimhsh
        // result->sym = newtemp();
        // result->boolConst = lv->boolConst && rv->boolConst  ;
        // emit(or , result , lv , rv , 0 ,yylineno);
        
        if(lv->type != boolexpr_e){
            lv->truelist = newnode();
            lv->truelist->data = next_quad_label();
            lv->truelist->next = 0;
            lv->falselist = newnode();
            lv->falselist->data = next_quad_label()+1;
            lv->falselist->next = 0 ;
            emit(if_eq , NULL , lv , newexpr_constbool(1) , 0 , yylineno);
            emit(jump , NULL , NULL , NULL , 0 , yylineno);

            label = next_quad_label();
            
            if(lv->notexpr){
                tmp->truelist = lv->truelist ;
                tmp->falselist = lv->falselist ;
                lv->truelist = tmp->falselist;
                lv->falselist = tmp->truelist; 
            }
        }
        if(rv->type != boolexpr_e){
            rv->truelist = newnode();
            rv->truelist->data = next_quad_label();
            rv->truelist->next = 0;
            rv->falselist = newnode();
            rv->falselist->data = next_quad_label()+1;
            rv->falselist->next = 0 ;
            emit(if_eq , NULL , rv , newexpr_constbool(1) , 0 , yylineno);
            emit(jump , NULL , NULL , NULL , 0 , yylineno);
            if(rv->notexpr){
                tmp->truelist = rv->truelist ;
                tmp->falselist = rv->falselist ;
                rv->truelist = tmp->falselist;
                rv->falselist = tmp->truelist; 
            }
        }
        
        backpatch(lv->falselist , label);
        result->truelist = merge_list(lv->truelist , rv->truelist);
        result->falselist = rv->falselist;

    }
    else{
        fprintf(errors,"UNDEFINED BOOL OP ASSERT ,line %d\n",yylineno);
        assert(0);
    }
 
    return result;
}

expr* expr_rel_op_expr(expr* lv , expr* rv , char* op){
    expr* result ;
    assert(lv);
    assert(rv);
    if(lv->type == error_e ){
        fprintf(errors,"Syntax error,try to do %s with undefined symbol at line %d\n",op,yylineno);
        ++syntax_errors_counter;
        return lv ;
    }
    else if( rv->type == error_e){
        fprintf(errors,"Syntax error,try to do %s with undefined symbol at line %d\n",op,yylineno);
        ++syntax_errors_counter;
        return rv ;
    }
    if(strcmp(op , "==") == 0){
       
    
        result = newexpr(boolexpr_e);
        result->sym = newtemp();
        
        // olikh apotimhsh

        // emit(if_eq , NULL , lv , rv , next_quad_label() + 3 , yylineno );
        // emit(assign , result , newexpr_constbool(0) , NULL , 0 , yylineno );
        // emit(jump , NULL , NULL , NULL , next_quad_label() + 2 , yylineno);
        // emit(assign , result , newexpr_constbool(1) , NULL , 0 , yylineno );
        
        result->truelist = newnode();
        result->truelist->data = next_quad_label();
        result->truelist->next = 0;
        result->falselist = newnode();
        result->falselist->data = next_quad_label()+1;
        result->falselist->next = 0;

        emit(if_eq , NULL , lv , rv , 0 , yylineno);
        emit(jump , NULL , NULL , NULL , 0 , yylineno);
    
    }
    else if(strcmp(op , "!=") == 0){
        
       
        result = newexpr(boolexpr_e);
        result->sym = newtemp();

        // olikh apotimhsh

        // emit(if_noteq , NULL , lv , rv , next_quad_label() + 3 , yylineno );
        // emit(assign , result , newexpr_constbool(0) , NULL , 0 , yylineno );
        // emit(jump , NULL , NULL , NULL , next_quad_label() + 2 , yylineno);
        // emit(assign , result , newexpr_constbool(1) , NULL , 0 , yylineno );   
        
        result->truelist = newnode();
        result->truelist->data = next_quad_label();
        result->truelist->next = 0;
        result->falselist = newnode();
        result->falselist->data = next_quad_label()+1;
        result->falselist->next = 0;
        
        emit(if_noteq , NULL , lv , rv , 0 , yylineno);
        emit(jump , NULL , NULL , NULL , 0 , yylineno);
    
    }
    else if(strcmp(op , "<") == 0){
        if(check_arithm(lv) && check_arithm(rv)){
            result = newexpr(boolexpr_e);
            result->sym = newtemp();
            // result->boolConst = lv->numConst < rv->numConst ;
            // emit(if_less , result, lv , rv , next_quad_label()+3 , yylineno);
            // emit(assign ,result , newexpr_constbool(0) , NULL , 0 , yylineno);
            // emit(jump ,NULL , NULL , NULL , next_quad_label()+2 , yylineno );
            // emit(assign , result , newexpr_constbool(1) , NULL , 0 , yylineno);
            
            result->truelist = newnode();
            result->truelist->data = next_quad_label();
            result->truelist->next = 0;
            result->falselist = newnode();
            result->falselist->data = next_quad_label()+1;
            result->falselist->next = 0;
           
           emit(if_less , result , lv , rv , 0 , yylineno);
           emit(jump , NULL , NULL , NULL , 0 , yylineno);
        }
        else{
            fprintf(errors,"Syntax error, < operation with wrong type of expressions,at line %d\n",yylineno);
            ++syntax_errors_counter;
            result = newexpr(error_e) ;
        }  
    }
    else if(strcmp(op , "<=") == 0){
        if(check_arithm(lv) && check_arithm(rv)){
            result = newexpr(boolexpr_e);
            result->sym = newtemp();
            // result->boolConst = lv->numConst <= rv->numConst ;
            // emit(if_lesseq , result, lv , rv , next_quad_label()+3 , yylineno);
            // emit(assign ,result , newexpr_constbool(0) , NULL , 0 , yylineno);
            // emit(jump ,NULL , NULL , NULL , next_quad_label()+2 , yylineno );
            // emit(assign , result , newexpr_constbool(1) , NULL , 0 , yylineno);
            
            result->truelist = newnode();
            result->truelist->data = next_quad_label();
            result->truelist->next = 0;
            result->falselist = newnode();
            result->falselist->data = next_quad_label()+1;
            result->falselist->next = 0;

           emit(if_lesseq , result , lv , rv , 0 , yylineno);
           emit(jump , NULL , NULL , NULL , 0 , yylineno);
        }
        else{
            fprintf(errors,"Syntax error, <= operation with wrong type of expressions,at line %d\n",yylineno);
            ++syntax_errors_counter;
            result = newexpr(error_e) ;
        }  
    }
    else if(strcmp(op , ">") == 0){
        if(check_arithm(lv) && check_arithm(rv)){
            result = newexpr(boolexpr_e);
            result->sym = newtemp();
            // result->boolConst = lv->numConst > rv->numConst ;
            // emit(if_greater , result, lv , rv , next_quad_label()+3 , yylineno);
            // emit(assign ,result , newexpr_constbool(0) , NULL , 0 , yylineno);
            // emit(jump ,NULL , NULL , NULL , next_quad_label()+2 , yylineno );
            // emit(assign , result , newexpr_constbool(1) , NULL , 0 , yylineno);
            
            result->truelist = newnode();
            result->truelist->data = next_quad_label();
            result->truelist->next = 0;
            result->falselist = newnode();
            result->falselist->data = next_quad_label()+1;
            result->falselist->next = 0;
           
           emit(if_greater , result , lv , rv , 0 , yylineno);
           emit(jump , NULL , NULL , NULL , 0 , yylineno);
        }
        else{
            fprintf(errors,"Syntax error, > operation with wrong type of expressions,at line %d\n",yylineno);
            ++syntax_errors_counter;
            result = newexpr(error_e) ;
        }  
    }
    else if(strcmp(op , ">=") == 0){
        if(check_arithm(lv) && check_arithm(rv)){
            result = newexpr(boolexpr_e);
            result->sym = newtemp();
            // result->boolConst = lv->numConst >= rv->numConst ;

            // result->boolConst = lv->numConst >= rv->numConst ;
            // emit(if_greatereq , result, lv , rv , next_quad_label()+3 , yylineno);
            // emit(assign ,result , newexpr_constbool(0) , NULL , 0 , yylineno);
            // emit(jump ,NULL , NULL , NULL , next_quad_label()+2 , yylineno );
            // emit(assign , result , newexpr_constbool(1) , NULL , 0 , yylineno);
            
            result->truelist = newnode();
            result->truelist->data = next_quad_label();
            result->truelist->next = 0;
            result->falselist = newnode();
            result->falselist->data = next_quad_label()+1;
            result->falselist->next = 0;
           
           emit(if_greatereq , result , lv , rv , 0 , yylineno);
           emit(jump , NULL , NULL , NULL , 0 , yylineno);
        }
        else{
            fprintf(errors,"Syntax error, >= operation with wrong type of expressions,at line %d\n",yylineno);
            ++syntax_errors_counter;
            result = newexpr(error_e) ;
        }  
    }
    else{
        fprintf(errors,"UNDEFINED REL OP ASSERT ,line %d\n",yylineno);
        assert(0);
    }   
    return result ; 
} 

expr* lvalue_expr(symbol* sym){
    expr* e;
    assert(sym);
    
    e = (expr*) malloc(sizeof(expr));

    memset(e,0,sizeof(expr));
    e->next = NULL ;
    e->sym = sym ;
    switch(sym->type2){

        case var_s:    e->type = var_e; break ;
        case programfunc_s:  e->type = programfunc_e; break ;
        case libraryfunc_s:   e->type = libraryfunc_e; break ;
        case error_s:     e->type = error_e; break;
        default:        assert(0);
    }
    return e ;
}

expr* term_is_member(expr* lv){
    expr* result ;
    
    lv = emit_iftableitem(lv);
    if(lv->type == tableitem_e){
        result = newexpr(tableitem_e);
        result->sym = newtemp() ;
        emit(tablegetelem,result,lv,lv->index,0,yylineno);
    }
    else{
        result = newexpr(tableitem_e);
        result->sym = lv->sym ;

    }
    return result;
}

expr* primary_lval_is_member(expr* lv){
    expr* result ;

    lv = emit_iftableitem(lv);
    if(lv->type == tableitem_e){
        result = newexpr(tableitem_e);
        result->sym = newtemp() ;
        emit(tablegetelem,result,lv,lv->index,0,yylineno);
    }
    else{
        result = newexpr(tableitem_e);
        result->sym = lv->sym ;

    }
    return result;

}

expr* member_item_dot(expr* lv , char* name){
    expr* ti = NULL ;
      
   if(lv->index != NULL){
        lv = emit_iftableitem(lv);
        if(lv->type == tableitem_e){
            ti = newexpr(tableitem_e);
            ti->sym = newtemp();
            ti->index = newexpr_conststring(name);
            emit(tablegetelem,ti,lv,lv->index,0,yylineno);
        }else{
            ti = newexpr(tableitem_e);
            ti->sym = lv->sym ;
            ti->index = newexpr_conststring(name);
        }
   }
   else{
       ti= lv ;
       ti->index = newexpr_conststring(name);
   }
   
   return ti;
}

expr* member_item_brack(expr* lv , expr* el){
    expr* res ;
    if(lv->index != NULL){
        lv = emit_iftableitem(lv);
        if(lv->type == tableitem_e){
            res = newexpr(tableitem_e);
            res->sym = newtemp() ;	
            res->index = el ;
            emit(tablegetelem , res , lv , lv->index , 0 , yylineno);
        }
        else{
            res = newexpr(tableitem_e);
            res->sym = lv->sym;
            res->index = el ;
        }
    }
    else{
        res = lv;
        res->index = el;    
    }
    return res ;
}

expr* make_call(expr*lv , expr* list){
    expr* func = lv , *result ;
    symtype_to_expr_type(func);
    list = reverse_expr(list);

    if(lv->index != NULL){
        func = emit_iftableitem(lv);
    }
    while(list != NULL){
        emit(param , NULL , list , NULL , 0 , yylineno);
        list = list->next ;
    }
    emit(call , NULL , func , NULL , 0 , yylineno);
    result = newexpr(var_e);
    result->sym = newtemp();
    emit(getretval , NULL , result , NULL , 0 , yylineno );
    
    return result ; 
}

expr* call_first(expr* cal , expr* el){
    assert(cal);
    return make_call(cal , el);
}

expr* call_second(expr* lv , call_s* cs){
    expr* tmp1 = NULL , *t2 ;

    assert(lv);
    assert(cs);

    if(cs->method == 1 ){
        tmp1 = lv ;
        if(lv->index == NULL){
            lv = member_item_dot(lv , cs->name);
            if(cs->elist == NULL){
                cs->elist = lv ;
            }else{
                lv->next = cs->elist ;
                cs->elist = lv ;
            }
        }
        else{
            lv = member_item_dot(lv , cs->name);
            if(cs->elist == NULL){
                cs->elist = lv ;
            }else{
                lv->next = cs->elist ;
                cs->elist = lv ;
            }
            tmp1 = newexpr(tableitem_e);
            tmp1->sym = newtemp();
            emit(tablegetelem , tmp1 , lv , lv->index , 0 , yylineno);
            lv = tmp1;
        }
    }
    else{
        if(lv->index != NULL){
            tmp1 = newexpr(tableitem_e);
            tmp1->sym = newtemp();
            emit(tablegetelem , tmp1 , lv , lv->index , 0 , yylineno);
            lv = tmp1;
        }
    }
    return make_call(lv , cs->elist);
}

expr* call_third(symbol* func , expr* el){
    expr* f ;
    assert(func);
    
    f = newexpr(programfunc_e);
    f->sym = func ;
    return make_call(f , el);
}

expr* objectdef_elist(expr* el){
    int i;
    expr* t = newexpr(newtable_e);
    t->sym = newtemp();

    emit(tablecreate , NULL , t , NULL , 0 , yylineno);
    for(i = 0 ; el ; el = el->next){
        //i for indexing
        emit(tablesetelem , el , t , newexpr_constnum(i++) , 0 , yylineno) ;
    }
    return t;
}

expr* objectdef_indexed(index_set* in){
    int i ;
    index_set* tmp = in ;
    expr* t = newexpr(newtable_e);

    t->sym = newtemp();
    emit(tablecreate , NULL , t , NULL , 0 , yylineno);

    while(tmp != NULL){
        emit(tablesetelem , tmp->value , t , tmp->index , 0 , yylineno);
        tmp = tmp->next;
    }
    return t;
}

expr* symtype_to_expr_type(expr* e){
    switch (e->sym->type1)
    {
    case LIBFUNC:
        e->type = libraryfunc_e;
        break;
    case USERFUNC:
        e->type = programfunc_e;
    default:
        break;
    }
}