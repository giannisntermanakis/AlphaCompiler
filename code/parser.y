/*csd4393 , csd4225 , csd4226*/
%{
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


	extern quad* quads;
	struct Stack* temp_var_Stack , *scope_offset_Stack , *loop_counter_Stack ;

	extern int yylex(void* ylval);
    extern FILE *yyin ;
    extern int yylineno ;
	extern int tempcounter ;

    extern char* yytext ;
    
	FILE *errors , *rulefile , *SYMBOLS ,*quadfile;
    
    SymTable_T symtable ;
    struct alpha_token_t *HEAD,*QUE , *ARG_BEGIN , *TEMP_ID ;
	symbol * temp_symbol = NULL ;
	
	
	unsigned label;
	list_t *ttl , *tfl;
	int syntax_errors_counter = 0 ;
	int infunctioncounter = 0 ;
	int loopcounter = 0 ;
	int openfuncs = 0 ;
	int unnamed = 0 ;
    int scope = 0 ;
	int ACCEPT = 1 ;
	int correct_args = 0 ;
	int check_branch = 0;
	
	expr* temp_exp , *temp_exp2 ,*tmp_constnum_expr;

	symbol* insert_tok(struct alpha_token_t*token, SymType type1);
	symbol* insert(symbol*data);
	
	
	symbol* lvalue_node(struct alpha_token_t*token , int scope , SymType type1 );
	
	symbol* id_for_primary(symbol* data);
	symbol* id_for_assign(symbol* data , int member_item);
	symbol* id_for_term(symbol * data, char*oper);
	symbol* id_for_member(symbol* data);
	symbol* id_for_call(symbol* data);
	symbol* check_for_global_ID(struct alpha_token_t*token , int scope);
	symbol* check_for_local_ID(struct alpha_token_t*token , int scope);
	symbol* check_for_func(struct alpha_token_t*token , int scope);
	symbol* check_for_func_unnamed(struct alpha_token_t*token , int scope);
	symbol* make_symbol(DATA* data , SymType type1);
	symbol* insert_func(symbol* data, int scope);
	
	
	symbol_t match_type1_to_type2(SymType type);

	char* unnamed_func_id() ;

	int currscope();
	int find_arguments();
    int yyerror(void *ylval,const char* msg);

%}
%parse-param {void *ylval} 
%lex-param {void *ylval}

%define parse.error verbose

%union{
    int     				intval;
    float   				realval;
	unsigned				unsignval;
	char* 					strval;
	symbol*					symval;
	DATA*					dataval;
	struct expr*			exprval;
    struct alpha_token_t* 	value;
	call_s*					callval;
	index_set*				setval;
	stmt_t					stmtval;
	for_t					forval;
	quad*					quadval;
}


%token   <value>     KEYWORD_FOR         "FOR"
%token   <value>     KEYWORD_WHILE       "WHILE"
%token   <value>     KEYWORD_ELSE        "ELSE"
%token   <value>     KEYWORD_FUNCTION    "FUNCTION"
%token   <value>     KEYWORD_IF          "IF"
%token   <value>     KEYWORD_RETURN      "RETURN"
%token   <value>     KEYWORD_BREAK       "BREAK"
%token   <value>     KEYWORD_CONTINUE    "CONTINUE"
%token   <value>     KEYWORD_AND         "AND"
%token   <value>     KEYWORD_NOT         "NOT"
%token   <value>     KEYWORD_OR          "OR"
%token   <value>     KEYWORD_LOCAL       "LOCAL"
%token   <value>     KEYWORD_TRUE        "TRUE"
%token   <value>     KEYWORD_FALSE       "FALSE"
%token   <value>     KEYWORD_NIL         "NIL"

%token  <value>      PLUS                "+"
%token  <value>      ASSIGN              "="
%token  <value>      MINUS               "-"
%token  <value>      MUL                 "*"
%token  <value>      DEV                 "/"
%token  <value>      MOD                 "%"
%token  <value>      EQUAL               "=="
%token  <value>      NOTEQUAL            "!="
%token  <value>      PLUSPLUS            "++"
%token  <value>      MINMIN              "--"
%token  <value>      MORE                ">"
%token  <value>      LESS                "<"
%token  <value>      MOREEQUAL           ">="
%token  <value>      LESSEQUAL           "<="

%token   <intval>    INT_CONST           "INT_CONST"
%token   <realval>   REAL_CONST          "REAL_CONST"
%token   <strval>    STRING              "STRING"
%token   <value>     LEFT_PAR            "("
%token   <value>     RIGHT_PAR           ")"
%token   <value>     LEFT_BRACK          "["
%token   <value>     RIGHT_BRACK         "]"
%token   <value>     LEFT_CURLBRA        "{"
%token   <value>     RIGHT_CURLBRRA      "}"
%token   <value>     SEMI                ";"
%token   <value>     COLOMN              ":"
%token   <value>     COMMA               ","
%token   <value>     DOUBLE_COLOMN       "::"
%token   <value>     DOT                 "."
%token   <value>     DOUBLE_DOT          ".."
%token   <value>     IDENTIFICATION      "IDENT"
%token               COMMENT1            "LINE_COM"
%token               COMMENT2            "MULTI_COM"

%type   <exprval>    	program  expr term assignexpr 
%type	<exprval>		primary lvalue member call elist elistpref elisthelppref
%type   <exprval>    	objectdef  const idlist ifstmt 
%type   <exprval>    	returnstmt helpexpr elisthelp

%type	<unsignval>		ifprefix elseprefix 
%type	<unsignval>		whilestart whilecond 
%type	<unsignval>		N M 
%type	<unsignval>	 	funcbody

%type	<callval>	 	callsuffix normcall methodcall

%type	<setval>		indexed indexedelem indexedhelp

%type	<dataval>	 	funcname

%type	<symval>	 	funprefix funcdef

%type	<stmtval>	 	stmts stmt break_ continue_ loopstmt 
%type	<stmtval>		block blockhelp whilestmt forstmt

%type 	<forval>	 	forprefix

%type	<intval>		K funcargs

%right          ASSIGN
%left           KEYWORD_OR
%left           KEYWORD_AND
%nonassoc       EQUAL           NOTEQUAL
%nonassoc       LESS            LESSEQUAL       MORE    MOREEQUAL
%left           MINUS           PLUS
%left           MUL             DEV             MOD
%right          KEYWORD_NOT     PLUSPLUS        MINMIN  UMINUS
%left           DOT             DOUBLE_DOT
%left           LEFT_BRACK      RIGHT_BRACK
%left           LEFT_PAR        RIGHT_PAR


%%
program:    stmts{
					fprintf(rulefile,"ACCEPTED\n\n\n"); 
				}
    |       %empty              {}
    ;       

stmts:	stmt		{
						$stmts = $stmt ;
					}
	|	stmts stmt	{

						$$.breakList = mergelist($1.breakList , $2.breakList);
						$$.contList = mergelist($1.contList , $2.contList);
					}
	;
