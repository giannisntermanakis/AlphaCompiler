#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "../headerfiles/i_code_datatypes.h"
#include "../headerfiles/symtable.h"
#define HASH_MULTIPLIER 65599
#define BUCKETS 100
static void insert_library(SymTable_T oSymTable) ;
extern FILE *SYMBOLS ,*errors;


/*return the bucket for this ident*/
static unsigned int SymTable_hash( int scope){
	unsigned int ui ;
	size_t uiHash = 0U ;

	uiHash = scope ;
	uiHash = uiHash % BUCKETS;
		/*printf("%s->%d\n",ident,uiHash);*/

	return uiHash;
}
/*Returns a new table*/
SymTable_T SymTable_new(void){
	int i = 0 ;
	SymTable_T newTable ;
	newTable = malloc(sizeof(struct SymTable) );
	newTable->lines = malloc(sizeof(symbol* ) * BUCKETS );
	for( i = 0 ; i < BUCKETS ; i++){
		newTable->lines[i] = NULL ;
	}
	insert_library(newTable);
	return newTable ;
}
/*free all teh hashtb*/
void SymTable_free(SymTable_T oSymTable){/*free all the memory*/
	
}

/*insert a new element*/
symbol* SymTable_insert(SymTable_T oSymTable , const char *name,SymType type1 ,unsigned int scope ,unsigned int numline ){
	unsigned int slot ;
	symbol *pos ;
	symbol *prev ;
	symbol *temp ;
	assert(name);
	assert(oSymTable);
	slot = SymTable_hash( scope );
	pos = oSymTable->lines[slot];

	if(pos == NULL){
		/*insert*/
		oSymTable->lines[slot] = malloc(sizeof(symbol));
		memset(oSymTable->lines[slot] , 0 , sizeof(symbol));

		oSymTable->lines[slot]->hide =  0 ;
		oSymTable->lines[slot]->type1 = type1 ;

		oSymTable->lines[slot]->value = malloc(sizeof(DATA));

		oSymTable->lines[slot]->value->name = (char*) name ;
		oSymTable->lines[slot]->value->scope =  scope ;
		oSymTable->lines[slot]->value->line =  numline ;
		
		oSymTable->lines[slot]->next = NULL ;
		return  oSymTable->lines[slot] ;
	}

	while(pos != NULL){
		
		
		if(strcmp(pos->value->name , name) == 0 && pos->value->scope == scope && pos->hide == 0 ){
			//printf("already exists as variable no insert\n");
			return NULL ;
		}
		
		prev = pos ;
		pos = prev->next ;
	}
	/*insert*/
	temp = malloc(sizeof(symbol));
	memset(temp, 0 , sizeof(symbol) );
	temp->hide = 0 ;
	temp->type1 = type1 ;
		temp->value = malloc(sizeof(DATA));

		temp->value->name = (char*) name ;
		temp->value->scope = scope ;
		temp->value->line = numline ;
	
	temp->next =NULL ;
	prev->next = temp ;
	return temp;

}
/*return 1 if contains,0 if not*/
symbol* SymTable_lookup_scope(SymTable_T oSymTable , const char *name ,unsigned int scope){
	unsigned int slot ;
	symbol *pos ;

	assert(oSymTable);
	assert(name);

	slot = SymTable_hash( scope );
	pos = oSymTable->lines[slot];
	while(pos != NULL){
        /*check the scope too*/
 
		if(strcmp(pos->value->name ,name) == 0 && pos->value->scope == scope  && pos->hide == 0 )
		{
			//fprintf(stderr,"found ->id (%s), type1 (%d), numline (%d), scope (%d), hide (%d)\n",pos->value->name,pos->type1,pos->value.varVal->line,pos->value.varVal->scope,pos->hide);
			return pos;
		}
		pos = pos->next ;
		
	}
	return NULL ;
}
symbol* SymTable_lookup_general(SymTable_T oSymTable , const char *name , int scope){
	unsigned int slot ;
	symbol *pos ;

	assert(oSymTable);
	assert(name);

	while(scope >= 0){
		slot = SymTable_hash(scope );
		pos = oSymTable->lines[slot];
		while(pos != NULL ){
			/*check the scope too*/

			if(strcmp(pos->value->name ,name) == 0 && pos->value->scope == scope &&pos->hide == 0 )
			{
				//fprintf(stderr,"found ->id (%s), type1 (%d), numline (%d), scope (%d), hide (%d)\n",pos->value.varVal->name,pos->type1,pos->value.varVal->line,pos->value.varVal->scope,pos->hide);
				return pos;
			}
			pos = pos->next ;
			
		}
		scope--;
	}
	return NULL ;
}
/*remove one element*/
int SymTable_hide(SymTable_T oSymTable  , unsigned int scope){
	unsigned int slot ;
	int index = 0;
	symbol *pos ;
	symbol *prev ;
	assert(oSymTable);

	slot = SymTable_hash(scope );
	pos = oSymTable->lines[slot];
	while(pos != NULL){
		
		if( pos->value->scope == scope ){
			pos->hide = 1 ;
		}
		prev = pos ;
		pos = prev->next ;
		index++;
		
	}
	return 0 ;
}
void SymTable_print(SymTable_T oSymTable){/*free all the memory*/
	int i = 0 ;
	symbol *temp ;
	symbol *p ;
	char* tpe ;
	if(oSymTable == NULL){
		return ;
	}
	for(i = 0 ; i < BUCKETS ; i++){
		if(oSymTable->lines[i] != NULL){
			fprintf(SYMBOLS,"------------------------------SCOPE_%d----------------------------\n",i);
			p = oSymTable->lines[i] ;
			while(p != NULL){
				if(p->type1 == GLOBAL)
					tpe = "GLOBAL";
				else if(p->type1 == LOCAL)
					tpe = "LOCAL";
				else if(p->type1 == FORMAL)
					tpe = "FORMAL";
				else if(p->type1 == USERFUNC)
					tpe = "USERFUNC";
				else if(p->type1 == LIBFUNC)
					tpe = "LIBFUNC";

				fprintf(SYMBOLS,"\"%s\",\t[\"%s\"],\tline:\"%d\"\toffset %d\tscopespace %d\n",p->value->name,tpe,p->value->line,p->offset,p->space);
				p = p->next;
			}
		}
			
	}
}
	
