#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// Function prototypes
struct astnode * make_ast_node(int type);
struct astnode * create_unary(int op_type, int op, struct astnode *expr);
struct astnode * create_binary(int op_type, int op, struct astnode *left, struct astnode *right);
struct astnode * create_ternary(int op_type, struct astnode *left, struct astnode *middle, struct astnode *right);
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

struct astnode_fncall {
    int operator; 
    enum op_type operator_type;
    struct astnode *p[100];
};

// // Numbers
// struct astnode_num {
//     enum num_type type; 
//     union {
//         unsigned long long integer; 
//         long double frac; 
//     };
// };

// // Identifiers 
// struct astnode_ident {
//     char * ident; 
// };

// // Char Literal
// struct astnode_char_lit {
//     char c; 
// };

// // String
// struct astnode_str_lit {
//     char * str; 
//     int len;
// };


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
            // struct astnode_logop logop;
            // struct astnode_assigncomp assigncomp;
            // struct astnode_assign assign;
            // struct astnode_deref deref;
            // struct astnode_addressof addressof;
            // struct astnode_compop compop;
            // struct astnode_sizeof sizeof;
            // struct astnode_indselect indselect;
            //struct ternary_op tern;
    };
};


#endif /* PARSER_H */