stmt:       expr SEMI           
						{
							fprintf(rulefile,"stmt\t->\t");
							yyerrok;        
							$$.breakList = 0;
							$$.contList = 0;  
							if($expr->notexpr == 1){
								$expr->truelist = newnode();
								$expr->truelist->data = next_quad_label();
								$expr->truelist->next = 0;
								$expr->falselist = newnode();
								$expr->falselist->data = next_quad_label()+1;
								$expr->falselist->next = 0 ;
								
								emit(if_eq , NULL , $expr , newexpr_constbool(1) , 0 , yylineno);
								emit(jump , NULL , NULL , NULL , 0 , yylineno);

								label = next_quad_label();

								//reverse
								ttl = $expr->truelist ;
								tfl = $expr->falselist ;
								$expr->truelist = tfl;
								$expr->falselist = ttl; 
								
								$expr->type = boolexpr_e;
							} 

							if($expr->type == boolexpr_e){
								complete($expr);
							} 
						}                        
    |       ifstmt              
						{
							fprintf(rulefile,"stmt\t->\t");
							$$.breakList = 0;
							$$.contList = 0;
						}
    |       whilestmt           
						{
							fprintf(rulefile,"stmt\t->\t");
							$$.breakList = 0;
							$$.contList = 0;
						}
    |       forstmt             
						{
							fprintf(rulefile,"stmt\t->\t");
							$$.breakList = 0;
							$$.contList = 0;
						}
    |       returnstmt          
						{
							fprintf(rulefile,"stmt\t->\t");
							$$.breakList = 0;
							$$.contList = 0;
						}
    |       break_      
						{
							fprintf(rulefile,"stmt(break)\t->\t");
							yyerrok;   
							$stmt = $break_;   
						}
    |       continue_
						{
							fprintf(rulefile,"stmt(continue)\t->\t");
							yyerrok;          
							$stmt = $continue_;  
						}
    |       block               
						{
							fprintf(rulefile,"stmt\t->\t");  

						}
    |       funcdef             
						{
							fprintf(rulefile,"stmt\t->\t");                      
							$$.breakList = 0;
							$$.contList = 0;
						}
    |       SEMI                
						{
							fprintf(rulefile,"stmt\t->\t");                      
							$$.breakList = 0;
							$$.contList = 0;
						}
    ;
        

break_: KEYWORD_BREAK SEMI	{
								if(loopcounter == 0){
									fprintf(errors,"syntax error,break not in a loop at line %d\n",yylineno);
									++ syntax_errors_counter;
								}
								make_stmt(& $break_);
								$break_.breakList = newlist(next_quad_label());
								$break_.contList = 0;

								emit(jump , NULL , NULL , NULL , 0 , yylineno);
							}
	;
continue_: KEYWORD_CONTINUE SEMI	{
										if(loopcounter == 0){
											fprintf(errors,"syntax error,continue not in a loop at line %d\n",yylineno);
											++ syntax_errors_counter;
										}
										make_stmt(& $continue_);
										$continue_.contList = newlist(next_quad_label());
										$continue_.breakList = 0;

										emit(jump , NULL , NULL , NULL , 0 , yylineno);
									}
	;
K:		%empty	
				{
					$K = next_quad_label();
				}
		;
expr:       helpexpr            
						{
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$expr = $helpexpr;
						}
	   |    expr  PLUS  expr  
	   					{	
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_arithm_op_expr($1 , $3 , "+");
						} 
       |    expr  MINUS  expr  
	   					{
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_arithm_op_expr($1 , $3 , "-");
						} 
       |    expr  MUL  expr  
	   					{	
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_arithm_op_expr($1 , $3 , "*");
						} 
       |    expr  DEV  expr  
	   					{
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_arithm_op_expr($1 , $3 , "/");
						} 
       |    expr  MOD  expr  
	   					{
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_arithm_op_expr($1 , $3 , "%");
						} 
       |    expr  MORE  expr  
	   					{
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_rel_op_expr($1 , $3 , ">");
						} 
       |    expr  MOREEQUAL  expr  
	   					{
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_rel_op_expr($1 , $3 , ">=");
						} 
       |    expr  LESS  expr  
	   					{
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_rel_op_expr($1 , $3 , "<");
						} 
       |    expr  LESSEQUAL  expr  
	   					{
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_rel_op_expr($1 , $3 , "<=");
						} 
       |    expr  EQUAL  expr  
	   					{
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_rel_op_expr($1 , $3 , "==");
						} 
       |    expr  NOTEQUAL  expr  
	   					{
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_rel_op_expr($1 , $3 , "!=");
						} 
       |    expr  KEYWORD_AND K expr  
	   					{
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_bool_op_expr($1 , $4 , "and" , $K);
						} 
       |    expr  KEYWORD_OR K expr  
	   					{
							fprintf(rulefile,"expr\t->\t");
							yyerrok;
							$$ = expr_bool_op_expr($1 , $4 , "or" , $K);
						} 
       ;

loopstmt:	loopstart stmt loopend  {
										$loopstmt = $stmt; 
									}
	;
loopstart:	%empty	{++loopcounter;}
	;
loopend:	%empty	{--loopcounter;}
	;
helpexpr:   assignexpr         
						{	
							$helpexpr = $assignexpr;
						}
    |       term        
						{
							$helpexpr = $term;
						}
    ;
	


