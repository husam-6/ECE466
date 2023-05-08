#ifndef QUADS_H
#define QUADS_H

#include "lexer.h"
#include "type.h"
#include "sym.h"
#include "parser.h"

extern int func_counter, bb_counter, register_counter;
extern struct basic_block * block_head;  
extern struct basic_block * block_tail;  
extern struct basic_block * curr_block;  
extern struct basic_block * break_block;
extern struct basic_block * cont_block; 

// Possible opcodes IR can emit
enum quad_opcode{
    MOV,
    LOAD,
    LEA,
    CMP,
    STORE,
    ADD,
    SUB,
    MUL,
    DIV,
    ARG,
    ARGBEGIN,
    CALL,
    RETURN_QUAD,
    POSTINC,
    POSTDEC
};

enum branch_type{
    BR=1,
    BREQ,
    BRGT,
    BRLT,
    BRNEQ,
    BRGEQ,
    BRLEQ,
};

enum generic_type{
    VARIABLE,
    TEMPORARY,
    CONSTANT,
    STRING_LITERAL,
    CHAR_LITERAL
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
        char char_lit; 
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
    char * func_name; 

    // Head of quad linked list
    struct quad * head;
    struct quad * tail;
    struct basic_block *next_block;

    // Left and right branches for CFG
    struct basic_block *left;
    struct basic_block *right;
    enum branch_type branch; 
};

// Allocator functions
struct quad * create_quad();
struct basic_block * create_basic_block();

// Print basic block
void print_basic_block(struct basic_block * bb);
void dump_basic_blocks(struct basic_block * head);
void print_quad(struct quad * q);
void print_generic_node();

// Allocators
struct generic_node * make_generic_node(enum generic_type type);
struct generic_node * new_temporary();

// Emit quads
void emit(enum quad_opcode opcode, struct generic_node * src1, struct generic_node * src2, struct generic_node * dest);
void gen_quads(struct astnode * asthead, char * func_name);
struct generic_node * gen_assign(struct astnode * node);

// Size of operator + helpers
int size_of(struct type_node * type);
int determine_if_pointer(struct type_node * tt);
struct generic_node * make_tmp_type();
struct type_node * get_type_from_generic(struct generic_node * node);
struct generic_node * gen_lvalue(struct astnode * node, int * mode);
struct generic_node * gen_rvalue(struct astnode * node, struct generic_node * target);

// Control flow
void gen_if(struct astnode * if_node);
void link_bb();
struct generic_node * gen_condexpr();
void gen_stmt();

// Generate quads for a given operator type
struct generic_node * gen_unary_node(struct astnode * node, struct generic_node * target);
struct generic_node * gen_binary_node(struct astnode * node, struct generic_node * target);
struct generic_node * gen_fn_call(struct astnode * node, struct generic_node * target);
void gen_for_loop(struct astnode * node);
void gen_while_loop(struct astnode * node);
struct linked_list * reverse_ll(struct linked_list * head);
// Recurse through AST using DFS

// Create new temporary

// Emit new quad

#endif /* QUADS_H */

