typedef struct SymTable *SymTable_T;


SymTable_T SymTable_new(void);
void SymTable_free(SymTable_T oSymTable);
symbol* SymTable_insert(SymTable_T oSymTable , const char *name, SymType type ,unsigned int scope ,unsigned int numline );
symbol* SymTable_lookup_scope(SymTable_T oSymTable , const char *name ,unsigned int scope);
symbol* SymTable_lookup_general(SymTable_T oSymTable , const char *name , int scope);

int SymTable_hide(SymTable_T oSymTable , unsigned int scope);
void SymTable_print(SymTable_T oSymTable);

int check_library(const char* name);
int search_for_function(SymTable_T sym , int curr_sc , int old_sc);
