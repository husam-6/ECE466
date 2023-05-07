#include "assemble.h"
#include <string.h>


void gen_assembly(){
    if (curr_scope->s_type != GLOBAL_SCOPE)
        die("Must be in global scope when generating target code");

    printf("//- - - - - \t\tGENERATING ASSEMBLY FROM QUADS\t\t - - - - -// \n");

    // print_symbol_table(1);

    // Open output file
    char * outputfile; 
    file_name[strlen(file_name)-2] = '\0';       // Remove .c
    asprintf(&outputfile, "./%s.s", file_name);
    // printf("FILENAME: %s\n", outputfile);
    fout = fopen(outputfile, "w+");

    // Loop through global symbol table
    // if variable -> .comm var_name, 4, 4 (if int)
    // if function -> .globl func_name     and then I start generate

    struct astnode_symbol * tmp = curr_scope->head;
    while(tmp != NULL){
        // Don't do anything for functioin declaration...
        if (tmp->type->type == FUNCTION_TYPE && tmp->symbol_k == DECL){
            fprintf(fout, "// FUNCTION DECLARATION, DO NOTHING\n");

            tmp = tmp->next; 
            continue; 
        }
        if (tmp->type->type == FUNCTION_TYPE){
            printf("GENERATING CODE FOR FUNCTION: %s\n", tmp->name);
            // Loop through all scope elements of inner scope
            struct scope * func_scope = tmp->inner_scope;
            int stack_offset = 0; 
            while(func_scope){
                // Loop through scope elements
                struct astnode_symbol * symbol = func_scope->head; 
                while (symbol != NULL){
                    // print_symbol(symbol, 0);
                    // printf("STACKOFFSET %d\n", 4 *stack_offset);

                    symbol->stack_offset = 4*stack_offset;
                    stack_offset++;
                    symbol = symbol->next; 
                }
                func_scope = func_scope->next_child;

                // Stop before seeing other scopes
                if (func_scope && func_scope->s_type == FUNC_SCOPE)
                    break;
            }

            // Global directive for function def
            fprintf(fout, ".globl %s\n", tmp->name);

            // Function def, standard x86 start
            fprintf(fout, "%s:\n", tmp->name);
            fprintf(fout, "\tpushl %%ebp\n");
            fprintf(fout, "\tmovl %%esp, %%ebp\n");

            // Reserve stack space for local and temp vars
            fprintf(fout, "\tsubl $%d, %%esp\n", 4 * stack_offset);

            // Loop through basic blocks associated with the function 
            struct basic_block * bb = tmp->b_block;
            int return_flag = 0; 
            while(bb){
                // Print label
                fprintf(fout, "%s: \n", bb->label);

                // Loop through all the quads in the given basic block 
                struct quad * q = bb->head; 
                while(q){
                    parse_quad(q);
                    if (q->opcode == RETURN_QUAD)
                        return_flag = 1; 
                    q = q->next_quad;
                }

                // Branch 
                switch (bb->branch){
                    case BR:        {fprintf(fout, "\tjmp %s\n", bb->left->label); break;}
                    case BREQ:      {fprintf(fout, "\tje %s\n\tjmp %s\n", bb->left->label, bb->right->label);  break;}
                    case BRGT:      {fprintf(fout, "\tjg %s\n\tjmp %s\n", bb->left->label, bb->right->label);  break;}
                    case BRLT:      {fprintf(fout, "\tjl %s\n\tjmp %s\n", bb->left->label, bb->right->label);  break;}
                    case BRNEQ:     {fprintf(fout, "\tjne %s\n\tjmp %s\n", bb->left->label, bb->right->label);  break;}
                    case BRGEQ:     {fprintf(fout, "\tjge %s\n\tjmp %s\n", bb->left->label, bb->right->label);  break;}
                    case BRLEQ:     {fprintf(fout, "\tjle %s\n\tjmp %s\n", bb->left->label, bb->right->label);  break;}
                }
                // printf("\t");

                bb = bb->next_block;
            }

            // Generate leave and ret at end of function
            if (!return_flag){
                fprintf(fout, "\tleave\n");
                fprintf(fout, "\tret\n");
            }

            // Write relevant sections for strings
            while(str_section_head){
                fprintf(fout, "%s:\n", str_section_head->label);
                fprintf(fout, "\t.string \"");

                // parse bytes in string (can't just print the string straight up)
                for (int i = 0; i < str_section_head->str.length; i++){
                    // fprintf(fout, "%hhx", yylval.str.content[i]);
                    char * tmp = to_char(str_section_head->str.content[i]);
                    fprintf(fout, "%s", tmp);
                }
                fprintf(fout, "\"\n");
                str_section_head = str_section_head->next;
            } 


            tmp = tmp->next; 
            continue; 
        }


        make_code_section(tmp->name);
        // print_symbol(tmp, 0);
        tmp = tmp->next; 
    }
    // fprintf(fout, "\n");
}


// Helper bss variable (comm directive)
void make_code_section(char * var){
    fprintf(fout, "D%d:\n", section_counter);
    section_counter++;

    // Assuming everything is an int or a pointer
    fprintf(fout, "\t.comm %s, 4, 4\n", var);
}


// Generate code for a given quad 
void parse_quad(struct quad * q){
    switch(q->opcode){
        case MOV:               {
                                    fprintf(fout, "\tmovl %s, %%eax\n", parse_operand(q->result)); 
                                    fprintf(fout, "\tmovl %s, %%eax\n", parse_operand(q->src1)); 
                                    fprintf(fout, "\tmovl %%eax, %s", parse_operand(q->result)); 
                                    break;
                                }
        case ARG:               {fprintf(fout, "\tpushl %s", parse_operand(q->src2)); break;}
        case CALL:              {fprintf(fout, "\tcall %s", parse_operand(q->src1)); break;}
        case RETURN_QUAD:       {
                                    // If theres a return value, move into eax and call ret
                                    if (q->src1)
                                        fprintf(fout, "\tmovl %s, %%eax\n", parse_operand(q->src1));
                                    fprintf(fout, "\tleave\n");
                                    fprintf(fout, "\tret");
                                    break;
                                }
        case ARGBEGIN:          {return;}
        case CMP:               {
                                    fprintf(fout, "\tmovl %s, %%eax\n", parse_operand(q->src1));
                                    fprintf(fout, "\tmovl %s, %%ebx\n", parse_operand(q->src2));
                                    fprintf(fout, "\tcmpl %%eax, %%ebx");
                                    break;
                                }
        default:                {fprintf(fout, "Unsupported quad opcode...");}
    }
    fprintf(fout, "\n");
}


char * parse_operand(struct generic_node * node){
    char * tmp; 
    switch(node->type){
        case VARIABLE:          {
                                    if (node->var.sym->stack_offset != -1){
                                        asprintf(&tmp, "%d(%%esp)", node->var.sym->stack_offset);
                                        return tmp; 
                                    }
                                    return node->var.name;
                                }
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