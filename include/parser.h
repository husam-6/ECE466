#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// Function prototypes (helper functions for major types of operations)
struct astnode * make_ast_node(int type);
struct astnode * create_unary(int op_type, int op, struct astnode *expr);
struct astnode * create_binary(int op_type, int op, struct astnode *left, struct astnode *right);
struct astnode * create_ternary(int op_type, struct astnode *left, struct astnode *middle, struct astnode *right);

// For declarations...
int check_for_symbol();
void add_symbol_entry();
void create_scope();
void print_symbol_table();
struct astnode * create_array_node(int size);

// For debugging, print the ast symbolically 
void print_ast(struct astnode * node, int depth);



// #include "../lexer/lexer.h"
enum op_type{
    BINOP = 0,
    ASSIGNMENT,
    ASSIGNMENT_COMPOUND,
    DEREF,
    ADDR_OF, 
    UNARY_OP, 
    SIZEOF_OP, 
    COMP_OP, 
    SELECT,
    LOGICAL_OP,
    TERNARY_OP,
    FUNC
};

enum node_type{
    FN_CALL = 0,
    UNARY_NODE,
    BINARY_NODE,
    TERNARY_NODE,
    NUM,
    IDENT_NODE,
    CHAR_LIT,
    STR_LIT,
    ARRAY_TYPE, 
    POINTER_TYPE, 
    FUNCTION_TYPE, 
    SCALAR_TYPE, 
};


// AST Node structs
// Unary operators
struct astnode_unary{
    int operator;
    enum op_type operator_type;
    struct astnode *expr; 
};

// Binary operators
struct astnode_binary {
    int operator;
    enum op_type operator_type;
    struct astnode *left;
    struct astnode *right;
};

// Ternary operators
struct astnode_ternary {
    enum op_type operator_type;
    struct astnode *left;
    struct astnode *middle;
    struct astnode *right;
};

// Function call node
struct astnode_fncall {
    struct astnode *postfix;
    struct linked_list *head;
};

// Function call linked list for arguments
struct linked_list {
    struct astnode *expr; 
    struct linked_list *next;
    int num_args; 
};

// Function call helper functions
struct astnode * create_fn_node(struct astnode *postfix, struct linked_list *head);
struct linked_list * create_ll_node(struct astnode *expr);
void push_ll(struct linked_list *head, struct astnode *expr);

// Enums for symbol structure
enum symbol_kind{
    decl,
    def
};

enum namespace {
    VAR_S,
    FUNC_S, 
    TAG_S,
    LABEL_S,
    MEMBER_S
};


// Symbol structure for symbol table
struct astnode_symbol {
    struct astnode * type;
    char * name;
    enum namespace n_space;
    enum symbol_kind symbol_k;  
    int storage_class;     //Only for variable declarations (-1 if not)
    
    // Next item in symbol table
    struct astnode_symbol * next;
};


// Global scope
extern struct scope curr_scope; 

// Scalar type struct
struct scalar_type{
    enum num_type arith_type;
};


// Array type nodeos
struct arr_type{
    int size;
};


struct type_node{
    struct astnode * next_type; 
    union{
        struct scalar_type scalar;
        struct arr_type array_node;
        // struct function_type func_node;
        // struct struct_union_type struct_union_node;
    };
};


// Node struct
struct astnode {
    enum node_type type;
    union {
            struct astnode_unary unary;
            struct astnode_binary binary;
            struct astnode_ternary ternary;
            struct number num;
            char *ident;
            char char_lit;
            struct string_literal str_lit;
            struct astnode_fncall fncall;
            struct astnode_symbol symbol_table_entry;
            struct type_node t_node; 

    };
};

// Scope struct 
struct scope {
    struct astnode_symbol * head;
    struct scope * outer;
};


#endif /* PARSER_H */