#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "quads.h"
#include "sym.h"
#include "type.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

// Counter for dummy sections
extern int section_counter; 
extern int string_counter;
extern struct str_section * str_section_head; 
extern FILE * fout; 

// Generate assembly
void gen_assembly();

// Helpers to create sections for strings and bss variables
void make_code_section(struct astnode_symbol * sym);
void gen_list_strings();
char * parse_operand(struct generic_node * node);
void parse_quad(struct quad * q);
void print_mov_op(struct quad * q);

// Struct for list of strings 
struct str_section {
    char * label;  
    struct string_literal str;
    struct str_section * next; 
};

struct str_section * create_str_section();

#endif /* QUADS_H */