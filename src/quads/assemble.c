#include "assemble.h"

void gen_assembly(){
    // dump_basic_blocks(block_head);
    printf("- - - - - \t\tGENERATING ASSEMBLY FROM QUADS\t\t - - - - - \n");

    // Loop through global symbol table
    // if variable -> .comm var_name, 4, 4 (if int)
    // if function -> .globl func_name     and then I start generate

    struct scope * tmp_scope = curr_scope;
    while(tmp_scope != NULL){
        
        printf("EXITING %s SCOPE...\n", print_scope(tmp_scope->s_type));
        tmp_scope = tmp_scope->outer;
    }
    

}