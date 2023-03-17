
//csd4393,csd4225,csd4226
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "../headerfiles/avm_datatypes.h"
#include "../headerfiles/avm_functions.h"

#define PI 3.14159265359
#define MAXINPUT 200
#define GLOBAL_AREA 0


/*
----------------
(Given):
print
typeof
totalarguments


done: print ,typeof, cos , sin , sqrt , input ,totalarguments ,onjecttotalmembers,strtonum , concat ,objectcopy
problem:  objectmemberkeys , argument
-----------------
*/
void libfunc_print (void){
    unsigned n = avm_totalactuals();
    unsigned i;
    for ( i = 0 ; i < n ; i++){
        char *s = avm_tostring(avm_getactual(i));
        // puts(s);
        fprintf(outfile,"%s",s);
        // if true of false s is not malloced
        if(s != NULL) 
          free(s);
    }
    fprintf(outfile,"\n");
}

// ok
void libfunc_typeof (void){
  
  unsigned int n = avm_totalactuals();

  if(n != 1){
      fprintf(avm_errors, "one argument (not %d) expected in 'typeof'!\n", n);
      executionFinished = 1;
  }else{
      avm_memcellclear(&retval);
      retval.type = string_m;
      retval.data.strVal = strdup(typeString[avm_getactual(0)->type]);
  }

}

/* Math libs*/
void libfunc_sqrt (void){
  
  avm_memcell *temp;
  double ret;
  unsigned int n = avm_totalactuals();

  if (n != 1 ) {
    fprintf(avm_errors, "one argument (not %d) expected in 'sqrt'!\n", n);
    executionFinished = 1;
  }
  else{
      
    avm_memcellclear(&retval);
    temp = avm_getactual(0);
    if( temp-> type == number_m && temp -> data.numVal >0){
      ret = sqrt(temp->data.numVal);
      retval.type = number_m;
      retval.data.numVal = ret;
    }
    else{
      retval.type = nil_m;
    }
  }
  
}

void libfunc_cos (void){
  avm_memcell *temp;
  double ret;
  unsigned int n = avm_totalactuals();

  if (n != 1 ) {
    fprintf(avm_errors, "one argument (not %d) expected in 'sqrt'!\n", n);
    executionFinished = 1;
  }
  else{

    avm_memcellclear(&retval);
    temp = avm_getactual(0);
      if( temp-> type == number_m ){
        ret = cos(temp->data.numVal * (double)(PI /180.0));
        retval.type = number_m;
        retval.data.numVal = ret;
      }else {
        retval.type = nil_m;
      }
  }
  
}

void libfunc_sin (void){
  avm_memcell *temp;
  double ret ;
  unsigned int n = avm_totalactuals();

  if (n != 1 ) {
    fprintf(avm_errors, "one argument (not %d) expected in 'sqrt'!\n", n);
    executionFinished = 1;
  } else {

    avm_memcellclear(&retval);
    temp = avm_getactual(0);
      if( temp-> type == number_m ){
        ret = sin(temp->data.numVal *(double) (PI /180.0));
        retval.type = number_m;
        retval.data.numVal = ret;
      }else {
        retval.type = nil_m;
      }
  }
}

//check for invalid ascii characters < ' ' or > '~'
void read_stdin(FILE* file, char* buffer, int max) {
    char c;
    int index;

    for(index = 0; index < (MAXINPUT - 1); ++index){
        c = getc(stdin);

        if (c == EOF || c == '\n') {
            break;
        }
        if(c >= ' ' && c <= '~'){
            buffer[index] = c;
        }
        else{
            fprintf(avm_errors,"error, input of dangerous character!\n");
            executionFinished = 1 ;
            return;
        }
    }

    buffer[index] = '\0';
    return ;
    
}

void libfunc_input(){
    char *buffer;
    double num = 0;

    buffer = (char *)malloc(MAXINPUT * sizeof(char));
    //check for invalid ascii characters < ' ' or > '~'
    read_stdin(stdin, buffer, MAXINPUT);
    if(executionFinished){
        memset(buffer,0,MAXINPUT * sizeof(char));
        free(buffer);
        return;
    }
    //string to num conversion
    num = atof(buffer);
    avm_memcellclear(&retval);

    //case number
    
    if(buffer[0]=='0'&&buffer[1]==0){
        retval.type  = number_m;
        retval.data.numVal = 0;
    }
    else if  (num != 0 ){
      retval.type = number_m;
      retval.data.numVal = num;
    }
    //case bool
    else if(strcmp(buffer , "true")==0) {
        retval.type = bool_m;
        retval.data.boolVal = 1;
        }
    else if (strcmp(buffer , "false")==0){
        retval.type = bool_m;
        retval.data.boolVal = 0;
    }
    //case nil
    else if (strcmp(buffer , "nil")==0 ){
          retval.type = nil_m;
    }
    //case string
    else{
        retval.type = string_m;
        retval.data.strVal = strdup(buffer);
    }
    if(buffer != NULL){
        memset(buffer,0,MAXINPUT * sizeof(char));
        free(buffer);
    }
    return;

}