term:       LEFT_PAR expr RIGHT_PAR     
							{
								fprintf(rulefile,"term\t->\t");
								yyerrok; 
								$term = $expr;
							}
    |       MINUS expr %prec UMINUS     
							{
								fprintf(rulefile,"term\t->\t");
								yyerrok; 

								assert($expr);
								$term = uminus_expr($expr);
							}
    |       KEYWORD_NOT expr            
							{
								fprintf(rulefile,"term\t->\t");
								yyerrok; 
								assert($expr);
								if($expr->type != error_e){
									
									//oliki apotimisi
									
									// $term = newexpr(boolexpr_e);
									// $term->sym = newtemp();
									// emit(not , $term , $expr , NULL , 0 , yylineno);
									
									
									//merikh
									
									//isws na prepei na checkarw an einai null oi listes tou
									if($expr->type != boolexpr_e){
										if($expr->notexpr == 1){
											$expr->notexpr = 0 ;
										}
										else{
											$expr->notexpr = 1 ;
										}
									}
								
									$term->type = $expr->type;
									$term->sym = $expr->sym;
									$term->notexpr = $expr->notexpr;
									$term->truelist = $expr->falselist;
									$term->falselist = $expr->truelist;
									

								}
								else{
									$term = $expr ;
								}
							}
    |       PLUSPLUS lvalue             
							{
								fprintf(rulefile,"term\t->\t");
								yyerrok;
								assert($lvalue);
								/*elegcos gia func/table ?*/
								
								$lvalue->sym = id_for_term($lvalue->sym,"++");
										
								if($lvalue->sym->type1 == ERROR){
									$lvalue->type = error_e;
								}
								if($lvalue->index != NULL){
									$term = term_is_member($lvalue);
									tmp_constnum_expr = newexpr(constnum_e);
									tmp_constnum_expr->sym = newtemp();
									tmp_constnum_expr->numConst = 1 ;

									temp_exp = newexpr(arithexpr_e);
									temp_exp->sym = newtemp();

									emit(add , $term , $term ,tmp_constnum_expr , 0 , yylineno);
									
									emit(tablesetelem , $term , $lvalue , $lvalue->index , 0 , yylineno);
								}
								else if(typecheck($lvalue , NULL , "incr/decr")){
									tmp_constnum_expr = newexpr(constnum_e);
									tmp_constnum_expr->numConst = 1 ;

									emit(add , $lvalue , $lvalue , tmp_constnum_expr , 0 , yylineno);

									++ $lvalue->numConst ;

									$term = newexpr(arithexpr_e);
									$term->sym = newtemp();
									$term->numConst = $lvalue->numConst ;
									emit(assign , $term , $lvalue , NULL , 0 , yylineno );
								}
								else{
									printf("++elsee problemm\n");
								}
								
							}
    |       lvalue PLUSPLUS             
							{
								
								fprintf(rulefile,"term\t->\t");
								yyerrok;
								assert($lvalue);
								/*elegcos gia func/table ?*/
	
								$lvalue->sym = id_for_term($lvalue->sym,"++");
								if($lvalue->sym->type1 == ERROR){
									$lvalue->type = error_e;
								}


								if($lvalue->index != NULL){
									$term = term_is_member($lvalue);
									
									tmp_constnum_expr = newexpr(constnum_e);
									tmp_constnum_expr->sym = newtemp();
									tmp_constnum_expr->numConst = 1 ;

									temp_exp = newexpr(arithexpr_e);
									temp_exp->sym = newtemp();
									emit(assign , temp_exp , $term , NULL , 0 , yylineno); 

									emit(add , $term , $term ,tmp_constnum_expr , 0 , yylineno);
									emit(tablesetelem, $term  , $lvalue , $lvalue->index , 0 , yylineno);

									$term = temp_exp;
								}
								else if( typecheck($lvalue , NULL , "incr/decr") ){

									$term = newexpr(arithexpr_e);
									$term->sym = newtemp();
									$term->numConst = $lvalue->numConst;										

									emit(assign , $term , $lvalue , NULL , 0 , yylineno);

									tmp_constnum_expr = newexpr(constnum_e);
									tmp_constnum_expr->numConst = 1 ;

									emit(add , $lvalue , $lvalue , tmp_constnum_expr , 0 , yylineno);
									++ $lvalue->numConst ;
								}
								else{
									printf("else++ problemmm\n");
								}
						
							}
    |       MINMIN lvalue               
							{
								fprintf(rulefile,"term\t->\t");
								yyerrok;
								assert($lvalue);
								/*elegcos gia func/table ?*/
								
								$lvalue->sym = id_for_term($lvalue->sym,"--");
										
								if($lvalue->sym->type1 == ERROR){
									$lvalue->type = error_e;
								}
								
								if($lvalue->index != NULL){
									$term = term_is_member($lvalue);
									tmp_constnum_expr = newexpr(constnum_e);
									tmp_constnum_expr->sym = newtemp();
									tmp_constnum_expr->numConst = 1 ;

									temp_exp = newexpr(arithexpr_e);
									temp_exp->sym = newtemp();

									emit(sub , $term , $term ,tmp_constnum_expr , 0 , yylineno);
									
									emit(tablesetelem , $term , $lvalue , $lvalue->index , 0 , yylineno);

								}
								else if(typecheck($lvalue , NULL , "incr/decr")){
									tmp_constnum_expr = newexpr(constnum_e);
									tmp_constnum_expr->numConst = 1 ;

									emit(sub , $lvalue , $lvalue , tmp_constnum_expr , 0 , yylineno);

									-- $lvalue->numConst ;

									$term = newexpr(arithexpr_e);
									$term->sym = newtemp();
									$term->numConst = $lvalue->numConst ;
									emit(assign , $term , $lvalue , NULL , 0 , yylineno );
								}
								else{
									printf("--elsee problemm\n");
								}
							}
    |       lvalue MINMIN               
							{
								fprintf(rulefile,"term\t->\t");
								yyerrok;
								assert($lvalue);
								/*elegcos gia func/table ?*/
	
								$lvalue->sym = id_for_term($lvalue->sym,"--");
									 
								if($lvalue->sym->type1 == ERROR){
									$lvalue->type = error_e;
								}


								if($lvalue->index != NULL){
								//if($lvalue->type == tableitem_e){
									$term = term_is_member($lvalue);
									//$term = emit_iftableitem($lvalue) ;
									tmp_constnum_expr = newexpr(constnum_e);
									tmp_constnum_expr->sym = newtemp();
									tmp_constnum_expr->numConst = 1 ;

									temp_exp = newexpr(arithexpr_e);
									temp_exp->sym = newtemp();
									emit(assign , temp_exp , $term , NULL , 0 , yylineno); 

									emit(sub , $term , $term ,tmp_constnum_expr , 0 , yylineno);
									
									emit(tablesetelem  , $term, $lvalue , $lvalue->index , 0 , yylineno);

									$term =  temp_exp;

								}
								else if( typecheck($lvalue , NULL , "incr/decr") ){

									$term = newexpr(arithexpr_e);
									$term->sym = newtemp();
									$term->numConst = $lvalue->numConst;										

									emit(assign , $term , $lvalue , NULL , 0 , yylineno);

									tmp_constnum_expr = newexpr(constnum_e);
									tmp_constnum_expr->numConst = 1 ;

									emit(sub , $lvalue , $lvalue , tmp_constnum_expr , 0 , yylineno);
									-- $lvalue->numConst ;
								}
								else{
									printf("else-- problemmm\n");
								}
							}
    |       primary                     
							{
								fprintf(rulefile,"term\t->\t");
								$term = $primary ;	
							}
    ;

assignexpr: lvalue  ASSIGN  expr         
						{
							fprintf(rulefile,"assignexpr\t->\t");
							yyerrok;
							$lvalue->sym = id_for_assign($lvalue->sym , 1);

							if($lvalue->sym->type1 == ERROR){
								$lvalue->type = error_e;
							}
							else{
								$lvalue->sym = id_for_assign($lvalue->sym , 0);
							}

							if($lvalue->index != NULL){
								$lvalue->type = tableitem_e;
							}
							//merikh apotimhsh
							if($expr->notexpr == 1){
								$expr->truelist = newnode();
								$expr->truelist->data = next_quad_label();
								$expr->truelist->next = 0;
								$expr->falselist = newnode();
								$expr->falselist->data = next_quad_label()+1;
								$expr->falselist->next = 0 ;
								
								emit(if_eq , NULL , $expr , newexpr_constbool(1) , 0 , yylineno);
								emit(jump , NULL , NULL , NULL , 0 , yylineno);

								label = next_quad_label();

								//reverse
								ttl = $expr->truelist ;
								tfl = $expr->falselist ;
								$expr->truelist = tfl;
								$expr->falselist = ttl; 
								
								$expr->type = boolexpr_e;
							} 

							if($expr->type == boolexpr_e){
								$assignexpr = complete($expr);
								$assignexpr = assign_expr($lvalue , $assignexpr);
							} 
							else{
								$assignexpr = assign_expr($lvalue , $expr);
							}
							
						}
          ;

primary:    lvalue                      
						{
							fprintf(rulefile,"primary\t->\t");
							assert($lvalue);
							/*elegxos gia func  ?*/
							if($lvalue->index != NULL){
								$primary = primary_lval_is_member($lvalue);
	
							}
							else{
								$lvalue->sym = id_for_primary($lvalue->sym);
								if($lvalue->sym->type1 == ERROR){
									$lvalue->type = error_e;
								}
								$primary = symtype_to_expr_type($primary);
								$primary = $lvalue ;
							}
						}
        |   call                        
						{
							fprintf(rulefile,"primary\t->\t");	
							$primary = $call ;
						}  
        |   objectdef                   
						{
							fprintf(rulefile,"primary\t->\t");
							$primary = $objectdef;
						}
        |   LEFT_PAR funcdef RIGHT_PAR  
						{
							fprintf(rulefile,"primary\t->\t");
							yyerrok;
							temp_exp = newexpr(programfunc_e);
							temp_exp->sym = $funcdef ;
							$primary = temp_exp ;
						}
        |   const                       
						{
							fprintf(rulefile,"primary\t->\t");
							$primary = $const;
						}
        ;

lvalue:     IDENTIFICATION  						        				
						{	
							fprintf(rulefile,"lvalue\t->\t");
							$lvalue = lvalue_expr( lvalue_node(yylval.value , scope , LOCAL) ) ;
						}
        |   KEYWORD_LOCAL IDENTIFICATION                              
						{
							fprintf(rulefile,"lvalue\t->\t") ; 
							yyerrok;
							$lvalue = lvalue_expr( check_for_local_ID(yylval.value , scope) ) ;
						}
        |   DOUBLE_COLOMN IDENTIFICATION              	            
						{
							fprintf(rulefile,"lvalue\t->\t"); 	
							yyerrok; 
							$lvalue = lvalue_expr( check_for_global_ID(yylval.value , scope) ) ;
						}
        |   member                                                          
						{
							fprintf(rulefile,"lvalue\t->\t");	
							$lvalue = $member ;
						}
		;

