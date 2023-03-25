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
    S_CLASS
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
struct top_tail * init_tt_node();
struct type_node * push_next_type(enum Type type, struct type_node *prev, struct type_node * next);
void print_type(struct type_node * type, int depth);
struct type_node * make_type_node(enum Type type);
int check_type_specifier(struct type_node * head);


// Function type node - includes return type and linked list of parameters
struct function_type {
    struct type_node * return_type; 
    struct astnode_symbol * param_head; 

};

struct top_tail{
    struct type_node * top; 
    struct type_node * tail; 
};

// Node to represent a type
struct type_node{
    enum Type type;
    struct type_node * next_type; 
    union{
        struct ident_type ident;
        struct scalar_type scalar; 
        // enum num_type arith_type;
        int size;
        struct function_type func_node;
        // struct struct_union_type struct_union_node;
    };
};


#endif /* TYPE_H */