// ok
void libfunc_objecttotalmembers(){

  struct avm_memcell *temp;
  unsigned  n = avm_totalactuals();

  if (n != 1 ) {
    fprintf(avm_errors, "one argument (not %d) expected in 'sqrt'!\n", n);
    executionFinished = 1;
  }else{
    avm_memcellclear(&retval);
    temp = avm_getactual(0);
    retval.type = number_m;
    if(temp -> type == table_m){
      retval.data.numVal = (double) temp ->data.tableVal ->total;
    }else{
      retval.data.numVal = 0;
    }

  }

}
// ok
void libfunc_totalarguments (void){



  avm_memcell sum;
  unsigned n = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);
  avm_memcell temp;
  temp.data.numVal = avm_get_envvalue(n + AVM_NUMACTUALS_OFFSET);
  avm_memcellclear(&retval);
  
  
  if (!(topsp - GLOBAL_TEMP +4)) {
    fprintf(avm_errors, "called 'totalarguments' outside a function!\n");
    retval.type = nil_m;
	// executionFinished = 1;
  }
  else{
      
    retval.type = number_m;
    retval.data.numVal = avm_get_envvalue(n + AVM_NUMACTUALS_OFFSET);
  }
}


//ta pinei sto global
void libfunc_argument(void){

  int arg;
  unsigned int n ;
  struct avm_memcell *temp;
  unsigned int p;
unsigned int l;
   avm_memcell sum;
  

  if(!(topsp - GLOBAL_TEMP +5)){
    fprintf(avm_errors, "called 'argument' outside a function!\n");
    retval.type = nil_m;
	// executionFinished = 1;
  return;
  
  }
	n = avm_totalactuals();
	p = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);

  
  avm_memcellclear(&retval);
    
  if(n != 1){
      fprintf(avm_errors, "one argument (not %d) expected in 'argument'!\n", n);
      executionFinished = 1;
      return;
    }
  
  
  if( !p ){
    fprintf(avm_errors,"error, 'argument' called outside a function!");
    // avm_error("'totalarguments' called outside a function!");
    retval.type = nil_m;
    return;
  }else{
    temp = avm_getactual(0);
    if(temp -> type != number_m){
      retval.type = nil_m;
      return;
    }else{
      
      arg = temp->data.numVal; 
      l =  avm_get_envvalue(n + AVM_NUMACTUALS_OFFSET);      

      if(&STACK[p + AVM_STACKENV_SIZE + 1 + arg]){
        
        temp = &STACK[p + AVM_STACKENV_SIZE + 1 + arg];
        if(temp -> type == number_m){
          if(temp->data.numVal == 0 ){
              avm_assign(&retval , temp);
          }else {
            retval.type = nil_m;
          }
        }
        avm_assign(&retval , temp);
      }else{
        retval.type = nil_m;
      }
    }
  }
}

//ok
void    libfunc_strtonum(void){
	unsigned int n = avm_totalactuals();
  	double ret ;
  	char *actual;

    if (n != 1 ) {
      fprintf(avm_errors, "one argument (not %d) expected in 'sqrt'!\n", n);
      executionFinished = 1;
    } else {
      avm_memcellclear(&retval);
      actual = avm_tostring(avm_getactual(0));
      ret = atof(actual);

      if(ret == 0 ){
        if(strcmp(actual , "0")== 0 ){
            retval.type = number_m;
            retval.data.numVal = ret;
        }else{
          retval.type = nil_m;
        }
      }else{
        retval.type = number_m;
        retval.data.numVal = ret;
      }
        
    }
}

// ok
void    libfunc_objectcopy(void){
  
  unsigned int n = avm_totalactuals();
  avm_memcell *temp;
  int i;
  
  if(n != 1){
    fprintf(avm_errors, "one argument (not %d) expected in 'sqrt'!\n", n);
    executionFinished = 1;
  }

  avm_memcellclear(&retval);
  temp = avm_getactual(0);

    if(temp -> type != table_m){
      retval.type = nil_m;
      return;
    }else{
	  avm_assign(&retval , temp);	  
    }
}

//ok
void libfunc_concat(void){
  unsigned i, n = avm_totalactuals();
  unsigned l1 ,l2;
  avm_memcell *t1;
  avm_memcell *t2;
  char *s1;
  char *s2;
  char *temp;

  if(n!=2){
    fprintf(avm_errors, "two arguments (not %d) expected in 'concat'!\n",n);
    executionFinished = 1;
  }else {
    avm_memcellclear(&retval);
    t1 = avm_getactual(0);    
    t2 = avm_getactual(1);

  if(t1->type != string_m || t2-> type != string_m){
    retval.type = nil_m;
  }
	else{
    	s1 = t1 -> data.strVal;
    	s2 = t2 -> data.strVal;
	    assert(s1 && s2);

		l1 = strlen(s1);
		l2 = strlen(s2);
		temp = (char*)malloc(sizeof(char)* (l1+l2+1-2)); //-2 cause of the removed ""
    memset(temp , 0 , (l1+l2+1-2)*sizeof(char));
    
		for(i = 0 ; i < l1-1 ; ++i){
			temp[i] = s1[i];
		}
		for(i = 1 ; i < l2 ; ++i){
			temp[i+l1-1-1] = s2[i];
		}
		  temp[i+l1] = '\0';
	    retval.type = string_m;
    	retval.data.strVal = temp;
    }
  }
}

void libfunc_objectmemberkeys(void){

  fprintf(avm_errors, "Error. ObjectMemberKeys not supported!\n");
  avm_memcellclear(&retval);
  retval.type = nil_m;
    
}