//csd4393,csd4225,csd4226

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

extern double*      numConsts;
extern char**       stringConsts;
extern char**       nameLibfuncs;
extern userfunc*    userFuncs;
extern instruction* instructions;


extern unsigned currStr , currNum , currInstr , currLibFuncs , currUserFuncs , program_var_offset;

void create_abc_file(void){
    FILE*        binaryfile;
    unsigned i , len;
    unsigned magicnum = 11682356;
    unsigned hacknum = 0;

    if( (binaryfile = fopen("./binary/binary.abc","wb") ) == NULL){
        fprintf(stderr,"Error opening binary.abc\n");
        exit(EXIT_FAILURE);
    }

    // magic number
    fwrite(&magicnum , 1 , sizeof(unsigned) ,binaryfile);


    // strings array
    fwrite(&currStr , 1 , sizeof(unsigned) , binaryfile);
    for(i = 0 ; i < currStr ; ++i){
        //len,name
        len = strlen(stringConsts[i]) + 1 ;
        fwrite(&len , 1 , sizeof(unsigned) , binaryfile);
        fwrite(stringConsts[i] , len , sizeof(char) , binaryfile);

    }

    // numbers array
    fwrite(&currNum , 1 , sizeof(unsigned) , binaryfile);
    for(i = 0 ; i < currNum ; ++i){
        fwrite(&numConsts[i], 1 , sizeof(double) , binaryfile);
    }

    // userfuncs array
    fwrite(&currUserFuncs , 1 , sizeof(unsigned) , binaryfile);
    for(i = 0 ; i < currUserFuncs ; ++i){
        //addr
        fwrite(&(userFuncs[i].address) , 1 , sizeof(unsigned) , binaryfile); 
        //loc size
        fwrite(&(userFuncs[i].localSize) , 1 , sizeof(unsigned) , binaryfile); 
        // formal size
        fwrite(&(userFuncs[i].totalFormals) , 1 , sizeof(unsigned) , binaryfile);
        //len,name
        len = strlen(userFuncs[i].id) + 1 ;
        fwrite(&len , 1 , sizeof(unsigned), binaryfile);
        fwrite(userFuncs[i].id , len , sizeof(char) , binaryfile);
    }

    // libfuncs array
    fwrite(&currLibFuncs , 1 , sizeof(unsigned) , binaryfile);
    for(i = 0 ; i < currLibFuncs ; ++i){
        //len,name
        len = strlen(nameLibfuncs[i]) + 1 ;
        fwrite(&len , 1 , sizeof(unsigned), binaryfile);
        fwrite(nameLibfuncs[i] , len , sizeof(char) , binaryfile);
    }

    // instructions array
    fwrite(&currInstr ,1 , sizeof(unsigned) , binaryfile);
    for(i = 0 ; i <currInstr ; ++i){
        fwrite(&(instructions[i].opcode) , 1 , sizeof(vmopcode) , binaryfile);
        hacknum = 0;
        if(instructions[i].result.type != ignore_a){
            hacknum = 100 ;
        }
        if(instructions[i].arg1.type != ignore_a){
            hacknum = hacknum + 20 ;
        }
        if(instructions[i].arg2.type != ignore_a){
            hacknum = hacknum + 3 ;
        }
        fwrite(&hacknum , 1 , sizeof(unsigned) , binaryfile);
        if(instructions[i].result.type != ignore_a){
            fwrite(&instructions[i].result.type , 1 , sizeof(vmarg_t) , binaryfile);
            fwrite(&instructions[i].result.val , 1 , sizeof(unsigned) , binaryfile);   
        }
        if(instructions[i].arg1.type != ignore_a){
            fwrite(&instructions[i].arg1.type , 1 , sizeof(vmarg_t) , binaryfile);
            fwrite(&instructions[i].arg1.val , 1 , sizeof(unsigned) , binaryfile);   
        }
        if(instructions[i].arg2.type != ignore_a){
            fwrite(&instructions[i].arg2.type , 1 , sizeof(vmarg_t) , binaryfile);
            fwrite(&instructions[i].arg2.val , 1 , sizeof(unsigned) , binaryfile);   
        }
        fwrite(&instructions[i].srcLine ,1 , sizeof(unsigned) , binaryfile);

    }
    //put the number of global vars
    fwrite(&program_var_offset , 1 , sizeof(unsigned) , binaryfile);

    fclose(binaryfile);
}