member:     lvalue DOT IDENTIFICATION           
									{
										fprintf(rulefile,"member\t->\t");
										yyerrok; 
										$lvalue->sym = id_for_member($lvalue->sym);	
										$member = member_item_dot($lvalue , $IDENTIFICATION->content);
									}
        |   lvalue LEFT_BRACK expr RIGHT_BRACK  
									{
										fprintf(rulefile,"member\t->\t");
										yyerrok;
										$lvalue->sym = id_for_member($lvalue->sym);	
										//newwww
										if($expr->notexpr == 1){
											$expr->truelist = newnode();
											$expr->truelist->data = next_quad_label();
											$expr->truelist->next = 0;
											$expr->falselist = newnode();
											$expr->falselist->data = next_quad_label()+1;
											$expr->falselist->next = 0 ;
											
											emit(if_eq , NULL , $expr , newexpr_constbool(1) , 0 , yylineno);
											emit(jump , NULL , NULL , NULL , 0 , yylineno);

											label = next_quad_label();

											//reverse
											ttl = $expr->truelist ;
											tfl = $expr->falselist ;
											$expr->truelist = tfl;
											$expr->falselist = ttl; 
											
											$expr->type = boolexpr_e;
										} 

										if($expr->type == boolexpr_e){
											$member = complete($expr);
											$member = member_item_brack($lvalue , $member);										} 
										else{
											$member = member_item_brack($lvalue , $expr);
										}
									}
        |   call DOT IDENTIFICATION             
									{
										fprintf(rulefile,"member\t->\t");
										yyerrok; 
										$member = member_item_dot($call , $IDENTIFICATION->content);
									}
        |   call LEFT_BRACK expr RIGHT_BRACK    
									{
										fprintf(rulefile,"member\t->\t");
										yyerrok;
										//newwww
										if($expr->notexpr == 1){
											$expr->truelist = newnode();
											$expr->truelist->data = next_quad_label();
											$expr->truelist->next = 0;
											$expr->falselist = newnode();
											$expr->falselist->data = next_quad_label()+1;
											$expr->falselist->next = 0 ;
											
											emit(if_eq , NULL , $expr , newexpr_constbool(1) , 0 , yylineno);
											emit(jump , NULL , NULL , NULL , 0 , yylineno);

											label = next_quad_label();

											//reverse
											ttl = $expr->truelist ;
											tfl = $expr->falselist ;
											$expr->truelist = tfl;
											$expr->falselist = ttl; 
											
											$expr->type = boolexpr_e;
										} 

										if($expr->type == boolexpr_e){
											$member = complete($expr);
											$member = member_item_brack($call , $member);										} 
										else{
											$member = member_item_brack($call , $expr);
										}
									}
        ;

call:       call LEFT_PAR elist RIGHT_PAR                                
									{
										fprintf(rulefile,"call\t->\t");
										yyerrok;
										printf("call %s\n",$1->sym->value->name);
										$$ = call_first($1 ,$elist );
									}
        |   lvalue  callsuffix               							 
									{
										fprintf(rulefile,"call\t->\t");
										yyerrok;
										$lvalue->sym = id_for_call($lvalue->sym) ;

										$call = call_second($lvalue , $callsuffix);
									}
        |   LEFT_PAR funcdef RIGHT_PAR LEFT_PAR elist RIGHT_PAR          
									{
										fprintf(rulefile,"call\t->\t");
										yyerrok;
										$call = call_third($funcdef , $elist);
									}
        ;

callsuffix: normcall                                                      
							{
								fprintf(rulefile,"callsuffix\t->\t");
								$callsuffix = $normcall ;
							}                 
        |   methodcall                                                    
							{
								fprintf(rulefile,"callsuffix\t->\t");
								$callsuffix = $methodcall ;
							}
        ;

normcall:   LEFT_PAR elist RIGHT_PAR                                      
							{
								fprintf(rulefile,"normcall\t->\t");
								yyerrok;

								$normcall = (call_s*) malloc(sizeof(call_s)) ;
								$normcall->elist = $elist ;
								$normcall->method = 0 ;
								$normcall->name = NULL ;
								temp_exp = $normcall->elist;
								
							}
        ;

methodcall: DOUBLE_DOT IDENTIFICATION LEFT_PAR elist RIGHT_PAR            
							{
								fprintf(rulefile,"normcall\t->\t");
								yyerrok;

								$methodcall = (call_s*) malloc(sizeof(call_s)) ;
								$methodcall->elist = $elist ;
								$methodcall->method = 1 ;
								$methodcall->name = strdup($IDENTIFICATION->content) ;
								temp_exp = $methodcall->elist;
								
							}
        ;


elistpref:	expr {
					if($expr->notexpr == 1){
						$expr->truelist = newnode();
						$expr->truelist->data = next_quad_label();
						$expr->truelist->next = 0;
						$expr->falselist = newnode();
						$expr->falselist->data = next_quad_label()+1;
						$expr->falselist->next = 0 ;
						
						emit(if_eq , NULL , $expr , newexpr_constbool(1) , 0 , yylineno);
						emit(jump , NULL , NULL , NULL , 0 , yylineno);

						label = next_quad_label();

						//reverse
						ttl = $expr->truelist ;
						tfl = $expr->falselist ;
						$expr->truelist = tfl;
						$expr->falselist = ttl; 
						
						$expr->type = boolexpr_e;
					} 
					if($expr->type == boolexpr_e){
						$elistpref = complete($expr);
					}
					else{
						$elistpref = $expr ;
					}
				}
		;

elist:      elistpref  elisthelp                                                
							{
								fprintf(rulefile,"elist\t->\t");
								yyerrok;
								//create the e-list
								$elistpref->next = $elisthelp;
								$elist = $elistpref ;
								
							}
        |   %empty                                                        
							{
								fprintf(rulefile,"elist\t->\t");
								$elist = NULL;
							}
        ;

elisthelppref:	expr{
						if($expr->notexpr == 1){
							$expr->truelist = newnode();
							$expr->truelist->data = next_quad_label();
							$expr->truelist->next = 0;
							$expr->falselist = newnode();
							$expr->falselist->data = next_quad_label()+1;
							$expr->falselist->next = 0 ;
							
							emit(if_eq , NULL , $expr , newexpr_constbool(1) , 0 , yylineno);
							emit(jump , NULL , NULL , NULL , 0 , yylineno);

							label = next_quad_label();

							//reverse
							ttl = $expr->truelist ;
							tfl = $expr->falselist ;
							$expr->truelist = tfl;
							$expr->falselist = ttl; 
							
							$expr->type = boolexpr_e;
						} 
						if($expr->type == boolexpr_e){
							$elisthelppref = complete($expr);
						}
						else{
							$elisthelppref = $expr ;
						}
					}
	;
elisthelp:  COMMA elisthelppref elisthelp{
									yyerrok;
									//create the e-list
									$elisthelppref->next = $3;
									$$ = $elisthelppref ;
								}
        |   %empty              {
									// end of e-list
									$elisthelp = NULL ;
								}    
        ;

objectdef:  LEFT_BRACK elist RIGHT_BRACK   {			
												fprintf(rulefile,"objectdef\t->\t");
												yyerrok;
												$objectdef = objectdef_elist($elist);
											}
		|	LEFT_BRACK indexed RIGHT_BRACK   {			
												fprintf(rulefile,"objectdef\t->\t");
												yyerrok;
												$objectdef = objectdef_indexed($indexed);
											}
											      
        ;

indexed:    indexedelem indexedhelp                                       
							{
								fprintf(rulefile,"indexed\t->\t");
								yyerrok;
								$indexedelem->next = $indexedhelp;
								$indexed = $indexedelem;
							}
        ;
indexedhelp: COMMA indexedelem indexedhelp      {
													yyerrok;
													$indexedelem->next = $3 ;
													$$ = $indexedelem ;
												}
        |    %empty                             {
													yyerrok;
													$indexedhelp = NULL ;
												}
        ;

