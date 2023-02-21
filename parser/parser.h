#pragma once 
#ifndef PARSER_H
#define PARSER_H

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
    INDIRECT_SELECT,
    LOGICAL_OP
};

enum node_type{
    FN_CALL = 0,
    UNARY_NODE,
    BINOP_NODE,
    TERNOP_NODE,
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
    int operator; 
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

// Numbers
struct astnode_num {
    enum num_type type; 
    union {
        unsigned long long integer; 
        long double frac; 
    };
};

// Identifiers 
struct astnode_ident {
    char * ident; 
};

// Char Literal
struct astnode_char_lit {
    char c; 
};

// String
struct astnode_str_lit {
    char * str; 
    int len;
};


// Node struct
struct astnode {
    enum node_type type;
    union {
            struct astnode_unary unary;
            struct astnode_binary binop;
            struct astnode_ternary ternop;
            struct astnode_num num;
            struct astnode_ident ident;
            struct astnode_char_lit char_lit;
            struct astnode_str_lit str_lit;
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