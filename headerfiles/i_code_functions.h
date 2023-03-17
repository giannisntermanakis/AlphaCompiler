void print_details(expr* arg1 , expr* arg2 , expr* result , iopcode op  , unsigned label,int line);

void print_assign(expr* source , expr* result,int line);
void print_add(expr*arg1 , expr* arg2 , expr* result,int line);
void print_sub(expr*arg1 , expr* arg2 , expr* result,int line);
void print_div(expr*arg1 , expr* arg2 , expr* result,int line);
void print_mod(expr*arg1 , expr* arg2 , expr* result,int line);
void print_mul(expr*arg1 , expr* arg2 , expr* result,int line);
void print_uminus(expr* arg1 ,expr* result,int line);

void print_not(expr* arg1 , expr* result,int line);
void print_less(expr* arg1, expr* arg2, unsigned label,int line);
void print_lesseq(expr* arg1, expr* arg2, unsigned label,int line);
void print_greater(expr* arg1, expr* arg2, unsigned label,int line);
void print_greatereq(expr* arg1, expr* arg2, unsigned label,int line);
void print_eq(expr* arg1, expr* arg2, unsigned label,int line);
void print_and(expr*arg1 , expr* arg2 , expr* result,int line);
void print_or(expr*arg1 , expr* arg2 , expr* result,int line);
void print_noteq(expr* arg1, expr* arg2, unsigned label,int line);

void print_param(expr* arg1,int line);
void print_getelem(expr* arg1 , expr* arg2 , expr* result,int line);
void print_setelem(expr* arg1 , expr* arg2 , expr* result,int line);
void print_return(expr* arg1,int line);


void print_quads();


void resettemp();
void expand(void);
void emit(
        iopcode     op,
        expr*       result,
        expr*       arg1,
        expr*       arg2,
        unsigned    label,
        unsigned    line
        );

       
void reset_formalarg_offset();
void reset_functionlocal_offset();
void restore_currscope_offset(unsigned n);
void patchlabel(unsigned quadNo, unsigned label);
void patchlist(int list , int label);
void patch_label(unsigned quadNo , unsigned label);
void make_stmt(stmt_t* s);
void load_prev_flo();
void enter_scope_space();
void exits_scope_space();
void save_prev_flo();

void backpatch(list_t* l1 , unsigned label);

unsigned char check_bool(expr* e);
unsigned char check_arithm(expr* e);
unsigned char convert_to_boolean(expr* e);
unsigned char typecheck(expr* e1 , expr* e2 , char* op);
unsigned int is_tempname(char* s);
unsigned int is_tempexpr(expr* e);
unsigned curr_scope_offset();
unsigned in_curr_scope_offset();
unsigned next_quad_label();

char* newtempname();

int currscope();
int newlist(int i);
int mergelist(int l1 , int l2);
int check_defined(expr* e1 ); 

list_t* merge_list(list_t*l1 , list_t* l2);
list_t* newnode();

scopespace_t curr_scope_space();


symbol* newtemp();
symbol* lookup_tmp(char* name , int scope);
symbol* insert_tmp(char* name );
symbol* id_for_assign(symbol* data , int member_item);
symbol_t match_type1_to_type2(SymType type);

expr* complete(expr* e1 );
expr* newexpr(expr_t t);
expr* newexpr_conststring(char* s);
expr* newexpr_constbool(int i);
expr* newexpr_constnum(int i);
expr* emit_iftableitem(expr* e);
expr* lvalue_expr(symbol* sym);
expr* expr_arithm_op_expr(expr* lv , expr* rv , char* op);
expr* expr_rel_op_expr(expr* lv , expr* rv , char* op);
expr* expr_bool_op_expr(expr* lv , expr* rv , char* op , unsigned label);
expr* assign_expr(expr* lv , expr* rv);
expr* find_defined_expr(symbol* sym);
expr* find_defined_to_assign(symbol* sym);
expr* copy_expr(expr* e1);
expr* uminus_expr(expr* e);
expr* member_item_dot(expr* lv , char* name);
expr* member_item_brack(expr* lv , expr* el);
expr* primary_lval_is_member(expr* lv);
expr* term_is_member(expr* lv);
expr* call_first(expr* cal , expr* el);
expr* call_second(expr* lv , call_s* cs);
expr* call_third(symbol* func , expr* el);
expr* newexpr_constnum(int i);
expr* objectdef_elist(expr* el);
expr* objectdef_indexed(index_set* in);
expr* call_expr(expr* lv , expr* el);
expr* reverse_expr(expr* og);



expr* symtype_to_expr_type(expr* e);