indexedelem:  LEFT_CURLBRA expr COLOMN expr RIGHT_CURLBRRA                
							{
								fprintf(rulefile,"indexedelem\t->\t");
								yyerrok;
								$indexedelem =(index_set*) malloc(sizeof(index_set));
								$indexedelem->index = $2 ;
								$indexedelem->value = $4 ;
							}
        ;

code_B1:
	%empty		{
				++scope;
				push(temp_var_Stack , tempcounter );
				resettemp();
				}
		;
code_B2:
	%empty 		{
				SymTable_hide(symtable,scope);
				scope--;
				tempcounter = pop(temp_var_Stack);
				}
		;
block:        LEFT_CURLBRA code_B1 blockhelp code_B2 RIGHT_CURLBRRA                       
										{
											fprintf(rulefile,"block\t->\t");
											yyerrok;
											$block = $blockhelp ;
										}
        ;
blockhelp:    stmt blockhelp            {
											fprintf(rulefile,"helpblock\t->\t");
											yyerrok;
											
											$$.breakList = mergelist($1.breakList , $2.breakList);

											$$.contList = mergelist($1.contList , $2.contList);	

										}
        |     %empty                    {
											$$.breakList = 0;
											$$.contList = 0;
										}
        ;
funcblockstart:		%empty	{
								push(loop_counter_Stack , loopcounter);
								loopcounter = 0 ;
								++infunctioncounter;
							}
		;
funcblockend:		%empty	{
								loopcounter = pop(loop_counter_Stack );
								--infunctioncounter ;
							}
		;
funcdef:	funprefix funcargs funcblockstart funcbody funcblockend{	
											int offset ;
											-- openfuncs;
											fprintf(rulefile,"funcdef\t->\t");
											yyerrok;
											exits_scope_space();
											$funprefix->totallocals = $funcbody ;
											$funprefix->funcargs = $funcargs;
											offset = pop(scope_offset_Stack);
											
											restore_currscope_offset(offset);
											temp_exp = newexpr(programfunc_e);
											temp_exp->sym = $funprefix ;

											emit(funcend , temp_exp , NULL , NULL , 0 , yylineno);
										}
		;

funprefix:	KEYWORD_FUNCTION funcname	{	
											++ openfuncs ;

											$funprefix = make_symbol($funcname , USERFUNC);
											$funprefix->iaddress = next_quad_label();
											$funprefix = insert_func($funprefix ,currscope());
											//if error/collision do not emit 
											temp_exp = newexpr(programfunc_e);
											temp_exp->sym = $funprefix ;
											emit(funcstart ,  temp_exp , NULL , NULL , 0 , yylineno);

											push(scope_offset_Stack , curr_scope_offset());
											enter_scope_space();
											reset_formalarg_offset();
											++scope ;
										}
		;

funcname:	IDENTIFICATION		{
									$funcname =(DATA*) malloc(sizeof(DATA));
									$funcname->name = $IDENTIFICATION->content;
									$funcname->scope = currscope();
									$funcname->line = $IDENTIFICATION->numline;
								}
		|	%empty				{
									$funcname =(DATA*) malloc(sizeof(DATA));
									$funcname->name = unnamed_func_id();
									++unnamed;
									$funcname->scope = currscope();
									$funcname->line = yylineno;
								}
		;

funcargs:	LEFT_PAR idlist RIGHT_PAR	{
											ARG_BEGIN = $1;
											
											$funcargs = find_arguments();
											
											enter_scope_space();
											reset_functionlocal_offset();
											--scope ;
										}
		;
funcbody:	block{
					$funcbody = curr_scope_offset();
					exits_scope_space();
				}
		;

const:      INT_CONST                                                           
						{
							fprintf(rulefile,"const\t->\t");
							$const = newexpr(constnum_e);
							$const->numConst = (double) $INT_CONST ;
						}
        |   REAL_CONST                                                          
						{
							fprintf(rulefile,"const\t->\t");
							$const = newexpr(constnum_e);
							$const->numConst = (double) $REAL_CONST ;
						}
        |   STRING                                                              
						{
							fprintf(rulefile,"const\t->\t");
							$const = newexpr_conststring($STRING);
						}
        |   KEYWORD_NIL                                                         
						{
							fprintf(rulefile,"const\t->\t"); 
							$const = newexpr(nil_e);
						}
        |   KEYWORD_TRUE                                                        
						{
							fprintf(rulefile,"const\t->\t");
							$const = newexpr(constbool_e);
							$const->boolConst = 1 ; 
						}
        |   KEYWORD_FALSE                                                       
						{
							fprintf(rulefile,"const\t->\t");
							$const = newexpr(constbool_e);
							$const->boolConst = 0 ; 
						}
        ;

idlist:     IDENTIFICATION idlisthelp                                         
  						{	
							fprintf(rulefile,"idlist\t->\t");
							yyerrok;
						}
        |   %empty                                                              
						{
							fprintf(rulefile,"idlist\t->\t"); //WHY at empty??
						}
        ;
idlisthelp: COMMA IDENTIFICATION idlisthelp       {yyerrok;}
        |   %empty                                {}
        ;

ifprefix:	KEYWORD_IF LEFT_PAR expr RIGHT_PAR 	{
													
													if($expr->notexpr == 1){
														$expr->truelist = newnode();
														$expr->truelist->data = next_quad_label();
														$expr->truelist->next = 0;
														$expr->falselist = newnode();
														$expr->falselist->data = next_quad_label()+1;
														$expr->falselist->next = 0 ;
														
														emit(if_eq , NULL , $expr , newexpr_constbool(1) , 0 , yylineno);
														emit(jump , NULL , NULL , NULL , 0 , yylineno);

														label = next_quad_label();

														
														ttl = $expr->truelist ;
														tfl = $expr->falselist ;
														$expr->truelist = tfl;
														$expr->falselist = ttl; 
														
														$expr->type = boolexpr_e;
													}
													if($expr->type == boolexpr_e){

														temp_exp = complete($expr);

														emit(if_eq ,NULL , temp_exp , newexpr_constbool(1) , next_quad_label()+2 , yylineno );
													}
													else{
														emit(if_eq ,NULL , $expr , newexpr_constbool(1) , next_quad_label()+2 , yylineno );
													}

													$ifprefix = next_quad_label();
													emit(jump ,NULL , NULL , NULL , 0 , yylineno);
											 	}
		;
elseprefix:	KEYWORD_ELSE	{
								$elseprefix = next_quad_label();
								emit(jump , NULL , NULL , NULL , 0 , yylineno);
							}
		;
ifstmt:		ifprefix stmt	{
								fprintf(rulefile,"ifstmt\t->\t");
								yyerrok;
								patchlabel($ifprefix , next_quad_label());
							}
		|	ifprefix stmt elseprefix stmt	{
												fprintf(rulefile,"ifstmt\t->\t");
												yyerrok;
												patchlabel($ifprefix , $elseprefix + 1);
												patchlabel($elseprefix , next_quad_label());
											}
		;

whilestart:	KEYWORD_WHILE	{
								$whilestart = next_quad_label();
							}
		;
whilecond:	LEFT_PAR expr RIGHT_PAR	{


										if($expr->notexpr == 1){
											$expr->truelist = newnode();
											$expr->truelist->data = next_quad_label();
											$expr->truelist->next = 0;
											$expr->falselist = newnode();
											$expr->falselist->data = next_quad_label()+1;
											$expr->falselist->next = 0 ;
											
											emit(if_eq , NULL , $expr , newexpr_constbool(1) , 0 , yylineno);
											emit(jump , NULL , NULL , NULL , 0 , yylineno);

											label = next_quad_label();

											
											ttl = $expr->truelist ;
											tfl = $expr->falselist ;
											$expr->truelist = tfl;
											$expr->falselist = ttl; 
											
											$expr->type = boolexpr_e;
										}
										if($expr->type == boolexpr_e){

											temp_exp = complete($expr);

											emit(if_eq , NULL , temp_exp , newexpr_constbool(1) , next_quad_label() +2 , yylineno);
										}
										else{
											emit(if_eq , NULL , $expr , newexpr_constbool(1) , next_quad_label() +2 , yylineno);
										}
										$whilecond = next_quad_label();
										emit(jump , NULL , NULL , NULL , 0 , yylineno);
									}
		;
