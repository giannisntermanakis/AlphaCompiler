%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../headerfiles/tokens.h"
#include "../headerfiles/i_code_datatypes.h"
#include "../headerfiles/symtable.h"
#include "../code/parser.tab.h" //this was in headerfiles but since 2021 it does not work

#define YY_DECL int yylex(void* ylval) 
#define CHUNK_SIZE 10000
#define eof 0 

extern struct alpha_token_t* QUE ,*HEAD ;

int token_counter = 1 ;
int returned;

%}
%option yylineno
%option noyywrap


delim [ \t\n]
ws  {delim}+
letter [a-zA-Z]
digit   [0-9]
integer {digit}+
real    {digit}+("."{digit}+)?
underscore [_]
id  {letter}({letter}|{digit}|{underscore})*
string "\""
                            
%%
"if"        { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD IF");          yylval.value = QUE ;      token_counter++; return KEYWORD_IF;}  
"else"      { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD ELSE");        yylval.value = QUE ;      token_counter++; return KEYWORD_ELSE; }
"while"     { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD WHILE");       yylval.value = QUE ;      token_counter++; return KEYWORD_WHILE;}
"for"       { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD FOR");         yylval.value = QUE ;      token_counter++; return KEYWORD_FOR; }
"function"  { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD FUNCTION");    yylval.value = QUE ;      token_counter++; return KEYWORD_FUNCTION;}
"return"    { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD RETURN");      yylval.value = QUE ;      token_counter++; return KEYWORD_RETURN; } 
"break"     { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD BREAK");       yylval.value = QUE ;      token_counter++; return KEYWORD_BREAK;} 
"continue"  { push_list(yylineno , token_counter , yytext , yyleng ,"KEYWORD CONTINUE");    yylval.value = QUE ;      token_counter++; return KEYWORD_CONTINUE;}
"and"       { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD AND");         yylval.value = QUE ;      token_counter++; return KEYWORD_AND;}
"not"       { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD NOT");         yylval.value = QUE ;      token_counter++; return KEYWORD_NOT;}
"or"        { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD OR");          yylval.value = QUE ;      token_counter++; return KEYWORD_OR;}
"local"     { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD LOCAL");       yylval.value = QUE ;      token_counter++; return KEYWORD_LOCAL;}
"true"      { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD TRUE");        yylval.value = QUE ;      token_counter++; return KEYWORD_TRUE; }
"false"     { push_list(yylineno , token_counter , yytext , yyleng ,"KEYWORD FALSE");       yylval.value = QUE ;      token_counter++; return KEYWORD_FALSE;}
"nil"       { push_list(yylineno , token_counter , yytext ,yyleng , "KEYWORD NIL");         yylval.value = QUE ;      token_counter++; return KEYWORD_NIL;}


"{"         { push_list(yylineno , token_counter , yytext ,yyleng , "LEFT_CURLBRA");        yylval.value = QUE ;      token_counter++; return LEFT_CURLBRA;}
"}"	        { push_list(yylineno , token_counter , yytext ,yyleng , "RIGTH_CURLBRA");       yylval.value = QUE ;      token_counter++; return RIGHT_CURLBRRA;}
"["         { push_list(yylineno , token_counter , yytext ,yyleng , "LEFT_BRA");            yylval.value = QUE ;      token_counter++; return LEFT_BRACK;}
"]"         { push_list(yylineno , token_counter , yytext ,yyleng , "RIGHT_BRA");           yylval.value = QUE ;      token_counter++; return RIGHT_BRACK;}
"("         { push_list(yylineno , token_counter , yytext ,yyleng , "LEFT_PAR");            yylval.value = QUE ;      token_counter++; return LEFT_PAR;}
")"         { push_list(yylineno , token_counter , yytext ,yyleng , "RIGHT_PAR");           yylval.value = QUE ;      token_counter++; return RIGHT_PAR;}
";"         { push_list(yylineno , token_counter , yytext ,yyleng , "SEMI");                yylval.value = QUE ;      token_counter++; return SEMI;}
","         { push_list(yylineno , token_counter , yytext ,yyleng , "COMMA");               yylval.value = QUE ;      token_counter++; return COMMA;}
":"         {returned= find_colomn() ;                                                      yylval.value = QUE ;                       return returned ;}
"."         {returned = find_dot() ;                                                        yylval.value = QUE ;                       return returned; }


"="	        { returned = find_assign();                                                     yylval.value = QUE ;                       return returned;}
"+"	        { returned = find_plus() ;                                                      yylval.value = QUE ;                       return returned;}
"-"         { returned = find_minus() ;                                                     yylval.value = QUE ;                       return returned;}
"*"         { push_list(yylineno , token_counter , yytext ,yyleng , "OPERATOR_MUL");        yylval.value = QUE ;      token_counter++; return MUL;}
"/"         { push_list(yylineno , token_counter , yytext ,yyleng , "OPERATOR_DEV");        yylval.value = QUE ;      token_counter++; return DEV;}
"%"         { push_list(yylineno , token_counter , yytext ,yyleng , "OPERATOR_MOD");        yylval.value = QUE ;      token_counter++; return MOD;}
"!="        { push_list(yylineno , token_counter , yytext ,yyleng , "OPERATOR_EQUAL");      yylval.value = QUE ;      token_counter++; return NOTEQUAL;}
">"         {returned = find_more() ;                                                       yylval.value = QUE ;                       return returned ; }
"<"         {returned = find_less() ;                                                       yylval.value = QUE ;                       return returned ;}
"//"        { find_comment1() ;                                                             yylval.value = QUE ;                       ;}
"/*"        {find_comment2() ;                                                              yylval.value = QUE ;                       ;}

{id}        { push_list(yylineno , token_counter , yytext ,yyleng , "IDENTIFICATION");      yylval.value = QUE ;      token_counter++; return IDENTIFICATION; }
{integer}   { push_list(yylineno , token_counter , yytext ,yyleng , "INTEGER CONST");       yylval.intval = atoi(QUE->content) ;      token_counter++; return INT_CONST;}
{real}      { push_list(yylineno , token_counter , yytext , yyleng ,"REAL CONST");          yylval.realval = atof(QUE->content) ;      token_counter++; return REAL_CONST;}
{string}    { find_string() ;                                                               yylval.strval = QUE->content ;                       return STRING ; }
{ws}        ;


<<EOF>>     {               ;                                                                                                         return 0;}
.           {fprintf(stderr,"Unexpected character-> -%s- ,at line %d\n",yytext,yylineno);                                             return 0;}
%%


char* allocate_space(int size){

    char* ptr ;
    if( (ptr = (char*) malloc(size * sizeof(char)) ) == NULL){
        perror("CANT ALLOCATE MEMORY\n");
        free_all();
        exit(-1) ;
    }
    return ptr ;
}
void  push_list(int line , int tok_num , char* cont , int len , char* type){
    //create
    struct alpha_token_t *new_tok = malloc(sizeof(struct alpha_token_t)) , *temp;

    new_tok->numline = line ;
    new_tok->numToken = tok_num ;
    new_tok->content = allocate_space(len+1) ;
    strcpy(new_tok->content , cont);
    new_tok->type = allocate_space(strlen(type)+1) ;
    strcpy(new_tok->type , type) ;
    new_tok->alpha_yylex = NULL;
       
       
     // now install
    if((struct alpha_token_t*) QUE == NULL){printf("null \n");exit(1);}
    QUE->alpha_yylex = new_tok ;
    QUE = QUE->alpha_yylex ;
    //fprintf(yyout,"%d:\t#%d\t\"%s\"\t%s\n",new_tok->numline ,new_tok->numToken , new_tok->content , new_tok->type ) ;
    return  ;
}
void print_array(struct alpha_token_t* head){
    struct alpha_token_t * tmp ;
    tmp = head ;
    if(tmp == NULL){printf("error null head in print\n"); exit(1);}
        
    tmp = tmp->alpha_yylex ;

    while( tmp != NULL){
        fprintf(yyout,"%d:\t#%d\t\"%s\"\t%s\n",tmp->numline ,tmp->numToken , tmp->content , tmp->type ) ;    
        tmp = tmp->alpha_yylex ;
    }
    return ;
}
void  find_string(void){
    int index = 0 ,start_line , end_line ;
    char c1 , c2 ;
    char Buffer_char[CHUNK_SIZE]  ;
    index = 0 ;
    c1 = 0;
    c2 = 0;
    start_line = yylineno ;

    memset(Buffer_char , 0 ,CHUNK_SIZE * sizeof(char));
    Buffer_char[index++] = '\"';
    while( (c1 = input() ) != eof && index < CHUNK_SIZE){

        if(c1 == '\"')          break ;
        else if(c1 != '\\')     Buffer_char[index++] = c1 ;
        else{

            c2 = input() ;

            if(c2 == 'n')       Buffer_char[index++] = '\n' ;   //  newline
            else if(c2 == 't')  Buffer_char[index++] = '\t' ;   //  tab
            else if(c2 == '\"') Buffer_char[index++] = '\"' ;   //  quote
            else if(c2 == '\\') Buffer_char[index++] = '\\' ;   //  slash
            else if(c2 == '\'') Buffer_char[index++] = '\'' ;   //  tonos
            else if(c2 == 'a')  Buffer_char[index++] = '\a' ;   //  alarm
            //the error maybe is because : if c2 = eof it will terminate
            else{
		        //we forgot to print the error[ just print in stderr unknown esc char]
                //here is an error unknown escape char or buffer overflow
                fprintf(stderr,"Illegal escape character \\%c at line %d\n",c2 , yylineno) ; 
                free_all() ;
                exit(-1);
            }
        }                     
    }

    if(c1 == eof ) {fprintf(stderr,"Lexical error non-terminated string\n"); free_all(); exit(-1);} 
    if (c1 == '\"'){
        Buffer_char[index++] = '\"' ;
        Buffer_char[index] = '\0' ;
    }
    push_list( start_line , token_counter++ , Buffer_char , index , "STRING" );
    yylval.strval = strdup(Buffer_char) ;
    return  ;

}
void find_comment1(void){
    int index = 0 ,start_line ;
    char c1 = 0 , c2 = 0 ;
    char* INFO ;
    start_line = yylineno ;
    while( (c1 = input()) != eof && c1 != '\n'){
    }
    INFO = make_string_for_comment_info(start_line, start_line );
    push_list( start_line , token_counter++ , INFO ,strlen(INFO) , "ONE-LINE_COMMENT") ; //we dont care about it's data
    free(INFO);

    return  ;
}
void find_comment2(void){
    int start_line , end_line , depth = 1 , nested_flag = 0 ,z = 0;
    int COM_C = 10 ,FILL_C = 0 ,begin = 0 ,prl , i;
    char c1 = 0 , c2 = 0  ;
    char* INFO ;

    struct comment_data *ARRAY ;
    ARRAY = (struct comment_data*) malloc(COM_C * sizeof(struct comment_data)) ;

    for( prl = 0 ; prl < COM_C ; prl++){
        ARRAY[prl].start = -1;
        ARRAY[prl].end = -1;
    }

    ARRAY[begin].end = -1;
    ARRAY[begin++].start = yylineno ;    FILL_C ++ ;
    
    start_line = yylineno ;
    FILL_C++ ;

    while( (c1 = input()) != eof && depth >0){

        if ( c1 == '/'){
            c2 = input() ;
            if(c2 == eof)   break ;
            else if (c2 == '*'){

                if(FILL_C == COM_C - 1 ){
                    //almost full => expand
                    ARRAY = (struct comment_data *) realloc(ARRAY , 2*COM_C * sizeof(struct comment_data  )) ;
                    COM_C *= 2;
                    ARRAY[begin].end = -1;
                    ARRAY[begin ++].start = yylineno ;
                    FILL_C ++ ;
      		     depth++;//new one
                }
                else{
                    ARRAY[begin].end = -1;
                    ARRAY[begin++].start = yylineno ;
                    
                    FILL_C ++ ;
                    depth++ ;
                }
                /*to depth den aujanei an den mpei sto else*/
            } 
            else unput(c2);
        }
        else if(c1 == '*' ){
            c2 = input() ;
            if(c2 == eof)   break;
            else if (c2 == '/'){
            z = begin - 1;
                while(z>=0){
                   
                    if(ARRAY[z].end == -1){
                        ARRAY[z].end = yylineno;
                        
                        break;
                    }
                    z--;
                }          

                depth-- ;

            } 
            else unput(c2);
        }
    } 
    unput(c1);
    INFO = make_string_for_comment_info(ARRAY[0].start, ARRAY[0].end );
    push_list( ARRAY[0].start , token_counter++ ,INFO , strlen(INFO), "MULTILINE COMMENT");
    
    if(ARRAY[0].end == -1){

        //error Non-Terminated comment
        fprintf(stderr,"Lexical error non-terminated comment starts at line %d\n",ARRAY[0].start);
        free(INFO);
        free(ARRAY);
        free_all() ; 

        exit(-1);

    }         
    


    for(  i = 1 ; i < begin ; i++){
        
        if(ARRAY[i].start != -1 && ARRAY[i].end == -1){

            fprintf(stderr,"Lexical error non-terminated comment starts at line %d\n",ARRAY[i].start);
            free(INFO);
            free(ARRAY);
            free_all() ; 

            exit(-1);

        }   
        INFO = make_string_for_comment_info(ARRAY[i].start, ARRAY[i].end );
        

        push_list( ARRAY[i].start , token_counter++ ,INFO , strlen(INFO), "NESTED COMMENT");


    }
    free(INFO);
   
    return  ;
}
char* make_string_for_comment_info(int start , int end ){
    int s_len ,e_len , count = 0 , i; 
    int tmp_s = start , tmp_e = end ;
    while (tmp_s != 0) {
        tmp_s /= 10;    
        ++count;
    }
    s_len = count ;
    count = 0 ;
    while (tmp_e != 0) {
        tmp_e /= 10;    
        ++count;
    }

    e_len = count ;

    char* str_start = (char*) malloc(s_len * sizeof(char));
    
    if(str_start == NULL){
        perror("CANT ALLOCATE MEMORY\n");
        free_all() ;
    }
    char* str_end = (char*) malloc(e_len * sizeof(char));
    if(str_end == NULL){
        perror("CANT ALLOCATE MEMORY\n");
        free(str_start);
        free_all() ;
    }

    /*make the inegers ->strings */
    itoa(start , str_start );
    itoa(end , str_end );

    char* total = (char*) malloc((10+s_len+e_len)*sizeof(char));
    total[0] = 'l';
    total[1] = 'i';
    total[2] = 'n';
    total[3] = 'e';
    total[4] = ' ';
    for( i = 0 ; i < s_len ; i++){
        total[5+i] = str_start[i];
    }
    total[5+s_len] = ' ';
    total[5+s_len+1] = 't';
    total[5+s_len+2] = 'o';
    total[5+s_len+3] =  ' ';
    for( i = 0 ; i < e_len ; i++){
        total[5+s_len+4+i] = str_end[i];
    }
    total[5+4+s_len+e_len] = '\0';

    free(str_start);
    free(str_end);

    return total ;
}

int find_colomn(void){
    char c = 0 ;
    c = input() ; 
    if(c == eof ){
        push_list( yylineno , token_counter , ":" ,1 , "COLOMN");        token_counter++;
        yylval.value =QUE;
        return COLOMN  ;
    }
    else if( c == ':'){
        push_list( yylineno , token_counter , "::" , 2 , "DOUBLE_COLOMN");        token_counter++;
        yylval.value =QUE;
        return DOUBLE_COLOMN  ;
    }
    else{
        unput(c) ;
        push_list( yylineno , token_counter , ":" ,1 , "COLOMN");        token_counter++;
        yylval.value =QUE;
        return COLOMN  ;
    }
}
int find_dot(void){
    char c = 0 ;
    c = input() ; 
    if(c == eof ){
        push_list( yylineno , token_counter , "." ,1 , "DOT");        token_counter++;
        yylval.value = QUE;
        return DOT;
    }
    else if( c == '.'){
        push_list( yylineno , token_counter , ".." , 2 , "DOUBLE_DOT");        token_counter++;
        yylval.value = QUE;
        return DOUBLE_DOT;
    }
    else{
        unput(c) ;
        push_list( yylineno , token_counter , "." ,1 , "DOT");        token_counter++;
        yylval.value = QUE;
        return DOT;
    }

}
int find_assign( void){
    char c = 0 ;
    c = input() ; 
    if(c == eof ){
        push_list( yylineno , token_counter , "=" ,1 , "OPERATOR_ASSIGNMENT");        token_counter++;
        yylval.value = QUE;
        return  ASSIGN;
    }
    else if( c == '='){
        push_list( yylineno , token_counter , "==" , 2 , "OPERATOR_EQUAL");        token_counter++;
        yylval.value = QUE;
        return  EQUAL;
    }
    else{
        unput(c) ;
        push_list( yylineno , token_counter , "=" ,1 , "OPERATOR_ASSIGNMENT");        token_counter++;
        yylval.value = QUE;
        return  ASSIGN;
    }

}
int find_plus( void){
    char c = 0 ;
    c = input() ; 
    if(c == eof ){
         push_list( yylineno , token_counter , "+" ,1 , "OPERATOR_PLUS");        token_counter++;
        yylval.value =QUE;
        return  PLUS;
    }
    else if( c == '+'){
        push_list( yylineno , token_counter , "++" , 2 , "OPERATOR_PLUSPLUS");        token_counter++;
        yylval.value =QUE;
        return  PLUSPLUS;
    }
    else{
        unput(c) ;
        push_list( yylineno , token_counter , "+" ,1 , "OPERATOR_PLUS");        token_counter++;
        yylval.value =QUE;
        return  PLUS;
    }

}
int find_minus(void){
    char c = 0 ;
    c = input() ; 
    if(c == eof ){
        push_list( yylineno , token_counter , "-" ,1 , "OPERATOR_MINUS");        token_counter++;
        yylval.value =QUE;
        return MINUS ;
    }
    else if( c == '-'){
        push_list( yylineno , token_counter , "--" , 2 , "OPERATOR_MINMIN");        token_counter++;
        yylval.value =QUE;   
        return MINMIN ;
    }
    else{
        unput(c) ;
        push_list( yylineno , token_counter , "-" ,1 , "OPERATOR_MINUS");        token_counter++;
        yylval.value =QUE;
        return MINUS ;
    }

}
int find_more(void ){
    char c = 0 ;
    c = input() ; 
    if(c == eof ){
        push_list( yylineno , token_counter , ">" ,1 , "OPERATOR_MORE");        token_counter++;
        yylval.value =QUE;
        return MORE ;
    }
    else if( c == '='){
        push_list( yylineno , token_counter , ">=" , 2 , "OPERATOR_MOREEQUAL");        token_counter++;
        yylval.value =QUE;
        return MOREEQUAL ;
    }
    else{
        unput(c) ;
        push_list( yylineno , token_counter , ">" ,1 , "OPERATOR_MORE");        token_counter++;
        yylval.value =QUE;
        return MORE ;
    }

}
int find_less(void){
    char c = 0 ;
    c = input() ; 
    if(c == eof ){
        push_list(yylineno , token_counter , "<" ,1 , "OPERATOR_LESS");        token_counter++;
        yylval.value =QUE;
        return LESS ;
    }
    else if( c == '='){
        push_list( yylineno , token_counter , "<=" , 2 , "OPERATOR_LESSEQUAL");        token_counter++;
        yylval.value =QUE;
        return LESSEQUAL ;
    }
    else{
        unput(c) ;
        push_list( yylineno , token_counter , "<" ,1 , "OPERATOR_LESS");        token_counter++;
        yylval.value =QUE;
        return LESS ;
    }
    

}
void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
    return ;

}
void itoa(int n, char s[])
 {
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);

    return ;

 }
void free_all(){
    struct alpha_token_t* tmp = HEAD , *prev ;
    if(tmp->numToken == 0)
        tmp = tmp->alpha_yylex ;
    while(tmp != NULL){
        free(tmp->content);
        free(tmp->type);
        tmp = tmp->alpha_yylex ;
    }
    tmp = HEAD ;
    while(tmp != NULL){
        prev = tmp ;
        tmp = tmp->alpha_yylex ;
        free(prev);
    }
    return ;
}
