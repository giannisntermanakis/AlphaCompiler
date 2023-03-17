#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../headerfiles/tokens.h"
#include "../headerfiles/i_code_datatypes.h"
#include "../headerfiles/symtable.h"
#include "../headerfiles/i_code_functions.h"
extern FILE* quadfile , *errors;

void print_details(expr* arg1 , expr* arg2 , expr* result , iopcode op  , unsigned label , int line){

    switch (op)
    {
    case assign :
        print_assign(arg1 , result , line );
        break;
    case add :
        print_add(arg1 , arg2 , result , line);
        break;
    case sub :
        print_sub(arg1 , arg2 , result , line);
        break;
    case mul :
        print_mul(arg1 , arg2 , result , line);
        break;
    case dev :
        print_div(arg1 , arg2 , result , line);
        break;
    case mod :
        print_mod(arg1 , arg2 , result , line);
        break;
    case not :
        print_not(arg1 ,result , line);
        break;
    case tablegetelem :
        print_getelem(arg1 , arg2 , result , line);
        break;
    case tablesetelem :
        print_setelem(arg1,arg2,result , line);
        break;
    case param :
        print_param(arg1 , line);
        break;
    case call :
        fprintf(quadfile,"CALL %s\t[line: %d]\n",arg1->sym->value->name, line);
        break;
    case getretval :
        fprintf(quadfile,"GETRETVAL %s\t[line: %d]\n",arg1->sym->value->name, line);
        break;
    case uminus :
        print_uminus(arg1 , result , line);
        break;
    case tablecreate :
        fprintf(quadfile , "TABLECREATE %s\t[line: %d]\n",arg1->sym->value->name, line);
        break;
    case funcstart:
        fprintf(quadfile , "FUNCSTART %s\t[line: %d]\n",result->sym->value->name, line);
        break;
    case funcend:
        fprintf(quadfile , "FUNCEND %s\t[line: %d]\n",result->sym->value->name, line);
        break;
    case jump :
        fprintf(quadfile , "JUMP %d\t[line: %d]\n",label, line);
        break;
    case if_eq :
        print_eq(arg1 , arg2 , label , line);
        break;
    case if_noteq :
        print_noteq(arg1 , arg2 , label , line);
        break;
    case if_less :
        print_less(arg1 , arg2 , label , line);
        break;
    case if_lesseq :
        print_lesseq(arg1 , arg2 , label , line);
        break;
    case if_greater :
        print_greater(arg1 , arg2 , label , line);
        break;
    case if_greatereq :
        print_greatereq(arg1 , arg2 , label , line);
        break;
    case and:
        print_and(arg1 , arg2 , result , line);
        break;
    case or :
        print_or(arg1 , arg2 , result , line);
        break;
    case ret:
        print_return(arg1 , line);
    default:
        break;
    }
}

