
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

typedef struct userfunc{
    unsigned    address;
    unsigned    localSize;
    unsigned    totalFormals;
    char*       id;
    returnList* retlist;
}userfunc;

typedef struct instruction {
    vmopcode opcode;
    vmarg result;
    vmarg arg1;
    vmarg arg2;
    unsigned srcLine;
}instruction;


typedef struct incomplete_jump{
    unsigned    instrNo;
    unsigned    iaddress; 
    struct incomplete_jump* next;
}incomplete_jump;


void create_tcode();
void add_incomplete_jum(unsigned instrNo , unsigned iaddress);
void emit_instr(instruction i );
void expand_instr(void);

unsigned consts_newstring(char*s);

unsigned consts_newnumber(double n);

unsigned libfuncs_newused(char* s);

void make_numberoperand (vmarg* arg , double val);

void make_booloperand (vmarg* arg , unsigned val);

void make_retvaloperand (vmarg* arg);

void make_operand(expr* e , vmarg* arg);

unsigned nextinstructionlabel();

void generate(vmopcode op,quad* q);
void generate_relational(vmopcode op,quad* q);

void generate_ADD(quad* q);                     
void generate_SUB(quad* q);                       
void generate_MUL(quad* q);                       
void generate_DIV(quad* q);       
void generate_MOD(quad* q);      

unsigned curr_process_quad();

void reset_return_list(returnList *l );
returnList* add_return_list(returnList *l ,unsigned instrNo);


