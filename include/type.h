#ifndef TYPE_H
#define TYPE_H

#include "lexer.h"
#include "sym.h"

enum Type{
    ARRAY_TYPE, 
    POINTER_TYPE, 
    FUNCTION_TYPE, 
    SCALAR_TYPE, 
    IDENT_TYPE,
    STRUCT_UNION_TYPE,
    S_CLASS,
    LABEL_TYPE
};

struct ident_type{
    char *name; 
    enum namespace n_space;
    enum storage_class s_class; 
};

struct scalar_type{
    enum num_type arith_type; 
    enum storage_class s_class; 
};

// struct type_node * create_array_node(int size);
struct top_tail * make_tt_node();
struct top_tail * create_scalar_node(enum num_type arith);
struct top_tail * create_s_class_node(enum storage_class s_class);
struct top_tail * create_function_node(struct top_tail * direct_declarator);
struct top_tail * create_stu_node();
struct top_tail * init_tt_node();
struct decl_list * make_decl_list_node();

struct type_node * make_type_node(enum Type type);
struct type_node * push_next_type(enum Type type, struct type_node *prev, struct type_node * next);
void print_type(struct type_node * type, int depth);
int check_type_specifier(struct type_node * head);

// For declarations and function definitions
void new_function_defs(struct top_tail * specifiers, struct top_tail * declarator);
void new_declaration(struct top_tail * specifiers, struct top_tail * declarator, int param);

// Function type node - includes return type and linked list of parameters
struct function_type {
    struct type_node * return_type; 
    struct astnode_symbol * param_head; 

};

// Struct to help keep track of top and tail of linked list of types
struct top_tail{
    struct type_node * top; 
    struct type_node * tail; 
};

// For declaration lists
struct decl_list{
    struct top_tail * item; 
    struct decl_list * next_decl; 
};

enum forward {
    INCOMPLETE = 0,
    COMPLETE = 1, 
};

enum stu_type {
    STRUCT_TYPE, 
    UNION_TYPE
};

// Struct for struct or unions
struct struct_union_type{
    enum stu_type stu_type;
    enum forward complete;
    char * ident;
    struct astnode_symbol * refers_to; 

    // Mini symbol table for struct members
    struct astnode_symbol * mini_head; 
};

// Node to represent a type
struct type_node{
    enum Type type;
    struct type_node * next_type; 
    union{
        struct ident_type ident;
        struct scalar_type scalar; 
        int size;
        struct function_type func_node;
        struct struct_union_type stu_node;
    };
};


#endif /* TYPE_H */