void print_assign(expr* source , expr* result , int line){
    if(source->type == constnum_e ){
        if(source->sym != NULL){
            fprintf(quadfile,"ASSIGN,  %s ,  %s\t[line: %d]\n", result->sym->value->name,source->sym->value->name, line);
        }else{
            fprintf(quadfile,"ASSIGN,  %s ,  %f\t[line: %d]\n", result->sym->value->name,source->numConst, line);
        }
    }
    else if(source->type == conststring_e){
        if(source->sym != NULL){
            fprintf(quadfile,"ASSIGN,  %s ,  %s\t[line: %d]\n", result->sym->value->name,source->sym->value->name, line);
        }else{
            fprintf(quadfile,"ASSIGN,  %s ,  %s\t[line: %d]\n", result->sym->value->name,source->strConst, line);
        }
    }
    else if(source->type == constbool_e){
        if(source->boolConst == 0 ){
            if(source->sym != NULL){
                fprintf(quadfile,"ASSIGN,  %s ,  %s\t[line: %d]\n", result->sym->value->name,source->sym->value->name, line);
            }else{
                fprintf(quadfile,"ASSIGN,  %s ,  FALSE\t[line: %d]\n", result->sym->value->name, line);
            }
        }
        else{
            if(source->sym != NULL){
                fprintf(quadfile,"ASSIGN,  %s ,  %s\t[line: %d]\n", result->sym->value->name,source->sym->value->name, line);
            }else{
                fprintf(quadfile,"ASSIGN,  %s ,  TRUE\t[line: %d]\n", result->sym->value->name, line);
            }
        }
    }
    else if(source->type == nil_e){
        if(source->sym != NULL){
            fprintf(quadfile,"ASSIGN,  %s ,  %s\t[line: %d]\n", result->sym->value->name,source->sym->value->name, line);
        }else{
            fprintf(quadfile,"ASSIGN,  %s ,  NIL\t[line: %d]\n", result->sym->value->name, line);
        }
    }
    else {
        fprintf(quadfile,"ASSIGN,  %s ,  %s\t[line: %d]\n", result->sym->value->name,source->sym->value->name, line);
    }
}
void print_add(expr*arg1 , expr* arg2 , expr* result , int line){

    if(arg1->type == constnum_e && arg2->type == constnum_e )
    {
        fprintf(quadfile,"ADD,  %s ,  %f , %f\t[line: %d]\n", result->sym->value->name,arg1->numConst,arg2->numConst, line);
    }
    else if(arg1->type == constnum_e && arg2->type != constnum_e )
    {
        fprintf(quadfile,"ADD,  %s ,  %f , %s\t[line: %d]\n", result->sym->value->name,arg1->numConst,arg2->sym->value->name, line);
    }
    else if(arg1->type != constnum_e && arg2->type == constnum_e )
    {
        fprintf(quadfile,"ADD,  %s ,  %s , %f\t[line: %d]\n", result->sym->value->name,arg1->sym->value->name,arg2->numConst, line);
    }
    else if(arg1->type != constnum_e && arg2->type != constnum_e)
    {
        fprintf(quadfile,"ADD,  %s ,  %s , %s\t[line: %d]\n", result->sym->value->name,arg1->sym->value->name,arg2->sym->value->name, line);
    }

}
void print_sub(expr*arg1 , expr* arg2 , expr* result , int line){

    if(arg1->type == constnum_e && arg2->type == constnum_e )
    {
        fprintf(quadfile,"SUB,  %s ,  %f , %f\t[line: %d]\n", result->sym->value->name,arg1->numConst,arg2->numConst, line);
    }
    else if(arg1->type == constnum_e && arg2->type != constnum_e )
    {
        fprintf(quadfile,"SUB,  %s ,  %f , %s\t[line: %d]\n", result->sym->value->name,arg1->numConst,arg2->sym->value->name, line);
    }
    else if(arg1->type != constnum_e && arg2->type == constnum_e )
    {
        fprintf(quadfile,"SUB,  %s ,  %s , %f\t[line: %d]\n", result->sym->value->name,arg1->sym->value->name,arg2->numConst, line);
    }
    else if(arg1->type != constnum_e && arg2->type != constnum_e)
    {
        fprintf(quadfile,"SUB,  %s ,  %s , %s\t[line: %d]\n", result->sym->value->name,arg1->sym->value->name,arg2->sym->value->name, line);
    }

}
void print_div(expr*arg1 , expr* arg2 , expr* result , int line){

    if(arg1->type == constnum_e && arg2->type == constnum_e )
    {
        fprintf(quadfile,"DIV,  %s ,  %f , %f\t[line: %d]\n", result->sym->value->name,arg1->numConst,arg2->numConst, line);
    }
    else if(arg1->type == constnum_e && arg2->type != constnum_e )
    {
        fprintf(quadfile,"DIV,  %s ,  %f , %s\t[line: %d]\n", result->sym->value->name,arg1->numConst,arg2->sym->value->name, line);
    }
    else if(arg1->type != constnum_e && arg2->type == constnum_e )
    {
        fprintf(quadfile,"DIV,  %s ,  %s , %f\t[line: %d]\n", result->sym->value->name,arg1->sym->value->name,arg2->numConst, line);
    }
    else if(arg1->type != constnum_e && arg2->type != constnum_e)
    {
        fprintf(quadfile,"DIV,  %s ,  %s , %s\t[line: %d]\n", result->sym->value->name,arg1->sym->value->name,arg2->sym->value->name, line);
    }

}
void print_mod(expr*arg1 , expr* arg2 , expr* result , int line){

    if(arg1->type == constnum_e && arg2->type == constnum_e )
    {
        fprintf(quadfile,"MOD,  %s ,  %f , %f\t[line: %d]\n", result->sym->value->name,arg1->numConst,arg2->numConst, line);
    }
    else if(arg1->type == constnum_e && arg2->type != constnum_e )
    {
        fprintf(quadfile,"MOD,  %s ,  %f , %s\t[line: %d]\n", result->sym->value->name,arg1->numConst,arg2->sym->value->name, line);
    }
    else if(arg1->type != constnum_e && arg2->type == constnum_e )
    {
        fprintf(quadfile,"MOD,  %s ,  %s , %f\t[line: %d]\n", result->sym->value->name,arg1->sym->value->name,arg2->numConst, line);
    }
    else if(arg1->type != constnum_e && arg2->type != constnum_e)
    {
        fprintf(quadfile,"MOD,  %s ,  %s , %s\t[line: %d]\n", result->sym->value->name,arg1->sym->value->name,arg2->sym->value->name, line);
    }

}
void print_mul(expr*arg1 , expr* arg2 , expr* result , int line){

    if(arg1->type == constnum_e && arg2->type == constnum_e )
    {
        fprintf(quadfile,"MUL,  %s ,  %f , %f\t[line: %d]\n", result->sym->value->name,arg1->numConst,arg2->numConst, line);
    }
    else if(arg1->type == constnum_e && arg2->type != constnum_e )
    {
        fprintf(quadfile,"MUL,  %s ,  %f , %s\t[line: %d]\n", result->sym->value->name,arg1->numConst,arg2->sym->value->name, line);
    }
    else if(arg1->type != constnum_e && arg2->type == constnum_e )
    {
        fprintf(quadfile,"MUL,  %s ,  %s , %f\t[line: %d]\n", result->sym->value->name,arg1->sym->value->name,arg2->numConst, line);
    }
    else if(arg1->type != constnum_e && arg2->type != constnum_e)
    {
        fprintf(quadfile,"MUL,  %s ,  %s , %s\t[line: %d]\n", result->sym->value->name,arg1->sym->value->name,arg2->sym->value->name, line);
    }

}
void print_uminus(expr* arg1 ,expr* result , int line){

    if(arg1->sym == NULL && arg1->type == constnum_e)
    {
        fprintf(quadfile , "UMINUS , %s , %f\t[line: %d]\n",result->sym->value->name , arg1->numConst, line);
    }
    else if(arg1->sym != NULL)
    {
        fprintf(quadfile , "UMINUS , %s , %s\t[line: %d]\n",result->sym->value->name , arg1->sym->value->name, line);
    }
}

