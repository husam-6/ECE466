#include "parser.h"
#include "parser.tab.h"
#include <string.h>

struct astnode * create_scalar_node(enum num_type arith){
    struct astnode * tmp = make_ast_node(SCALAR_TYPE);
    tmp->t_node.scalar.arith_type = arith;
    return tmp; 
}

struct astnode * push_next_type(enum node_type type, struct astnode * prev, struct astnode * next){
    if (prev == next){
        printf("Loop?\n");
    }

    // Make node
    struct astnode * next_type = make_ast_node(type);

    // Point newly created node to next in sequence
    next_type->t_node.next_type = next;

    // Point previous node to newly created node
    prev->t_node.next_type = next_type;

    return next_type;
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
// void add_symbol_entry(char * ident, int namespace, int kind, int stg_class){
void add_symbol_entry(char * ident){
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