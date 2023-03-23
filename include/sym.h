#ifndef SYM_H
#define SYM_H

extern enum storage_class tmp_s_class;

// For declarations...
int check_for_symbol();
void add_symbol_entry();
void create_scope();
void print_symbol_table();
void print_declaration();

// Enums for symbol structure
enum symbol_kind{
    DECL,
    DEF
};

enum namespace {
    VAR_S,
    FUNC_S, 
    TAG_S,
    LABEL_S,
    MEMBER_S
};

enum storage_class{
    AUTO_S,
    REGISTER_S, 
    EXTERN_S,
    STATIC_S,
    NON_VAR
};


// Symbol structure for symbol table
struct astnode_symbol {
    struct type_node * type;
    char * name;
    enum namespace n_space;
    enum symbol_kind symbol_k;  
    enum storage_class s_class;     //Only for variable declarations (-1 if not)
    int line_num; 
    
    // Next item in symbol table
    struct astnode_symbol * next;
};


// Global scope
extern struct scope curr_scope; 


enum scope_type{
    S_GLOBAL,
    S_FUNC,
    S_BLOCK,
    S_PROTOTYPE,
    S_MEMBER
};

// Scope struct 
struct scope {
    enum scope_type s_type; 
    struct astnode_symbol * head;
    struct scope * outer;
};


#endif /* SYM_H */