void print_not(expr* arg1 , expr* result , int line){
    if(arg1->type == constbool_e)
    {
        if(arg1->boolConst == 0 ){
            fprintf(quadfile , "NOT,\t%s,\tFALSE\t[line: %d]\n",result->sym->value->name , line);
        }
        else {
            fprintf(quadfile , "NOT,\t%s,\tTRUE\t[line: %d]\n",result->sym->value->name , line);
        }
    }
    else if(arg1->sym != NULL)
    {
        fprintf(quadfile , "NOT,\t%s,\t%s\t[line: %d]\n",result->sym->value->name , arg1->sym->value->name, line);
    }
}

void print_less(expr* arg1, expr* arg2, unsigned label , int line){
    fprintf(quadfile,"IF_LESS,\t");

    switch (arg1->type)
    {
    case constnum_e:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        else{
            fprintf(quadfile,"%f,\t",arg1->numConst);
        }
        break;
    default:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        break;
    }

    switch (arg2->type)
    {
    case constnum_e:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        else{
            fprintf(quadfile,"%f,\t",arg2->numConst);
        }
        break;
    default:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        break;
    }


    fprintf(quadfile,"%d,\t[line:%d]\n",label ,line );
}
void print_lesseq(expr* arg1, expr* arg2, unsigned label , int line){
    fprintf(quadfile,"IF_LESSEQ,\t");

    switch (arg1->type)
    {
    case constnum_e:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        else{
            fprintf(quadfile,"%f,\t",arg1->numConst);
        }
        break;
    default:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        break;
    }

    switch (arg2->type)
    {
    case constnum_e:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        else{
            fprintf(quadfile,"%f,\t",arg2->numConst);
        }
        break;
    default:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        break;
    }


    fprintf(quadfile,"%d,\t[line:%d]\n",label ,line );
}
void print_greater(expr* arg1, expr* arg2, unsigned label , int line){
    fprintf(quadfile,"IF_GREATER,\t");

    switch (arg1->type)
    {
    case constnum_e:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        else{
            fprintf(quadfile,"%f,\t",arg1->numConst);
        }
        break;
    default:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        break;
    }

    switch (arg2->type)
    {
    case constnum_e:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        else{
            fprintf(quadfile,"%f,\t",arg2->numConst);
        }
        break;
    default:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        break;
    }


    fprintf(quadfile,"%d,\t[line:%d]\n",label ,line );
}
void print_greatereq(expr* arg1, expr* arg2, unsigned label , int line){
    fprintf(quadfile,"IF_GREATEREQ,\t");

    switch (arg1->type)
    {
    case constnum_e:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        else{
            fprintf(quadfile,"%f,\t",arg1->numConst);
        }
        break;
    default:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        break;
    }

    switch (arg2->type)
    {
    case constnum_e:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        else{
            fprintf(quadfile,"%f,\t",arg2->numConst);
        }
        break;
    default:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        break;
    }


    fprintf(quadfile,"%d,\t[line:%d]\n",label ,line );
}
void print_eq(expr* arg1, expr* arg2, unsigned label, int line){

    fprintf(quadfile,"IF_EQUAL,\t");

    switch (arg1->type)
    {
    case constnum_e:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        else{
            fprintf(quadfile,"%f,\t",arg1->numConst);
        }
        break;
    case constbool_e:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        else{
            if(arg1->boolConst == 0){
                fprintf(quadfile,"FALSE,\t");
            }
            else{
                fprintf(quadfile,"TRUE,\t");
            }
        }
        break;
    case conststring_e:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        else{
            fprintf(quadfile,"%s,\t",arg1->strConst);
        }
        break;
    case nil_e:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        else{
            fprintf(quadfile,"NIL,\t");
        }
        break;
    default:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        break;
    }

    switch (arg2->type)
    {
    case constnum_e:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        else{
            fprintf(quadfile,"%f,\t",arg2->numConst);
        }
        break;
    case constbool_e:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        else{
            if(arg2->boolConst == 0){
                fprintf(quadfile,"FALSE,\t");
            }
            else{
                fprintf(quadfile,"TRUE,\t");
            }
        }
        break;
    case conststring_e:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        else{
            fprintf(quadfile,"%s,\t",arg2->strConst);
        }
        break;
    case nil_e:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        else{
            fprintf(quadfile,"NIL,\t");
        }
        break;
    default:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        break;
    }


    fprintf(quadfile,"%d,\t[line:%d]\n",label ,line );





}


