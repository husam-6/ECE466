#ifndef SYM_H
#define SYM_H

extern enum storage_class tmp_s_class;

enum scope_type{
    GLOBAL_SCOPE,
    FUNC_SCOPE,
    BLOCK_SCOPE,
    PROTOTYPE_SCOPE,
    MEMBER_SCOPE
};

// For declarations...
int check_for_symbol();
int search_all_tabs();
void add_symbol_entry();
void create_scope();
void print_symbol_table();
void print_symbol();
void print_scope_symbols();
char * print_union_struct();
char * print_scope();
char * print_namespace();

// For scopes
struct scope * make_new_scope(enum scope_type s_type);
void create_new_scope();
void close_outer_scope();
struct astnode_symbol * reverse(struct astnode_symbol * head);

// Enums for symbol structure
enum symbol_kind{
    DECL,
    DEF
};

enum namespace {
    VAR_S,
    FUNC_S,             // temp namespace...
    TAG_S,
    LABEL_S,
    MEMBER_S
};

enum storage_class{
    EXTERN_S,
    AUTO_S,
    REGISTER_S, 
    STATIC_S,
    NA
};


// Symbol structure for symbol table
struct astnode_symbol {
    struct type_node * type;
    char * name;
    enum namespace n_space;
    enum symbol_kind symbol_k;  
    enum storage_class s_class;     //Only for variable declarations (-1 if not)
    int line_num;
    char * file_name;
    enum scope_type scope; 

    // Next item in symbol table
    struct astnode_symbol * next;
};


// Global scope
extern struct scope * curr_scope; 


// Scope struct 
struct scope {
    // Scope type and symbol table head for given scope
    enum scope_type s_type; 
    struct astnode_symbol * head;

    // Linked list of any scope seen inside of this scope (to save after popping from stack)
    struct scope * next_child;

    // Pointer to parent / outer scope
    struct scope * outer;
};


void print_params(struct astnode_symbol * head, int depth);
char * print_s_class(enum storage_class s_class);

#endif /* SYM_H */