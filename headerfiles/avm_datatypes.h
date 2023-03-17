#ifndef __AVM_DATATYPES__
#define __AVM_DATATYPES__

#define AVM_TABLE_HASHSIZE 211
 
#define AVM_NUMACTUALS_OFFSET 4
#define AVM_SAVEDPC_OFFSET 3
#define AVM_SAVEDTOP_OFFSET 2
#define AVM_SAVEDTOPSP_OFFSET 1

#define AVM_ENDING_PC t_code_size
#define AVM_STACK_SIZE      4096
#define AVM_STACKENV_SIZE 4
#define AVM_STRING_BUFFER 64
#define AVM_MAX_INSTRUCTIONS (unsigned) nop_v

#define AVM_WIPEOUT(m)      memset(&(m) ,0 , sizeof(m))

//types 

typedef enum vmopcode {
    assign_v,           add_v,              sub_v,
    mul_v,              div_v,              mod_v,
    uminus_v,
    jump_v,             jeq_v,              jne_v,
    jle_v,              jge_v,              jlt_v,
    jgt_v,              call_v,             pusharg_v,
    funcenter_v,        funcexit_v,         newtable_v,
    tablegetelem_v,     tablesetelem_v,     nop_v
}vmopcode;

typedef enum avm_memcell_t{
    number_m      =0,
    string_m,
    bool_m,
    table_m,
    userfunc_m,
    libfunc_m,
    nil_m,
    undef_m
}avm_memcell_t;

typedef enum vmarg_t{
    label_a                 =0,
    global_a                ,
    formal_a                ,
    local_a                 ,
    number_a                ,
    string_a                ,
    bool_a                  ,
    nil_a                   ,
    userfunc_a              ,
    libfunc_a               ,
    retval_a                ,
    ignore_a
}vmarg_t ;

typedef struct vmarg{
    vmarg_t type;
    unsigned    val;
}vmarg;

typedef struct returnList{
    unsigned    instrNo;
    struct returnList* next;
}returnList;

typedef struct instruction {
    vmopcode opcode;
    vmarg result;
    vmarg arg1;
    vmarg arg2;
    unsigned srcLine;
}instruction;

typedef struct userfunc{
    unsigned    address;
    unsigned    localSize;
    unsigned    totalFormals;
    char*       id;
    returnList* retlist;
}userfunc;

typedef struct avm_table avm_table ;

typedef struct avm_memcell{
    avm_memcell_t   type;
    union{
        double          numVal;
        char*           strVal;
        unsigned char   boolVal;
        avm_table*      tableVal;
        unsigned        funcVal;
        char*           libfuncVal;
    } data;
}avm_memcell ;

typedef struct avm_table_bucket {
    avm_memcell         key;
    avm_memcell         value;
    struct avm_table_bucket*   next;
}avm_table_bucket;

typedef struct avm_table{
    unsigned                refCounter;
    avm_table_bucket*       strIndexed[AVM_TABLE_HASHSIZE];
    avm_table_bucket*       numIndexed[AVM_TABLE_HASHSIZE];
    avm_table_bucket*       userfuncIndexed[AVM_TABLE_HASHSIZE];
    avm_table_bucket*       libfuncIndexed[AVM_TABLE_HASHSIZE];
    avm_table_bucket*       boolIndexed[AVM_TABLE_HASHSIZE];
    unsigned                total;
}avm_table;

// dispatchers
typedef void (*execute_func_t) (instruction*);
typedef void (*library_func_t) (void);

//variables
extern FILE* avm_warnings , *avm_errors ,*outfile;

extern unsigned char executionFinished;

extern double*      numConsts_array;
extern char**       stringConsts_array;
extern char**       libFuncs_array;
extern userfunc*    userFuncs_array;
extern instruction* t_code;
extern unsigned total_globals ;
extern unsigned stringConsts_array_size , numConsts_array_size , t_code_size , libFuncs_array_size , userFuncs_array_size , total_globals;

extern unsigned top , topsp ;
extern unsigned pc;
extern unsigned GLOBAL_TEMP;
extern unsigned totalActuals , srcline;
extern unsigned char executionFinished;
extern avm_memcell ax , bx , cx , retval ;
extern avm_memcell STACK[];
extern char* typeString[];

#endif