#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../headerfiles/avm_datatypes.h"
#include "../headerfiles/avm_functions.h"


// ok
void avm_tableincrefcounter(avm_table* t){
    ++t->refCounter ;
}
// ok
void avm_tabledecrefcounter ( avm_table* t){
    assert(t->refCounter > 0);
    if(--t->refCounter == 0)
        avm_tabledestroy(t);
}
// ok
void avm_tablebucketsinit(avm_table_bucket ** p){
    unsigned i;
    for(i = 0 ; i < AVM_TABLE_HASHSIZE ;++i){
        p[i] = (avm_table_bucket*) 0;
    }
}
// ok
avm_table* avm_tablenew(void){
    avm_table* t = (avm_table*) malloc(sizeof(avm_table));

    AVM_WIPEOUT(*t);
    t->refCounter = 0;
    t->total = 0;
    avm_tablebucketsinit(t->numIndexed);
    avm_tablebucketsinit(t->strIndexed);
    avm_tablebucketsinit(t->userfuncIndexed);
    avm_tablebucketsinit(t->libfuncIndexed);
    avm_tablebucketsinit(t->boolIndexed);
    return t ;
}
// ok
void avm_tablebucketsdestroy(avm_table_bucket** p){
    unsigned i;
    avm_table_bucket* b , *del ;
    for(i = 0 ; i < AVM_TABLE_HASHSIZE ; ++i , ++p){
        for(b = *p ; b;){
            del = b ;
            b = b->next;
            avm_memcellclear(&del->key);
            avm_memcellclear(&del->value);
            free(del);
        }
        p[i] = (avm_table_bucket*) 0;
    }
}
// ok
void avm_tabledestroy(avm_table* t){
    avm_tablebucketsdestroy(t->strIndexed);
    avm_tablebucketsdestroy(t->numIndexed);
    avm_tablebucketsdestroy(t->userfuncIndexed);
    avm_tablebucketsdestroy(t->libfuncIndexed);
    avm_tablebucketsdestroy(t->boolIndexed);
    free(t);
}

static unsigned hash_func(char* s){
    unsigned i , hashnumber = 77;
    for(i = 0 ; i < strlen(s) ; ++i){
        hashnumber *= s[i] ;
    }
    return hashnumber % AVM_TABLE_HASHSIZE;
}
//  ok
avm_memcell* avm_tablegetelem(avm_table* table , avm_memcell* index){
    unsigned pos;
    avm_table_bucket* bucket;
    assert(index);
    assert(table);
    pos = hash_func(avm_tostring(index));
    assert(pos < AVM_TABLE_HASHSIZE);
    if(index->type == number_m){
        bucket = table->numIndexed[pos];
        if(bucket && bucket->key.data.numVal == index->data.numVal)
            return &bucket->value;
    }
    else if(index->type == string_m){
        bucket = table->strIndexed[pos];
        if(bucket && strcmp(bucket->key.data.strVal, index->data.strVal) == 0)
            return &bucket->value;
    }
    else if(index->type == bool_m){
        bucket = table->boolIndexed[pos];
        if(bucket && bucket->key.data.boolVal == index->data.boolVal)
            return &bucket->value;
    }
    else if(index->type == userfunc_m){
        bucket = table->userfuncIndexed[pos];
        if(bucket && bucket->key.data.funcVal == index->data.funcVal)
            return &bucket->value;
    }
    else if(index->type == libfunc_m){
        bucket = table->libfuncIndexed[pos];
        if(bucket && strcmp(bucket->key.data.libfuncVal, index->data.libfuncVal) == 0)
            return &bucket->value;
    }
    assert(0);
}

static avm_table_bucket* newbucket(avm_memcell* index , avm_memcell* content){
    avm_table_bucket* bucket;

    bucket =(avm_table_bucket*) malloc(sizeof(avm_table_bucket));
    avm_assign(&bucket->key,index);
    avm_assign(&bucket->value , content);
    bucket->next = (avm_table_bucket*)0;
    return bucket;
}
//  ok
void avm_tablesetelem(avm_table* table , avm_memcell* index , avm_memcell* content){
    unsigned pos;
    avm_table_bucket* bucket , *temp;
    assert(index);
    assert(table);
    assert(content);
    pos = hash_func(avm_tostring(index));

    assert(pos < AVM_TABLE_HASHSIZE);

    temp = newbucket(index , content);

    if(index->type == number_m){
        bucket = table->numIndexed[pos];         
        while(bucket != NULL){
            if(bucket && bucket->key.data.numVal == index->data.numVal){
                avm_assign(&bucket->value ,content);
                return;
            }
            bucket = bucket->next;
        }
        if(bucket == NULL){
            table->numIndexed[pos] = temp ;
            ++table->total;
        }
        return ;
    }
    else if(index->type == string_m){
        bucket = table->strIndexed[pos]; 
        while(bucket != NULL){
            if(bucket && strcmp(bucket->key.data.strVal , index->data.strVal) == 0){
                avm_assign(&bucket->value ,content);
                return;
            }
            bucket = bucket->next;
        }
        if(bucket == NULL){
            ++table->total;
            table->strIndexed[pos] = temp ;
        }
        return ;
    }
    else if(index->type == bool_m){
        bucket = table->boolIndexed[pos];         
        while(bucket != NULL){
            if(bucket && bucket->key.data.boolVal == index->data.boolVal){
                avm_assign(&bucket->value ,content);
                return;
            }
            bucket = bucket->next;
        }
        if(bucket == NULL){
            table->boolIndexed[pos] = temp ;
            ++table->total;
        }
        return ;
    }
    else if(index->type == userfunc_m){
        bucket = table->userfuncIndexed[pos];         
        while(bucket != NULL){
            if(bucket && bucket->key.data.funcVal == index->data.funcVal){
                avm_assign(&bucket->value ,content);
                return;
            }
            bucket = bucket->next;
        }
        if(bucket == NULL){
            table->userfuncIndexed[pos] = temp ;
            ++table->total;
        }
        return ;
    }
    else if(index->type == libfunc_m){
        bucket = table->libfuncIndexed[pos]; 
        while(bucket != NULL){
            if(bucket && strcmp(bucket->key.data.libfuncVal , index->data.strVal) == 0){
                avm_assign(&bucket->value ,content);
                return;
            }
            bucket = bucket->next;
        }
        if(bucket == NULL){
            table->libfuncIndexed[pos] = temp ;
            ++table->total;
        }
        return ;
    }
    assert(0);
}