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