whilestmt:	whilestart whilecond loopstmt	{
											fprintf(rulefile,"whilestmt\t->\t");
											yyerrok;
											emit(jump , NULL , NULL , NULL , $whilestart , yylineno);
											
											patchlabel($whilecond , next_quad_label());
											
											patchlist($loopstmt.breakList , next_quad_label());
											patchlist($loopstmt.contList , $whilestart);
											$$ = $loopstmt;
										}
		;

N:	%empty	{
				$N = next_quad_label();
				emit(jump, NULL , NULL ,NULL , 0 , yylineno);
			}
	;
M:	%empty	{
				$M = next_quad_label();
			}
	;
forprefix:	KEYWORD_FOR LEFT_PAR elist SEMI M expr SEMI	{

															if($expr->notexpr == 1){
																$expr->truelist = newnode();
																$expr->truelist->data = next_quad_label();
																$expr->truelist->next = 0;
																$expr->falselist = newnode();
																$expr->falselist->data = next_quad_label()+1;
																$expr->falselist->next = 0 ;
																
																emit(if_eq , NULL , $expr , newexpr_constbool(1) , 0 , yylineno);
																emit(jump , NULL , NULL , NULL , 0 , yylineno);

																label = next_quad_label();

																
																ttl = $expr->truelist ;
																tfl = $expr->falselist ;
																$expr->truelist = tfl;
																$expr->falselist = ttl; 
																
																$expr->type = boolexpr_e;
															}
															if($expr->type == boolexpr_e){
																temp_exp = complete($expr);
																$forprefix.test = $M ;
																$forprefix.enter = next_quad_label();
																emit(if_eq , NULL , temp_exp , newexpr_constbool(1) , 0 , yylineno);
															}
															else{
																$forprefix.test = $M ;
																$forprefix.enter = next_quad_label();
																emit(if_eq , NULL , $expr , newexpr_constbool(1) , 0 , yylineno);
															}
															
														}	
	;
forstmt:	forprefix N elist RIGHT_PAR N loopstmt N {
													fprintf(rulefile,"forstmt\t->\t");
													yyerrok;
													
													patchlabel($forprefix.enter , $5 + 1 );
													
													patchlabel($2 , next_quad_label() );
													

													patchlabel($5 , $forprefix.test );

													patchlabel($7 , $2 + 1 );

													patchlist($loopstmt.breakList , next_quad_label() );
													patchlist($loopstmt.contList , $2 + 1 );
												}
	;

returnstmt: KEYWORD_RETURN expr SEMI                                      
									{
										fprintf(rulefile,"returnstmt\t->\t");
										yyerrok;
										
										// this is from not expr
										if($expr->notexpr == 1){
											$expr->truelist = newnode();
											$expr->truelist->data = next_quad_label();
											$expr->truelist->next = 0;
											$expr->falselist = newnode();
											$expr->falselist->data = next_quad_label()+1;
											$expr->falselist->next = 0 ;
											
											emit(if_eq , NULL , $expr , newexpr_constbool(1) , 0 , yylineno);
											emit(jump , NULL , NULL , NULL , 0 , yylineno);

											label = next_quad_label();

											
											ttl = $expr->truelist ;
											tfl = $expr->falselist ;
											$expr->truelist = tfl;
											$expr->falselist = ttl; 
											
											$expr->type = boolexpr_e;
           								}

										if($expr->type == boolexpr_e){

											$returnstmt = complete($expr);

											if(infunctioncounter > 0){
												emit(ret ,NULL ,$returnstmt , NULL , 0 , yylineno);
											}
											else{
												fprintf(errors,"Syntax error, return not in a function ,at line %d\n",yylineno);
											}
										}
										else{
											if(infunctioncounter > 0){
												emit(ret ,NULL ,$expr , NULL , 0 , yylineno);
											}
											else{
												fprintf(errors,"Syntax error, return not in a function ,at line %d\n",yylineno);
											}
										}

									}
		|	KEYWORD_RETURN SEMI
								{
									
									if(infunctioncounter > 0){
										emit(ret ,NULL ,NULL , NULL , 0 , yylineno);
									}
									else{
										fprintf(errors,"Syntax error, return not in a function ,at line %d\n",yylineno);
									}								
								}
        ;

%%


symbol *lvalue_node(struct alpha_token_t*token , int scope , SymType type1 ){
	symbol*node = malloc(sizeof(symbol));

	memset(node , 0 , sizeof(symbol));

	node->hide = 0 ;
	node->type1 = type1 ;
	node->type2 = match_type1_to_type2(type1);

	node->value = malloc(sizeof(DATA));
	memset(node->value , 0 , sizeof(DATA));
	node->value->name = strdup(token->content);
	node->value->scope = scope;
	node->value->line = token->numline;

	
	node->next = NULL ;
	return node ;

}

symbol* make_symbol(DATA* data , SymType type1){
	symbol*node = malloc(sizeof(symbol));
	memset(node , 0 , sizeof(symbol));
	node->hide = 0 ;
	node->type1 = type1 ;	
	node->type2 = match_type1_to_type2(type1);
	node->value = data ;
	node->next = NULL;
	return node ;
}

symbol* id_for_primary(symbol* data){
	symbol * ptr , *ptr2;
	int no_valid ;
	if(data->type1 == ERROR){
		return data;
	}
	ptr = SymTable_lookup_general(symtable , data->value->name ,data->value->scope );
	if(ptr == NULL){
		ptr = insert(data) ;
		return ptr ;
	}
	else{
		ptr->type2 = match_type1_to_type2(ptr->type1);
		
		//perfect condition/found in global
		if (ptr->value->scope == 0){
			return ptr ;
		}
		//check if valid 
		else if(ptr->value->scope > 0 ){
			no_valid = search_for_function(symtable , scope , ptr->value->scope );
			if(no_valid == 1 && ptr->type1 != LIBFUNC && ptr->openfuncs != openfuncs){
				fprintf(errors , "syntax error, cannot access symbol \"%s\" at line %d\n", data->value->name, data->value->line);
				++ syntax_errors_counter;
				data->type1 = ERROR;
				data->type2 = error_s ;
				return data;
			}
			return ptr;
		}
		else{
			fprintf(stderr,"id for prim assertion [%s , %d]\n",data->value->name ,yylineno);
			assert(0); 
		}

	}

}

symbol* insert(symbol* data){

	symbol* ptr = NULL;

	if(scope == 0 && (data->type1 == LOCAL || data->type1 == GLOBAL)) 
		data->type1 = GLOBAL;

	if(check_library(data->value->name) ){
		ptr =SymTable_insert(symtable , data->value->name, data->type1 ,data->value->scope , data->value->line);
		ptr->type2 = match_type1_to_type2(data->type1);
		ptr->space =  curr_scope_space();
		ptr->offset = curr_scope_offset();
		ptr->openfuncs = openfuncs ;
		in_curr_scope_offset();
		free(data);
	}
	else{
		fprintf(errors,"syntax error, at line %d trying to shadow \"%s\" \n",data->value->line , data->value->name); //this is for assignment
		++ syntax_errors_counter;
		data ->type1 = ERROR ;
		data->type2 = error_s;
		return data ;
		
	}
	
	return ptr ;
}

