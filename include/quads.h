#ifndef QUADS_H
#define QUADS_H

#include "lexer.h"
#include "type.h"
#include "sym.h"
#include "parser.h"

extern int func_counter, bb_counter, register_counter;
extern struct basic_block * block_head;  
extern struct basic_block * curr_block;  

// Possible opcodes IR can emit
enum quad_opcode{
    MOV,
    LOAD,
    LEA,
    CMP,
    BR,
    BREQ,
    BRGT,
    BRLT,
    BRNEQ,
    BRGEQ,
    BRLEQ,
    STORE,
    ADD,
    SUB,
    MUL,
    DIV,
};

enum generic_type{
    VARIABLE,
    TEMPORARY,
    CONSTANT,
    STRING_LIT,
};

enum MODE{
    DIRECT, 
    INDIRECT,
};

enum op_arithmetic{
    POINTER_MINUS_POINTER,
    POINTER_MINUS_NUM,
    POINTER_PLUS_NUM,
    NUM_NUM
};

enum num_or_pointer{
    NUM_VAR,
    POINTER_VAR
};

// Generic node in a quad (basically all the terminals of an AST)
struct generic_node{
    enum generic_type type;
    union{
        struct astnode_ident var;
        struct temporary{
            int reg_num; 
            char * ident;
            struct type_node * operation_type; 
        } temp;
        struct number num;
        struct string_literal str;
    };
    enum num_or_pointer n_p; 
};

// Quad struct
struct quad {
    // Quad opcode
    enum quad_opcode opcode; 

    // src1 and src2 are rvalues, result an lvalue.
    struct generic_node *result, *src1, *src2;
    struct quad *next_quad; 
};

// Basic blocks
struct basic_block{
    char * label; 

    // Head of quad linked list
    struct quad * head;
    struct quad * tail;
    struct basic_block *next_block;
};

// Allocator functions
struct quad * create_quad();
struct basic_block * create_basic_block();

// Print basic block
void print_basic_block(struct basic_block * bb);
void print_quad(struct quad * q);
struct generic_node * make_generic_node(enum generic_type type);
struct generic_node * new_temporary();
void emit(enum quad_opcode opcode, struct generic_node * src1, struct generic_node * src2, struct generic_node * dest);
void gen_quads(struct linked_list * asthead, char * func_name);
struct generic_node * gen_assign(struct astnode * node);
int size_of(struct type_node * type);
int determine_if_pointer(struct type_node * tt);
struct generic_node * make_tmp_type();

// Recurse through AST using DFS

// Create new temporary

// Emit new quad

#endif /* QUADS_H */

