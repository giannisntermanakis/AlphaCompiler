#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../headerfiles/avm_datatypes.h"
#include "../headerfiles/avm_functions.h"


// ok use switch for dispatching
char* avm_tostring(avm_memcell* m){
    char *result;
    assert(m->type >= 0 && m->type <= undef_m);
    switch(m->type){
        case number_m:
            result = number_tostring(m);
            break;
        case string_m:
            result = string_tostring(m);
            break;
        case bool_m:
            result = bool_tostring(m);
            break;
        case table_m:
            result = table_tostring(m);
            break;
        case userfunc_m:
            result = userfunc_tostring(m);
            break;
        case libfunc_m:
            result = libfunc_tostring(m);
            break;
        case nil_m:
            result = nil_tostring(m);
            break;
        case undef_m:
            result = undef_tostring(m);
            break;
    }
    return result;
}
// ok
char* number_tostring(avm_memcell* m){
    char *result;
    if(result = (char *)malloc(AVM_STRING_BUFFER * sizeof(char))){
        memset(result , 0 ,AVM_STRING_BUFFER * sizeof(char) );
        sprintf(result, "%f", m->data.numVal);
        return result;

    }
    fprintf(stderr,"malloc failed number_tostring\n");
    assert(0);
}
// ok , trim the quotes of the strings
char* string_tostring(avm_memcell* m){
    char *result ,*p1;
    unsigned len ;
    if(m->data.strVal[0]== '\"' ){
        len = strlen(m->data.strVal)-2;
        result = (char*) malloc(len * sizeof(char));
        p1 =  m->data.strVal ;
        memcpy(result , ++p1,len);
        result[len]='\0';
    }
    else
        result = strdup(m->data.strVal);
    return result;
}
// ok
char* bool_tostring(avm_memcell* m){
    char *result;
    if (m->data.boolVal == 0){
        result = (char*)malloc(sizeof(char)* 6);
        memset(result , 0 ,6 * sizeof(char) );
        strcpy(result,"false");

    }
    else if(m->data.boolVal == 1){
        result = (char*)malloc(sizeof(char)* 5);
        memset(result , 0 ,5 * sizeof(char) );
        strcpy(result,"true");
    }
    else{
        memset(result , 0 ,AVM_STRING_BUFFER * sizeof(char) );
        fprintf(avm_errors, "Unknown bool type: \'%c\'!, at line: %d\n", m->data.boolVal,srcline);
        executionFinished = 1;
    }
    return result;
}
// ok
char* table_tostring(avm_memcell* m){
    unsigned i , more =0;
    unsigned counter = 0;
    char* result_buffer ;
    avm_table_bucket*tmp, **numtable , **strtable, **userfunctable , **libfunctable , **booltable;

    numtable = m->data.tableVal->numIndexed;
    strtable = m->data.tableVal->strIndexed;
    booltable = m->data.tableVal->boolIndexed;
    userfunctable = m->data.tableVal->userfuncIndexed;
    libfunctable = m->data.tableVal->libfuncIndexed;

    ++counter;
    for(i = AVM_TABLE_HASHSIZE-1 ; i > 0 ; --i){
        if((tmp=numtable[i])!=NULL){
            while(tmp != NULL){
                if(more++){
                    ++counter;
                }
                    ++counter;
                    counter += strlen (avm_tostring(&tmp->key));
                    ++counter;
                if(tmp->value.type == string_m){
                        ++counter;
                        counter += strlen (avm_tostring(&tmp->value));
                        ++counter;
                }else{
                    counter += strlen (avm_tostring(&tmp->value));
                }
                ++counter;
                tmp= tmp->next;
            }
        }
    }
    
    for(i = AVM_TABLE_HASHSIZE-1 ; i > 0 ; --i){
        if((tmp=strtable[i])!=NULL){
            while(tmp != NULL){
                if(more++){
                    ++counter;
                }
                ++counter;
                ++counter;
                counter+=strlen(avm_tostring(&tmp->key));
                ++counter;
                ++counter;

                if(tmp->value.type == string_m){
                    ++counter;
                    counter+=strlen(avm_tostring(&tmp->value));
                    ++counter;
                }else{
                    counter+=strlen(avm_tostring(&tmp->value));
                }
                ++counter;
                tmp= tmp->next;
            }
        }
    }
    
    for(i = AVM_TABLE_HASHSIZE-1 ; i > 0 ; --i){
        if((tmp=booltable[i])!=NULL){
            while(tmp != NULL){
                if(more++){
                    ++counter;   
                }
                ++counter;
                counter+= strlen(avm_tostring(&tmp->key));
                ++counter;

                if(tmp->value.type == string_m){
                    ++counter;
                    counter+= strlen(avm_tostring(&tmp->value));
                    ++counter;
                }else{
                    counter+= strlen(avm_tostring(&tmp->value));
                }
                ++counter;
                tmp= tmp->next;
            }
        }
    }
    
    for(i = AVM_TABLE_HASHSIZE-1 ; i > 0 ; --i){
        if((tmp=userfunctable[i])!=NULL){
            while(tmp != NULL){
                if(more++){
                    ++counter;
                }
                ++counter;
                counter+= strlen(avm_tostring(&tmp->key));
                ++counter;
                
                if(tmp->value.type == string_m){
                    ++counter;
                    counter+= strlen(avm_tostring(&tmp->value));
                    ++counter;
                }else{
                    counter+= strlen(avm_tostring(&tmp->value));
                }
                ++counter;
                tmp= tmp->next;
            }
        }
    }
    
    for(i = AVM_TABLE_HASHSIZE-1 ; i > 0 ; --i){
        if((tmp=libfunctable[i])!=NULL){
            while(tmp != NULL){
                if(more++){
                    ++counter;
                }
                ++counter;
                counter+= strlen(avm_tostring(&tmp->key));
                ++counter;

                if(tmp->value.type == string_m){
                    ++counter;
                    counter+= strlen(avm_tostring(&tmp->value));
                    ++counter;
                }else{
                    counter+= strlen(avm_tostring(&tmp->value));

                }
                ++counter;
                tmp= tmp->next;
            }
        }
    }
    ++counter;
    result_buffer = (char*) malloc((counter+1)*sizeof(char));


    strcpy(result_buffer,"[");
    more = 0;
    for(i = AVM_TABLE_HASHSIZE-1 ; i > 0 ; --i){
        if((tmp=numtable[i])!=NULL){
            while(tmp != NULL){
                if(more++){
                    strcat(result_buffer,",");
                }
                strcat(result_buffer,"{");
                strcat(result_buffer,avm_tostring(&tmp->key));
                strcat(result_buffer,":");
                if(tmp->value.type == string_m){
                    strcat(result_buffer,"\"");
                    strcat(result_buffer,avm_tostring(&tmp->value));
                    strcat(result_buffer,"\"");
                }else
                    strcat(result_buffer,avm_tostring(&tmp->value));
                strcat(result_buffer,"}");
                tmp= tmp->next;
            }
        }
    }
    
    for(i = AVM_TABLE_HASHSIZE-1 ; i > 0 ; --i){
        if((tmp=strtable[i])!=NULL){
            while(tmp != NULL){
                if(more++)
                    strcat(result_buffer,",");
                strcat(result_buffer,"{");
                strcat(result_buffer,"\"");
                strcat(result_buffer,avm_tostring(&tmp->key));
                strcat(result_buffer,"\"");
                strcat(result_buffer,":");
                if(tmp->value.type == string_m){
                    strcat(result_buffer,"\"");
                    strcat(result_buffer,avm_tostring(&tmp->value));
                    strcat(result_buffer,"\"");
                }else
                    strcat(result_buffer,avm_tostring(&tmp->value));
                strcat(result_buffer,"}");
                tmp= tmp->next;
            }
        }
    }
    
    for(i = AVM_TABLE_HASHSIZE-1 ; i > 0 ; --i){
        if((tmp=booltable[i])!=NULL){
            while(tmp != NULL){
                if(more++)
                    strcat(result_buffer,",");
                strcat(result_buffer,"{");
                strcat(result_buffer,avm_tostring(&tmp->key));
                strcat(result_buffer,":");
                if(tmp->value.type == string_m){
                    strcat(result_buffer,"\"");
                    strcat(result_buffer,avm_tostring(&tmp->value));
                    strcat(result_buffer,"\"");
                }else
                    strcat(result_buffer,avm_tostring(&tmp->value));
                strcat(result_buffer,"}");
                tmp= tmp->next;
            }
        }
    }
    
    for(i = AVM_TABLE_HASHSIZE-1 ; i > 0 ; --i){
        if((tmp=userfunctable[i])!=NULL){
            while(tmp != NULL){
                if(more++)
                    strcat(result_buffer,",");
                strcat(result_buffer,"{");
                strcat(result_buffer,avm_tostring(&tmp->key));
                strcat(result_buffer,":");
                if(tmp->value.type == string_m){
                    strcat(result_buffer,"\"");
                    strcat(result_buffer,avm_tostring(&tmp->value));
                    strcat(result_buffer,"\"");
                }else
                    strcat(result_buffer,avm_tostring(&tmp->value));
                strcat(result_buffer,"}");
                tmp= tmp->next;
            }
        }
    }
    
    for(i = AVM_TABLE_HASHSIZE-1 ; i > 0 ; --i){
        if((tmp=libfunctable[i])!=NULL){
            while(tmp != NULL){
                if(more++)
                    strcat(result_buffer,",");
                strcat(result_buffer,"{");
                strcat(result_buffer,avm_tostring(&tmp->key));
                strcat(result_buffer,":");
                if(tmp->value.type == string_m){
                    strcat(result_buffer,"\"");
                    strcat(result_buffer,avm_tostring(&tmp->value));
                    strcat(result_buffer,"\"");
                }else
                    strcat(result_buffer,avm_tostring(&tmp->value));
                strcat(result_buffer,"}");
                tmp= tmp->next;
            }
        }
    }
    
    strcat(result_buffer,"]");
    return result_buffer;
}
// ok
char* userfunc_tostring(avm_memcell* m){
    char *result;
    if(result = (char *)malloc(AVM_STRING_BUFFER * sizeof(char))){
        memset(result , 0 ,AVM_STRING_BUFFER * sizeof(char) );
        sprintf(result, "User function: %d", m->data.funcVal);
        return result;
    }
    fprintf(stderr,"malloc failed libfunc_tostring\n");
    assert(0);
}
// ok
char* libfunc_tostring(avm_memcell* m){
    char *result;
    if(result = (char *)malloc(AVM_STRING_BUFFER * sizeof(char))){
        memset(result , 0 ,AVM_STRING_BUFFER * sizeof(char) );
        sprintf(result, "Library function: %s", m->data.libfuncVal);
        return result;
    }
    fprintf(stderr,"malloc failed libfunc_tostring\n");
    assert(0);
}
// ok
char* nil_tostring(avm_memcell* m){
    char *result;
    if(result = (char *)malloc(4 * sizeof(char))){
        memset(result , 0 ,4 * sizeof(char) );
        strcpy(result,"nil");
        return result;
    }
    fprintf(stderr,"malloc failed nil_tostring\n");
    assert(0);
}
// ok
char* undef_tostring(avm_memcell* m){
    char *result;
    if( result = (char *)malloc(10 * sizeof(char))){
        memset(result , 0 ,10 * sizeof(char) );
        strcpy(result,"undefined");
        return result;
    }
    fprintf(stderr,"malloc failed undef_tostring\n");
    assert(0);
}


