#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "type.h"
#include "sym.h"

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
    FOR_LOOP,
    DECLARATION,
    COMPOUND,
    WHILE_LOOP,
    IF_STMT
};

// Function prototypes (helper functions for major types of operations)
struct astnode * make_ast_node(int type);
struct astnode * create_unary(int op_type, int op, struct astnode *expr);
struct astnode * create_binary(int op_type, int op, struct astnode *left, struct astnode *right);
struct astnode * create_ternary(int op_type, struct astnode *left, struct astnode *middle, struct astnode *right);
struct astnode * create_for_loop(struct astnode * init, struct astnode * cond, struct astnode * body, struct astnode * inc);
struct astnode * create_while_loop(struct astnode * expr, struct astnode * cond, int do_while);
struct astnode * create_if_stmt(struct astnode * stmt, struct astnode * cond, struct astnode * else_stmt);
void resolve_identifier(char * ident, enum namespace n_space, struct astnode * node);
void n_tabs(int n);
char * print_datatype(int type);
void die(const char *msg);


// For debugging, print the ast symbolically 
void print_ast(struct astnode * node, int depth);


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

struct astnode_ident{
    char * name; 
    struct astnode_symbol * sym; 
};

struct astnode_for_loop{
    struct astnode * init; 
    struct astnode * cond;
    struct astnode * body;
    struct astnode * inc; 
};


struct astnode_while_loop{
    int do_while;
    struct astnode * stmt;
    struct astnode * cond;
};

struct astnode_if_stmt{
    struct astnode * cond; 
    struct astnode * stmt;
    struct astnode * else_stmt; 
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

// Print list of ast's 
void dump_ast(struct linked_list *asthead, int tabs);

// Node struct
struct astnode {
    enum node_type type;
    union {
            struct astnode_unary unary;
            struct astnode_binary binary;
            struct astnode_ternary ternary;
            struct number num;
            // char *ident;
            struct astnode_ident ident; 
            char char_lit;
            struct string_literal str_lit;
            struct astnode_fncall fncall;
            struct astnode_for_loop for_loop;  
            struct astnode_while_loop while_loop;  
            struct astnode_if_stmt if_stmt; 
            struct linked_list * ds_list;
            // struct astnode_symbol tab_entry;
            // struct type_node t_node; 

    };
};


#endif /* PARSER_H */