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
};

struct ident_type{
    char *name; 
    enum namespace n_space;
    enum storage_class s_class; 
};

// struct type_node * create_array_node(int size);
struct type_node * create_scalar_node(enum num_type arith);
struct type_node * push_next_type(enum Type type, struct type_node *prev, struct type_node * next);
void print_type(struct type_node * type, int depth);
struct type_node * make_type_node(enum Type type);

// Node to represent a type
struct type_node{
    enum Type type;
    struct type_node * next_type; 
    union{
        struct ident_type ident;
        enum num_type arith_type;
        int size;
        // struct function_type func_node;
        // struct struct_union_type struct_union_node;
    };
};


#endif /* TYPE_H */