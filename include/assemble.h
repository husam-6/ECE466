#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "quads.h"
#include "sym.h"
#include "type.h"

// Counter for dummy sections
extern int section_counter; 

// Generate assembly
void gen_assembly();

// Helpers to create sections for strings and bss variables
void make_code_section(char * var);
void gen_list_strings();
char * parse_operand(struct generic_node * node);

#endif /* QUADS_H */