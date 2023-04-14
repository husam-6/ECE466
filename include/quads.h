#ifndef QUADS_H
#define QUADS_H

#include "lexer.h"
#include "type.h"
#include "sym.h"
#include "parser.h"

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
    BRGEQ,
    BRLEQ,
    STORE,
};

union generic_node{
    char * var; 
};

// Quad struct
struct quad {
    // Quad opcode
    enum quad_opcode opcode; 

    // src1 and src2 are rvalues, result an lvalue.
    union generic_node *result, *src1, *src2;
    struct quad *next_quad; 
};

// Basic blocks
struct basic_block{
    char * label; 

    // Head of quad linked list
    struct quad * head; 
    struct basic_block *next_block;

};

// Allocator functions
struct quad * create_quad();
struct basic_block * create_basic_block();

#endif /* QUADS_H */