int search_for_function(SymTable_T sym , int curr_sc , int old_sc ){
	unsigned int slot;
	symbol* pos;
	while(curr_sc-1 >= old_sc){

		slot = SymTable_hash( curr_sc-1);
		pos = sym->lines[slot];
		while(pos != NULL){
			if(pos->type1 == USERFUNC && pos->value->scope == curr_sc-1 ){
				return 1;
			}
			pos = pos->next ;
		}
		curr_sc--;
	}
	return 0;
}


static void insert_library(SymTable_T oSymTable){
	SymTable_insert(oSymTable , "print" ,LIBFUNC ,0 , 0 );
	SymTable_insert(oSymTable , "input" ,LIBFUNC ,0 , 0 );
	SymTable_insert(oSymTable , "objectmemberkeys" ,LIBFUNC ,0 , 0 );
	SymTable_insert(oSymTable , "objecttotalmembers" ,LIBFUNC ,0 , 0 );
	SymTable_insert(oSymTable , "objectcopy" ,LIBFUNC ,0 , 0 );
	SymTable_insert(oSymTable , "totalarguments" ,LIBFUNC ,0 , 0 );
	SymTable_insert(oSymTable , "argument" ,LIBFUNC ,0 , 0 );
	SymTable_insert(oSymTable , "typeof" ,LIBFUNC ,0 , 0 );
	SymTable_insert(oSymTable , "strtonum" ,LIBFUNC ,0 , 0 );
	SymTable_insert(oSymTable , "sqrt" ,LIBFUNC ,0 , 0 );
	SymTable_insert(oSymTable , "cos" ,LIBFUNC ,0 , 0 );
	SymTable_insert(oSymTable , "sin" ,LIBFUNC ,0 , 0 );
	SymTable_insert(oSymTable , "concat" ,LIBFUNC ,0 , 0 );
return ;
}
//return 0 if shadows , 1 else 
int check_library(const char* name){
	if(strcmp(name , "print") == 0 
	|| strcmp(name , "input") == 0 
	|| strcmp(name , "objectmemberkeys") == 0 
	|| strcmp(name , "objectcopy") == 0 	
	|| strcmp(name , "totalarguments") == 0 
	|| strcmp(name , "argument") == 0 
	|| strcmp(name , "typeof") == 0 
	|| strcmp(name , "strtonum") == 0 
	|| strcmp(name , "sqrt") == 0 
	|| strcmp(name , "cos") == 0 
	|| strcmp(name , "sin") == 0 )
	{
		return 0 ;
	}
	return 1 ;
}