symbol* id_for_assign(symbol* data , int member_item){

	int no_valid ;
	symbol* ptr , *ptr2 ;

	if(data->type1 == ERROR){
		return data;
	}
	
	ptr = SymTable_lookup_general(symtable , data->value->name ,data->value->scope );

	if(ptr == NULL){
		//insert  and free
		ptr = insert(data) ;
		return ptr ;
	}
	else{
		ptr->type2 = match_type1_to_type2(ptr->type1);

		if((ptr->type1 == LIBFUNC || ptr->type1 == USERFUNC )&& member_item == 0){
			fprintf(errors,"syntax error,cannot assign lvalue is a function \"%s\",at line %d\n",data->value->name ,yylineno);
			++ syntax_errors_counter;
			ptr2 = malloc(sizeof(symbol));
			ptr2->hide = 0 ;
			ptr2->type1 = GLOBAL ;
			ptr2->type2 = error_s ;
			ptr2->value = malloc(sizeof(DATA));
			ptr2->value->name = strdup(data->value->name);
			ptr2->value->scope = data->value->scope;
			ptr2->value->line = data->value->line;
			return ptr2 ;
		}
		if(member_item == 1)
		{
			//perfect condition/found in global
			if (ptr->value->scope == 0){
				return ptr ;
			}
			//check if valid 
			else if(ptr->value->scope > 0 ){
				no_valid = search_for_function(symtable , scope , ptr->value->scope );
				if(no_valid == 1 &&  ptr->openfuncs != openfuncs){
					fprintf(errors , "syntax error, cannot access symbol \"%s\" at line %d\n", data->value->name, yylineno);
					++ syntax_errors_counter;
					ptr2 = malloc(sizeof(symbol));
					ptr2->hide = 0 ;
					ptr2->type1 = GLOBAL ;
					ptr2->type2 = error_s ;
					ptr2->value = malloc(sizeof(DATA));
					ptr2->value->name = strdup(data->value->name);
					ptr2->value->scope = data->value->scope;
					ptr2->value->line = data->value->line;
					return ptr2 ;
				}
				return ptr;
			}
			else{
				fprintf(stderr,"id for assign assertion [%s , %d]\n",data->value->name ,yylineno);
				assert(0);
			}
		}
		else{
			return ptr;
		}
	}

}

symbol* id_for_term(symbol * data, char*oper){
	
	//error if data is func
	//else insert
	int no_valid ;
	symbol* ptr , *ptr2 ;
	if(data->type1 == ERROR){
		return data;
	}
	
	ptr = SymTable_lookup_general(symtable , data->value->name ,data->value->scope );
	if(ptr == NULL){
		//insert  and free
		ptr = insert(data) ;
		
		return ptr ;
	}
	else{
		ptr->type2 = match_type1_to_type2(ptr->type1);

		if(ptr->type1 == LIBFUNC || ptr->type1 == USERFUNC ){
			fprintf(errors,"syntax error,cannot %s a function \"%s\",at line %d\n",oper,data->value->name ,yylineno);
			++ syntax_errors_counter;
			data->type1 = ERROR;
			data->type2 = error_s ;
			return data ;
		}
		
		//perfect condition/found in global
		if (ptr->value->scope == 0){
			return ptr ;
		}
		//check if valid 
		else if(ptr->value->scope > 0 ){
			no_valid = search_for_function(symtable , scope , ptr->value->scope );
			if(no_valid == 1 && ptr->openfuncs != openfuncs){
				fprintf(errors , "syntax error, cannot access symbol \"%s\" at line %d\n", data->value->name, yylineno);
				++ syntax_errors_counter;
				data->type1 = ERROR;
				data->type2 = error_s ;
				return data;
			}
			return ptr;
		}
		else{
			fprintf(stderr,"id for term assertion [%s , %d]\n",data->value->name ,yylineno);
			assert(0);
		}
	}

}

symbol* id_for_member(symbol* data){
	

	symbol * ptr , *ptr2;
	int no_valid ;
	if(data->type1 == ERROR){
		return data;
	}
	ptr = SymTable_lookup_general(symtable , data->value->name ,data->value->scope );

	if(ptr == NULL){
		//insert  and free
		ptr = insert(data) ;
		return ptr ;
	}
	else{
		ptr->type2 = match_type1_to_type2(ptr->type1);

		//perfect condition/found in global
		if (ptr->value->scope == 0){
			return ptr ;
		}
		//check if valid 
		else if(ptr->value->scope > 0 ){
			no_valid = search_for_function(symtable , scope , ptr->value->scope );
			if(no_valid == 1 &&  ptr->openfuncs != openfuncs){
				fprintf(errors , "syntax error, cannot access symbol \"%s\" at line %d\n", data->value->name, data->value->line);
				++ syntax_errors_counter;
				data->type1 = ERROR;
				data->type2 = error_s ;
				return data;
			}
			return ptr;
		}
		else{
			fprintf(stderr,"id for member assertion [%s , %d]\n",data->value->name ,yylineno);
			assert(0);
		}
	}
}

symbol * id_for_call(symbol* data){

	symbol * ptr , *ptr2;
	int no_valid ;
	if(data->type1 == ERROR){
		return data;
	}
	ptr = SymTable_lookup_general(symtable , data->value->name ,data->value->scope );
	
	if(ptr == NULL){
		//insert  
		ptr = insert(data) ;
		
		return ptr ;
	}
	else{
		ptr->type2 = match_type1_to_type2(ptr->type1);

		//perfect condition/found in global
		if (ptr->value->scope == 0){
			ptr->type2 = match_type1_to_type2(ptr->type1);
			return ptr ;
		}
		//check if valid 
		else if(ptr->value->scope > 0 && ptr->type1 != USERFUNC && ptr->type1 != LIBFUNC){
			no_valid = search_for_function(symtable , scope , ptr->value->scope );
			if(no_valid == 1 && ptr->type1 != LIBFUNC && ptr->openfuncs != openfuncs){
				fprintf(errors , "syntax error, cannot access symbol \"%s\" at line %d\n", data->value->name, data->value->line);
				++ syntax_errors_counter;
				data->type1 = ERROR;
				data->type2 = error_s ;
				return data;
			}
			return ptr;
		}
		else if(ptr->value->scope > 0 && (ptr->type1 == USERFUNC || ptr->type1 == LIBFUNC)){
			return ptr ;
		}
		
		else{
			fprintf(stderr,"id for call assertion [name %s ,type %d , line %d]\n",data->value->name ,data->type1,yylineno);
			return ptr ;
		}
	}
}

symbol* insert_func(symbol* data, int scope){
	int valid ;
	symbol * ptr;
	ptr = SymTable_lookup_scope(symtable,data->value->name, scope );
	if(ptr == NULL){
		//check for library 
		if(check_library(data->value->name) ){
			ptr =SymTable_insert(symtable , data->value->name, data->type1 ,data->value->scope , data->value->line);
			ptr->space =  curr_scope_space();
			ptr->offset = curr_scope_offset();
			ptr->openfuncs = openfuncs ;
			in_curr_scope_offset();
			free(data);
			ptr->type2 = match_type1_to_type2(ptr->type1);

		}
		else{
			fprintf(errors,"syntax error, at line %d trying to shadow \"%s\" \n",data->value->line , data->value->name); //this is for assignment
			++ syntax_errors_counter;
			data->type1 = ERROR ;
			data->type2 = error_s;
			return data ;
			
		}

		return ptr ;
	}
	else{
		if(ptr->type1 == GLOBAL || ptr->type1 == LOCAL || ptr->type1 == FORMAL){
			fprintf(errors , "syntax error, try to define function \"%s\" at line %d ,but has already defined as a variable in this scope(%d) at line %d\n",data->value->name , data->value->line ,ptr->value->scope, ptr->value->line);
			++ syntax_errors_counter;
		}
		else {
			fprintf(errors , "syntax error, try to redefine function \"%s\" at line %d ,but has already defined in this scope(%d) at line %d\n",data->value->name , data->value->line ,ptr->value->scope, ptr->value->line);
			++ syntax_errors_counter;
		}
		ptr = malloc(sizeof(symbol));
		memset(ptr , 0 , sizeof(symbol));
		ptr->value = malloc(sizeof(DATA));
		ptr->value->name = strdup(data->value->name);
		ptr->value->scope = currscope();
		ptr->value->line = data->value->line;
		ptr->type1 = ERROR ;
		ptr->type2 = error_s ;
	
		return ptr ;
	}
}

