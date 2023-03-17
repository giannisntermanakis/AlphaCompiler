#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../headerfiles/avm_datatypes.h"

double*      numConsts_array;
char**       stringConsts_array;
char**       libFuncs_array;
userfunc*    userFuncs_array;
instruction* t_code;


unsigned stringConsts_array_size , numConsts_array_size,
        t_code_size , libFuncs_array_size , userFuncs_array_size,
        total_globals;


void read_abc_file(void){
    FILE*        binaryfile;
    unsigned i , len;
    unsigned magicnum ;
    unsigned hacknum ;

    if( (binaryfile = fopen("./binary/binary.abc","r") ) == NULL){
        fprintf(stderr,"Error opening binary.abc\n");
        exit(EXIT_FAILURE);
    }

    // magic number
    fread(&magicnum , sizeof(unsigned) ,1 , binaryfile);
    if(magicnum != 11682356){
        fprintf(stderr,"invalid magic number!\n");
        exit(EXIT_FAILURE);
    }

    // strings array
    fread(&stringConsts_array_size , sizeof(unsigned) , 1 , binaryfile);

    stringConsts_array = malloc(stringConsts_array_size* sizeof(char*));

    for(i = 0 ; i < stringConsts_array_size ; ++i){
        //len,name
        fread(&len  , sizeof(unsigned), 1 , binaryfile);
        stringConsts_array[i] = malloc(len* sizeof(char));
        fread(stringConsts_array[i] , len , sizeof(char) , binaryfile);

    }

    // numbers array
    fread(&numConsts_array_size , sizeof(unsigned) ,  1 ,binaryfile);
    numConsts_array = malloc(numConsts_array_size* sizeof(double));

    for(i = 0 ; i < numConsts_array_size ; ++i){
        fread(&numConsts_array[i],  sizeof(double) ,1 , binaryfile);
    }

    // userFuncs_array array
    fread(&userFuncs_array_size  , sizeof(unsigned) , 1, binaryfile);
    userFuncs_array = malloc(userFuncs_array_size * sizeof(userfunc));
    for(i = 0 ; i < userFuncs_array_size ; ++i){
        //addr
        fread(&(userFuncs_array[i].address) , sizeof(unsigned), 1  , binaryfile); 
        //loc size
        fread(&(userFuncs_array[i].localSize) , sizeof(unsigned), 1  , binaryfile); 
        fread(&(userFuncs_array[i].totalFormals) , sizeof(unsigned), 1  , binaryfile); 
        //len,name
        fread(&len , sizeof(unsigned), 1 , binaryfile);
        userFuncs_array[i].id = malloc( len * sizeof(char));
        fread(userFuncs_array[i].id  , sizeof(char), len , binaryfile);
    }

    // libfuncs array
    fread(&libFuncs_array_size , sizeof(unsigned) , 1 , binaryfile);
    libFuncs_array = malloc(libFuncs_array_size * sizeof(char*));
    for(i = 0 ; i < libFuncs_array_size ; ++i){
        //len,name
        fread(&len , sizeof(unsigned), 1 , binaryfile);
        libFuncs_array[i] = malloc(len * sizeof(char));
        fread(libFuncs_array[i] , sizeof(char) , len , binaryfile);
    }

    // t_code array
    fread(&t_code_size , sizeof(unsigned),1  , binaryfile);
    t_code = malloc(t_code_size * sizeof(instruction));
    for(i = 0 ; i <t_code_size ; ++i){
        fread(&(t_code[i].opcode)  , sizeof(vmopcode), 1 , binaryfile);
        
        fread(&hacknum  , sizeof(unsigned), 1 , binaryfile);
        
        
        if(hacknum  >= 100){
            hacknum = hacknum - 100;
            fread(&t_code[i].result.type , sizeof(vmarg_t), 1  , binaryfile);
            fread(&t_code[i].result.val , sizeof(unsigned), 1  , binaryfile);   
        }
        else{
            t_code[i].result.type = ignore_a;
        }
        if(hacknum  >= 20 ){
            hacknum = hacknum - 20;
            fread(&t_code[i].arg1.type , sizeof(vmarg_t) , 1 , binaryfile);
            fread(&t_code[i].arg1.val , sizeof(unsigned) , 1 , binaryfile);   
        }
        else{
            t_code[i].arg1.type = ignore_a;
        }
        if(hacknum == 3){
            hacknum = 0;
            fread(&t_code[i].arg2.type , sizeof(vmarg_t) , 1 , binaryfile);
            fread(&t_code[i].arg2.val , sizeof(unsigned) , 1 , binaryfile);   
        }
        else{
            t_code[i].arg2.type = ignore_a;
        }
        // line
        fread(&t_code[i].srcLine , sizeof(unsigned) , 1 , binaryfile);
    }

    // total global vars
    fread(&total_globals , sizeof(unsigned) , 1 , binaryfile);

    fclose(binaryfile);
}