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
    printf("---------\tPRINTING SYMBOL TABLE...\t---------\n");
    struct astnode * tmp; 
    tmp = curr_scope.head; 
    while (tmp->tab_entry.next != NULL){
        printf("- Symbol Ident: %s\n", tmp->tab_entry.name);
        printf("- With the following type: \n");
        print_ast(tmp->tab_entry.type, 0);
        tmp = tmp->tab_entry.next; 
    }
    printf("- Symbol Ident: %s\n", tmp->tab_entry.name);
    printf("- With the following type: \n");
    print_ast(tmp->tab_entry.type, 0);
}


// Symbol table helper function to check for an entry
// Returns 2 if table was empty when called
// Returns 1 if the entry is in the table
// Returns 0 if it isnt
int check_for_symbol(char * ident){
    struct astnode * tmp; 
    tmp = curr_scope.head; 
    if (tmp == NULL)
        return 2; 

    // Check all entries in symbol table
    while (tmp->tab_entry.next != NULL){
        // Check if we find the identifier
        if (!strcmp(tmp->tab_entry.name, ident)){
            // Still need to check if the same type + namespace (could be a valid redeclaration as well)
            return 1;
        }
        tmp = tmp->tab_entry.next; 
    }

    if (tmp->tab_entry.name == ident)
        return 1;

    return 0;
}

// Add symbol to symbol table
// void add_symbol_entry(char * ident, int namespace, int kind, int stg_class){
void add_symbol_entry(char * ident, struct astnode * type){
    int in_table = check_for_symbol(ident);     // Only checks in current scope for now

    // Already in table
    if (in_table == 1){
        yyerror("Redeclaration of variable");
        exit(2);
    }
    
    // Only saving identifier and type for now...
    struct astnode * new_symbol = make_ast_node(AST_SYMBOL); 
    if (in_table == 2){
        // Push onto symbol stack 
        new_symbol->tab_entry.name = ident; 
        new_symbol->tab_entry.type = type; 
        curr_scope.head = new_symbol; 
        new_symbol->tab_entry.next = NULL; 
        return;
    }


    // Push onto symbol stack 
    new_symbol->tab_entry.name = ident; 
    new_symbol->tab_entry.type = type; 
    new_symbol->tab_entry.next = curr_scope.head; 
    curr_scope.head = new_symbol; 
}