#include "assemble.h"

void gen_assembly(){
    if (curr_scope->s_type != GLOBAL_SCOPE)
        die("Must be in global scope when generating target code");

    printf("- - - - - \t\tGENERATING ASSEMBLY FROM QUADS\t\t - - - - - \n");

    // Loop through global symbol table
    // if variable -> .comm var_name, 4, 4 (if int)
    // if function -> .globl func_name     and then I start generate

    struct astnode_symbol * tmp = curr_scope->head;
    while(tmp != NULL){
        // Don't do anything for functioin declaration...
        if (tmp->type->type == FUNCTION_TYPE && tmp->symbol_k == DECL){
            printf("// FUNCTION DECLARATION, DO NOTHING\n");

            tmp = tmp->next; 
            continue; 
        }
        if (tmp->type->type == FUNCTION_TYPE){
            // Function def, standard x86 start
            printf(".%s:\n", tmp->name);
            printf("\tpusl %%ebp\n");
            printf("\tmovl %%esp, %%ebp\n");

            // Loop through basic blocks associated with the function 
            struct basic_block * bb = tmp->b_block;
            while(bb){
                // Print label
                printf("%s\n", bb->label);

                // Loop through all the quads in the given basic block 
                struct quad * q = bb->head; 
                while(q){
                    printf("\t");
                    parse_quad(q);
                    q = q->next_quad;
                }
                bb = bb->next_block;
            }
            // print_basic_block(tmp->b_block);
            tmp = tmp->next; 
            continue; 
        }

        make_code_section(tmp->name);
        // print_symbol(tmp, 0);
        tmp = tmp->next; 
    }    
}


// Helper bss variable (comm directive)
void make_code_section(char * var){
    printf("D%d:\n", section_counter);
    section_counter++;

    // Assuming everything is an int or a pointer
    printf("\t.comm %s, 4, 4\nc", var);
}


// Generate code for a given quad 
void parse_quad(struct quad * q){
    switch(q->opcode){
        case MOV:       {printf("movl %s, %s", parse_operand(q->src1), parse_operand(q->result)); break;}
        default:        {printf("Unsupported...");}
    }
    printf("\n");
}


char * parse_operand(struct generic_node * node){
    char * tmp; 
    switch(node->type){
        case VARIABLE:  {return node->var.name;}
        // case TEMPORARY:  {return node->var.name;}
        // case STRING_LITERAL:  {return node->var.name;}
        case CONSTANT:  {
                            asprintf(&tmp, "$%lld", node->num.integer);           // Again everything is an int for now
                            return tmp; 
                        }
        default:        {printf("Unsupported...\n");}
    }

}

void gen_list_strings();