//olikh apotimhsh
void print_and(expr*arg1 , expr* arg2 , expr* result, int line){

    if(arg1->type == constbool_e && arg2->type == constbool_e )
    {

        if(arg1->boolConst == 0 && arg2->boolConst == 0){
            fprintf(quadfile,"AND, %s , FALSE ,  FALSE \t[line: %d]\n",result->sym->value->name, line);
        }
        else if(arg1->boolConst == 0 && arg2->boolConst != 0){
            fprintf(quadfile,"AND, %s , FALSE ,  TRUE \t[line: %d]\n",result->sym->value->name, line);
        }
        else if(arg1->boolConst != 0 && arg2->boolConst == 0){
            fprintf(quadfile,"AND, %s , TRUE ,  FALSE \t[line: %d]\n",result->sym->value->name, line);
        }
        else if(arg1->boolConst != 0 && arg2->boolConst != 0){
            fprintf(quadfile,"AND, %s , TRUE ,  TRUE \t[line: %d]\n",result->sym->value->name, line);
        }
    }
    else if(arg1->type == constbool_e && arg2->type != constbool_e )
    {
        if(arg1->boolConst == 0 ){
            fprintf(quadfile,"AND, %s , FALSE , %s \t[line: %d]\n",result->sym->value->name,arg2->sym->value->name, line);
        }
        else {
            fprintf(quadfile,"AND, %s , TRUE , %s \t[line: %d]\n",result->sym->value->name,arg2->sym->value->name, line);
        }
    }
    else if(arg1->type != constbool_e && arg2->type == constbool_e )
    {
        if(arg2->boolConst == 0 ){
            fprintf(quadfile,"AND, %s , %s , FALSE \t[line: %d]\n",result->sym->value->name,arg1->sym->value->name, line);
        }
        else {
            fprintf(quadfile,"AND, %s , %s , TRUE \t[line: %d]\n",result->sym->value->name,arg1->sym->value->name, line);
        }    }
    else if(arg1->type != constbool_e && arg2->type != constbool_e)
    {
        fprintf(quadfile,"AND,  %s ,  %s , %s\t[line: %d]\n",result->sym->value->name ,arg1->sym->value->name,arg2->sym->value->name, line);
    }

}
//olikh apotimhsh
void print_or(expr*arg1 , expr* arg2 , expr* result, int line){

    if(arg1->type == constbool_e && arg2->type == constbool_e )
    {

        if(arg1->boolConst == 0 && arg2->boolConst == 0){
            fprintf(quadfile,"OR, %s , FALSE ,  FALSE \t[line: %d]\n",result->sym->value->name, line);
        }
        else if(arg1->boolConst == 0 && arg2->boolConst != 0){
            fprintf(quadfile,"OR, %s , FALSE ,  TRUE \t[line: %d]\n",result->sym->value->name, line);
        }
        else if(arg1->boolConst != 0 && arg2->boolConst == 0){
            fprintf(quadfile,"OR, %s , TRUE ,  FALSE \t[line: %d]\n",result->sym->value->name, line);
        }
        else if(arg1->boolConst != 0 && arg2->boolConst != 0){
            fprintf(quadfile,"OR, %s , TRUE ,  TRUE \t[line: %d]\n",result->sym->value->name, line);
        }
    }
    else if(arg1->type == constbool_e && arg2->type != constbool_e )
    {
        if(arg1->boolConst == 0 ){
            fprintf(quadfile,"OR, %s , FALSE , %s \t[line: %d]\n",result->sym->value->name,arg2->sym->value->name, line);
        }
        else {
            fprintf(quadfile,"OR, %s , TRUE , %s \t[line: %d]\n",result->sym->value->name,arg2->sym->value->name, line);
        }
    }
    else if(arg1->type != constbool_e && arg2->type == constbool_e )
    {
        if(arg2->boolConst == 0 ){
            fprintf(quadfile,"OR, %s , %s , FALSE \t[line: %d]\n",result->sym->value->name,arg1->sym->value->name, line);
        }
        else {
            fprintf(quadfile,"OR, %s , %s , TRUE \t[line: %d]\n",result->sym->value->name,arg1->sym->value->name, line);
        }    }
    else if(arg1->type != constbool_e && arg2->type != constbool_e)
    {
        fprintf(quadfile,"OR,  %s ,  %s , %s\t[line: %d]\n",result->sym->value->name ,arg1->sym->value->name,arg2->sym->value->name, line);
    }
}
void print_noteq(expr* arg1, expr* arg2, unsigned label, int line){


    fprintf(quadfile,"IF_NOTEQ,\t");
    switch (arg1->type)
    {
    case constnum_e:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        else{
            fprintf(quadfile,"%f,\t",arg1->numConst);
        }
        break;
    case constbool_e:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        else{
            if(arg1->boolConst == 0){
                fprintf(quadfile,"FALSE,\t");
            }
            else{
                fprintf(quadfile,"TRUE,\t");
            }
        }
        break;
    case conststring_e:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        else{
            fprintf(quadfile,"%s,\t",arg1->strConst);
        }
        break;
    case nil_e:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        else{
            fprintf(quadfile,"NIL,\t");
        }
        break;
    default:
        if(arg1->sym != NULL){
            fprintf(quadfile,"%s,\t",arg1->sym->value->name);
        }
        break;
    }
switch (arg2->type)
    {
    case constnum_e:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        else{
            fprintf(quadfile,"%f,\t",arg2->numConst);
        }
        break;
    case constbool_e:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        else{
            if(arg2->boolConst == 0){
                fprintf(quadfile,"FALSE,\t");
            }
            else{
                fprintf(quadfile,"TRUE,\t");
            }
        }
        break;
    case conststring_e:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        else{
            fprintf(quadfile,"%s,\t",arg2->strConst);
        }
        break;
    case nil_e:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        else{
            fprintf(quadfile,"NIL,\t");
        }
        break;
    default:
        if(arg2->sym != NULL){
            fprintf(quadfile,"%s,\t",arg2->sym->value->name);
        }
        break;
    }

    fprintf(quadfile,"%d,\t[line:%d]\n",label ,line );

}
void print_getelem(expr* arg1 , expr* arg2 , expr* result, int line){
    fprintf(quadfile,"TABLE_SET_ELEM,\t");
    switch(result->type){
        case constnum_e: fprintf(quadfile,"%f\t",result->numConst);break;
        case constbool_e:if(result->boolConst == 0) fprintf(quadfile,"false\t");
                        else fprintf(quadfile,"true\t");
                        break;
        case conststring_e: fprintf(quadfile,"%s\t",result->strConst);break;
        case nil_e:fprintf(quadfile,"nil\t");break;
        default:fprintf(quadfile,"%s\t",result->sym->value->name);break;
    }
    switch(arg1->type){
        case constnum_e: fprintf(quadfile,"%f\t",arg1->numConst);break;
        case constbool_e:if(arg1->boolConst == 0) fprintf(quadfile,"false\t");
                        else fprintf(quadfile,"true\t");
                        break;
        case conststring_e: fprintf(quadfile,"%s\t",arg1->strConst);break;
        case nil_e:fprintf(quadfile,"nil\t");break;
        default:fprintf(quadfile,"%s\t",arg1->sym->value->name);break;
    }
    switch(arg2->type){
        case constnum_e: fprintf(quadfile,"%f\t",arg2->numConst);break;
        case constbool_e:if(arg2->boolConst == 0) fprintf(quadfile,"false\t");
                        else fprintf(quadfile,"true\t");
                        break;
        case conststring_e: fprintf(quadfile,"%s\t",arg2->strConst);break;
        case nil_e:fprintf(quadfile,"nil\t");break;
        default:fprintf(quadfile,"%s\t",arg2->sym->value->name);break;
    }
    fprintf(quadfile,"line: %d\n",line);
}
void print_setelem(expr* arg1 , expr* arg2 , expr* result, int line){
    // printf("types res[%d] 1[%d] 2[%d]\n",result->type , arg1->type , arg2->type);
    fprintf(quadfile,"TABLE_SET_ELEM,\t");
    switch(result->type){
        case constnum_e: fprintf(quadfile,"%f\t",result->numConst);break;
        case constbool_e:if(result->boolConst == 0) fprintf(quadfile,"false\t");
                        else fprintf(quadfile,"true\t");
                        break;
        case conststring_e: fprintf(quadfile,"%s\t",result->strConst);break;
        case nil_e:fprintf(quadfile,"nil\t");break;
        default:fprintf(quadfile,"%s\t",result->sym->value->name);break;
    }
    switch(arg1->type){
        case constnum_e: fprintf(quadfile,"%f\t",arg1->numConst);break;
        case constbool_e:if(arg1->boolConst == 0) fprintf(quadfile,"false\t");
                        else fprintf(quadfile,"true\t");
                        break;
        case conststring_e: fprintf(quadfile,"%s\t",arg1->strConst);break;
        case nil_e:fprintf(quadfile,"nil\t");break;
        default:fprintf(quadfile,"%s\t",arg1->sym->value->name);break;
    }
    switch(arg2->type){
        case constnum_e: fprintf(quadfile,"%f\t",arg2->numConst);break;
        case constbool_e:if(arg2->boolConst == 0) fprintf(quadfile,"false\t");
                        else fprintf(quadfile,"true\t");
                        break;
        case conststring_e: fprintf(quadfile,"%s\t",arg2->strConst);break;
        case nil_e:fprintf(quadfile,"nil\t");break;
        default:fprintf(quadfile,"%s\t",arg2->sym->value->name);break;
    }
    fprintf(quadfile,"line: %d\n",line);
    
}
void print_param(expr* arg1, int line){
    if(arg1->sym == NULL){
        if(arg1->type == constnum_e){
            fprintf(quadfile,"PARAM %f\t[line: %d]\n",arg1->numConst, line);
        }
        else if(arg1->type == conststring_e){
            fprintf(quadfile,"PARAM %s\t[line: %d]\n",arg1->strConst, line);
        }
        else if(arg1->type == constbool_e){
            if(arg1->boolConst == 0){
                fprintf(quadfile,"PARAM FALSE\t[line: %d]\n",line);
            }else{
                fprintf(quadfile,"PARAM TRUE\t[line: %d]\n",line);
            }
        }
        else if(arg1->type == nil_e){
            fprintf(quadfile,"PARAM NIL\t[line: %d]\n",line);
        }
    }
    else{
        fprintf(quadfile,"PARAM %s\t[line: %d]\n",arg1->sym->value->name, line);
    }
}
void print_return(expr* arg1, int line){
    if(arg1 == NULL ){
        fprintf(quadfile,"RETURN NIL\t[line: %d]\n",line);
    }
    else if(arg1->sym == NULL){
        if(arg1->type == nil_e){
            if(arg1->sym != NULL){
                fprintf(quadfile,"RETURN %s\t[line: %d]\n",arg1->sym->value->name, line);
            }
            else{
                fprintf(quadfile,"RETURN NIL\t[line: %d]\n",line);
            }
        }
        else if(arg1->type == conststring_e){
            if(arg1->sym != NULL){
                fprintf(quadfile,"RETURN %s\t[line: %d]\n",arg1->sym->value->name, line);
            }
            else{
                fprintf(quadfile,"RETURN %s\t[line: %d]\n",arg1->strConst, line);
            }
        }
        else if(arg1->type == constbool_e){
            if(arg1->sym != NULL){
                fprintf(quadfile,"RETURN %s\t[line: %d]\n",arg1->sym->value->name, line);
            }
            else{
                if(arg1->boolConst == 0){
                    fprintf(quadfile,"RETURN FALSE\t[line: %d]\n",line);
                }
                else{
                    fprintf(quadfile,"RETURN TRUE\t[line: %d]\n",line);
                }
            }
        }
        else if(arg1->type == constnum_e){
            if(arg1->sym != NULL){
                fprintf(quadfile,"RETURN %s\t[line: %d]\n",arg1->sym->value->name, line);
            }
            else{
                fprintf(quadfile,"RETURN %f\t[line: %d]\n",arg1->numConst, line);
            }
        }
    }
    else{
        fprintf(quadfile,"RETURN %s\t[line: %d]\n",arg1->sym->value->name, line);

    }

}
