
struct alpha_token_t{
    unsigned int numline ;
    unsigned int numToken ;
    char* content ;
    char* type ;
    struct alpha_token_t* alpha_yylex ;
};
struct comment_data{
    int start ;
    int end   ;
};
char* allocate_space(int size) ;
void push_list(int line , int tok_num , char* cont , int len , char* type);
void print_array(struct alpha_token_t* head) ;
void find_string(void) ;
void find_comment1(void) ;
void find_comment2(void) ;
char* make_string_for_comment_info(int start , int end );
void reverse(char s[]);
void itoa(int n, char s[]);
int find_colomn(void) ;
int find_dot(void) ;
int find_assign(void) ;
int find_plus(void) ;
int find_minus(void) ;
int find_more(void) ;
int find_less(void);
void free_all() ;
