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
                    parse_quad(q);
                    q = q->next_quad;
                }
                bb = bb->next_block;
            }

            // Write relevant sections for strings
            while(str_section_head){
                printf("%s:\n", str_section_head->label);
                printf("\t.string \"");

                // parse bytes in string (can't just print the string straight up)
                for (int i = 0; i < str_section_head->str.length; i++){
                    // printf("%hhx", yylval.str.content[i]);
                    char * tmp = to_char(str_section_head->str.content[i]);
                    printf("%s", tmp);
                }
                printf("\"");
                printf("\t.string \"%s\"\n", str_section_head->str.content);
                str_section_head = str_section_head->next;
            } 

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
        case MOV:       {printf("\tmovl %s, %s", parse_operand(q->src1), parse_operand(q->result)); break;}
        case ARG:       {printf("\tpushl %s", parse_operand(q->src2)); break;}
        case CALL:      {printf("\tcall %s \n\tleave", parse_operand(q->src1)); break;}
        case ARGBEGIN:  {return;}
        default:        {printf("Unsupported quad opcode...");}
    }
    printf("\n");
}


char * parse_operand(struct generic_node * node){
    char * tmp; 
    switch(node->type){
        case VARIABLE:          {return node->var.name;}
        // case TEMPORARY:  {return node->var.name;}
        case STRING_LITERAL:    {
                                    // Return $.section_name for string
                                    asprintf(&tmp, "$.L%d", string_counter);
                                    char * str_section; 

                                    // Save name in list to create section later
                                    asprintf(&str_section, ".L%d", string_counter);
                                    string_counter++;

                                    // Push onto list of strings
                                    struct str_section * new_sec = create_str_section(); 
                                    new_sec->label = str_section; 
                                    new_sec->str = node->str;
                                    if (str_section_head){
                                        new_sec->next = str_section_head; 
                                        str_section_head = new_sec; 
                                    }
                                    else
                                        str_section_head = new_sec; 

                                    return tmp;
                                }
        case CONSTANT:          {
                                    asprintf(&tmp, "$%lld", node->num.integer);           // Again everything is an int for now
                                    return tmp; 
                                }
        default:        {printf("Unsupported operand...\n");}
    }

    return tmp; 

}

// For list of string literals 
struct str_section * create_str_section(){
    return (struct str_section *)malloc(sizeof(struct str_section));
}



void gen_list_strings();