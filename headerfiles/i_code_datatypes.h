typedef enum SymType {
	GLOBAL,
	LOCAL,
	FORMAL,
	USERFUNC,
	LIBFUNC,
	ERROR
}SymType;

typedef enum scopespace_t {
    program_var,
    function_local,
    formal_arg
}scopespace_t ;

typedef enum symbol_t{
    var_s,
    programfunc_s,
    libraryfunc_s,
    error_s
}symbol_t;

//iopcode
typedef enum iopcode{
    assign = 0,     add,        sub,
    mul,        dev,        mod,
    uminus,     and,        or,
    not,        if_eq,      if_noteq,
    if_lesseq,  if_greatereq,   if_less,
    if_greater, jump,    call,
    param,      ret,        getretval,
    funcstart,  funcend,    tablecreate,    
    tablegetelem,   tablesetelem
}iopcode;
//expr_t
typedef enum expr_t{
    error_e,
    retval_e,
    
    var_e,
    tableitem_e,

    libraryfunc_e,
    programfunc_e,

    arithexpr_e,
    boolexpr_e,
    assignexpr_e,
    newtable_e,

    constnum_e,
    constbool_e,
    conststring_e,

    nil_e
}expr_t;

typedef struct DATA{
	char*        name;
	unsigned int scope ;
	unsigned int line ;
}DATA ;

typedef struct symbol{
	int 			hide ;
	SymType 		type1;
	symbol_t 		type2;
	DATA* 			value;
	struct symbol* 	next ;
    scopespace_t    space;
    unsigned        offset;
    int             openfuncs;
    unsigned        iaddress;
    unsigned        taddress;
    unsigned        totallocals;
    int             funcargs;

}symbol;

struct arguments{
	char* arg_id ;
	struct arguments *next ;
};

struct SymTable{
	symbol **lines ;
};

typedef struct list_t{
    int data;
    struct list_t* next;
}list_t;

//expr
typedef struct expr{
    expr_t          type;
    symbol*         sym;
    struct expr*    index;
    double          numConst;
    char*           strConst;
    unsigned char   boolConst;
    struct expr*    next; 
    unsigned        notexpr;
    list_t*         truelist;
    list_t*         falselist;
}expr;


//quads
typedef struct{
    iopcode     op;
    expr*       result;
    expr*       arg1;
    expr*       arg2;
    unsigned    label;
    unsigned    line;
    unsigned    taddress;
}quad ;

//call type
typedef struct{
    expr*           elist;
    unsigned char   method;
    char*           name;
}call_s;


typedef struct index_set{
    expr* index ;
    expr* value ;
    struct index_set* next ;
}index_set;

typedef struct {
    int breakList ;
    int contList;
}stmt_t;

typedef struct{
    unsigned test ;
    unsigned enter;
}for_t;

