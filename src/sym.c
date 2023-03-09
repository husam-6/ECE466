#include "parser.h"
#include "parser.tab.h"
#include <string.h>


struct astnode * create_array_node(int size){
    struct astnode *node = make_ast_node(ARRAY_TYPE);
    node->t_node.array_node.size = size;         // for now, -1 for undefined size
    node->t_node.next_type = NULL;
    return node; 
}


// Print symbol table identifiers
void print_symbol_table(){
    printf("Printing symbol table...\n");
    struct astnode_symbol * tmp; 
    tmp = curr_scope.head; 
    while (tmp->next != NULL){
        printf("Symbol Ident: %s\n", tmp->name);
        tmp = tmp->next; 
    }
    printf("Symbol Ident: %s\n", tmp->name);
}


// Symbol table helper function to check for an entry
// Returns 2 if table was empty when called
// Returns 1 if the entry is in the table
// Returns 0 if it isnt
int check_for_symbol(char * ident){
    struct astnode_symbol * tmp; 
    tmp = curr_scope.head; 
    if (tmp == NULL)
        return 2; 

    // Check all entries in symbol table
    while (tmp->next != NULL){
        // Check if we find the identifier
        if (!strcmp(tmp->name, ident)){
            // Still need to check if the same type (ie if its a valid redeclaration)
            return 1;
        }
        tmp = tmp->next; 
    }

    if (tmp->name == ident)
        return 1;

    return 0;
}

// Add symbol to symbol table
void add_symbol_entry(char * ident, int namespace, int kind, int stg_class){
    int in_table = check_for_symbol(ident);     // Only checks in current scope for now

    // Already in table
    if (in_table == 1){
        yyerror("Redeclaration of variable");
        exit(1);
    }
    
    // Only saving identifier for now...
    struct astnode_symbol * new_symbol = (struct astnode_symbol *)malloc(sizeof(struct astnode_symbol)); 
    if (in_table == 2){
        // Push onto symbol stack 
        new_symbol->name = ident; 
        curr_scope.head = new_symbol; 
        new_symbol->next = NULL; 
        return;
    }


    // Push onto symbol stack 
    new_symbol->name = ident; 
    new_symbol->next = curr_scope.head; 
    curr_scope.head = new_symbol; 
}