char* unnamed_func_id(){
	int i , tmp = unnamed ;
	int count = 0 ;
	char* name , *num ;

	while (tmp != 0) {
		tmp /= 10;    
		++count;
    }
	num = malloc(count * sizeof(char));
	itoa(unnamed , num);
	name = malloc ( (count + 2 + 1) * sizeof(char) );
	name[0] = '_';
	name[1] = 'F';
	for(i = 0 ; i < count ; i ++){
		name[i+2] = num[i];
	}
	name[i+2] = '\0';
	free(num);
	return name ;
}

symbol* check_for_local_ID(struct alpha_token_t*token , int scope){

	symbol * ptr;
	int valid ;
	ptr = SymTable_lookup_scope(symtable,token->content , scope );

	//does not exist
	if(ptr == NULL){
		//check for library 
		ptr = insert_tok(token , LOCAL) ;
	}
	
	return ptr ;
}

symbol* check_for_global_ID(struct alpha_token_t*token , int scope){
	symbol * ptr;
	int valid ;
	ptr = SymTable_lookup_scope(symtable,token->content , 0 );
	
	//does not exist
	if(ptr == NULL){
		fprintf(errors,"syntax error,undefined global symbol %s at line %d\n",token->content , token->numline);
		++ syntax_errors_counter;
		ptr = (symbol*) malloc(sizeof(symbol));
		memset(ptr , 0 , sizeof(symbol));
		ptr->value = malloc(sizeof(DATA));
		ptr->value->name = strdup(token->content);
		ptr->value->scope = currscope();
		ptr->value->line = token->numline;
		ptr->type1 = ERROR ;
		ptr->type2 = error_s ;
	}
	return ptr ;
}

symbol* lookup_tmp(char*name , int scope){
	return SymTable_lookup_scope(symtable , name  , scope);
}

symbol* insert_tmp(char* name ){
	return SymTable_insert(symtable , name , LOCAL , currscope() , yylineno) ;
}

int find_arguments(){
	int arg_no = 0;
	//ARG _BEGIN  is a global pointer to the left pareth ( , arguments can be only IDs 
	struct alpha_token_t *prev_tok, *current_tok = ARG_BEGIN;
	symbol * exist;
	//curr = left parethesis -> next => first arg
	current_tok = current_tok->alpha_yylex;      
	//prev = first arg  
	prev_tok = current_tok ;
	//if first arg is rigth parenth, no arguments
	if(prev_tok == yylval.value )
		return arg_no;
	current_tok = current_tok->alpha_yylex;
	
	while(current_tok != yylval.value){
		if(strcmp(prev_tok->content,",") != 0 ){
			//all the arguments are new ,so insert them no need for lookup
			if(strcmp(prev_tok->type , "IDENTIFICATION") != 0)
				
				// this is error but in phase 4 i do not check it becareful
				
				return 0 ; 
			exist = SymTable_lookup_scope(symtable, prev_tok->content , scope);
			if(exist == NULL){
				insert_tok(prev_tok , FORMAL);
				++arg_no;
		
			}
			else{ 
				fprintf(errors,"syntax error,formal argument: \"%s\",at line %d , already defined in this scope \n", prev_tok->content,prev_tok->numline);
				++ syntax_errors_counter;
			}
		}
		prev_tok = current_tok ;
		current_tok = current_tok->alpha_yylex ;
	}

	if(strcmp(prev_tok->content,",") != 0 ){    
		if(strcmp(prev_tok->type , "IDENTIFICATION") != 0)
				
				// this is error but in phase 4 i do not check it becareful
				
				return 0 ;    
		exist = SymTable_lookup_scope(symtable, prev_tok->content , scope);
		if(exist == NULL){
			//fprintf(stderr,"lets insert formal arg %s\n",prev_tok->content);
			insert_tok(prev_tok , FORMAL);
			++arg_no;

		}
		else{
			fprintf(errors,"syntax error,formal argument: \"%s\",at line %d , already defined in this scope \n", prev_tok->content,prev_tok->numline);
			++ syntax_errors_counter;
		}
	}
	return arg_no ;
}

symbol_t match_type1_to_type2(SymType type){
	symbol_t result;
	switch(type){
		case GLOBAL	:		result = var_s; break;		
		case LOCAL	:		result = var_s; break;
		case FORMAL	:		result = var_s; break;
		case USERFUNC:		result = programfunc_s; break;
		case LIBFUNC :		result = libraryfunc_s; break;
		case ERROR	:		result = error_s; break;
		default :	assert(0);
	}
	return result;
}

symbol* insert_tok(struct alpha_token_t*token, SymType type){

	symbol* ptr = NULL;
	if(scope == 0 && (type == LOCAL || type == GLOBAL)) type = GLOBAL;
	if(check_library(token->content) ){
		//fprintf(stderr,"lets define token %s at line %d\n",token->content,token->numline);
		ptr =SymTable_insert(symtable , token->content, type , scope , token->numline);
		ptr->type2 = match_type1_to_type2(type);
		ptr->space =  curr_scope_space();
		ptr->offset = curr_scope_offset();
		ptr->openfuncs = openfuncs ;
		in_curr_scope_offset();
	}else {
		fprintf(errors,"syntax error,at line %d trying to shadow \"%s\" \n",token->numline , token->content); //this is for assignment
		++ syntax_errors_counter;
		ptr = malloc(sizeof(symbol));
		memset(ptr , 0 , sizeof(symbol));
		ptr->value = malloc(sizeof(DATA));
		ptr->value->name = strdup(token->content);
		ptr->value->scope = currscope();
		ptr->value->line = token->numline;
		ptr->type1 = ERROR ;
		ptr->type2 = error_s;
	}
	return ptr ;
}

int yyerror(void *ylval ,const char* msg){
    fprintf(errors,"%s,at line: %d\n",msg , yylineno );
	++ syntax_errors_counter;
    return 0;
}

int main(int argc , char**argv){
    extern FILE *yyin, *yyout , *errors , *SYMBOLS , *quadfile; 
    extern struct alpha_token_t *HEAD , *QUE ;
    int try =  0;
    
	symtable = SymTable_new();
    
	errors = stderr;
    SYMBOLS =stderr;// fopen("outputs/symtable.out" , "w");
    rulefile = fopen("outputs/grammar.out" , "w");
	quadfile =stderr;// fopen("outputs/quads.out" , "w");


    HEAD = malloc(sizeof(struct alpha_token_t));
    HEAD->numline = 0 ; 
    HEAD->numToken = 0 ;
    HEAD->content = NULL ;
    HEAD->type = NULL ;
    HEAD->alpha_yylex = NULL;

    QUE = HEAD ;

	temp_var_Stack = createStack();
	scope_offset_Stack = createStack();
	loop_counter_Stack = createStack();

	if(argc < 2) {
		printf("-Using stdin-\n");
		yyin = stdin;
	}
    else if(argc == 2){
         
        if( (yyin = fopen(argv[1], "r")) == NULL)    {printf("Error open %s \n", argv[1]) ; exit(-1);}

        yyout = stdout ;
    }


    while( yyparse(HEAD));

	fprintf(stderr,"\n\n");
    SymTable_print(symtable);

	if(syntax_errors_counter == 0 ){
		fprintf(quadfile , "\n-FORMAT-\nlineNo\top\t[result]\t[arg1]\t[arg2]\n");
		print_quads();
		create_tcode();
	}
	


	fclose(rulefile);
    fclose(errors);
    fclose(yyin); 
    fclose(yyout);
    
	// free_all() ;
    return 0 ;
} 
