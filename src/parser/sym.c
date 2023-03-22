#include "parser.h"
#include "parser.tab.h"
#include "type.h"
#include "sym.h"
#include <string.h>

struct astnode_symbol * make_symbol_node() {
    struct astnode_symbol *node = (struct astnode_symbol *)malloc(sizeof(struct astnode_symbol));
    return node;
}


// Print symbol table identifiers
void print_symbol_table(){
    printf("------------\tPRINTING SYMBOL TABLE...\t------------\n");
    struct astnode_symbol * tmp; 
    tmp = curr_scope.head; 
    while (tmp->next != NULL){
        printf("- Symbol Ident: %s\n", tmp->name);
        printf("- With the following type: \n");
        print_type(tmp->type, 0);
        tmp = tmp->next; 
    }
    printf("- Symbol Ident: %s\n", tmp->name);
    printf("- With the following type: \n");
    print_type(tmp->type, 0);
}


// Symbol table helper function to check for an entry
// Returns 2 if table was empty when called
// Returns 1 if the entry is in the table
// Returns 0 if it isnt
int check_for_symbol(char * ident, enum namespace n_space){
    struct astnode_symbol * tmp; 
    tmp = curr_scope.head; 
    if (tmp == NULL)
        return 2; 

    // Check all entries in symbol table
    while (tmp->next != NULL){
        // Check if we find the identifier with the same namespace...
        if ((!strcmp(tmp->name, ident)) && (tmp->n_space == n_space)){
            return 1;
        }
        tmp = tmp->next; 
    }

    // Check the last node as well
    if (((!strcmp(tmp->name, ident))) && (tmp->n_space == n_space))
        return 1;

    return 0;
}

// Add symbol to symbol table
// void add_symbol_entry(char * ident, int namespace, int kind, int stg_class){
void add_symbol_entry(char * ident, struct type_node * type, enum namespace n_space){
    int in_table = check_for_symbol(ident, n_space);     // Only checks in current scope for now

    // Already in table
    if (in_table == 1){
        yyerror("Redeclaration of variable");
        exit(2);
    }
    
    // Only saving identifier and type for now...
    struct astnode_symbol * new_symbol = make_symbol_node(); 
    if (in_table == 2){
        // Push onto symbol stack 
        new_symbol->name = ident; 
        new_symbol->type = type; 
        new_symbol->next = NULL;
        new_symbol->n_space = n_space; 
        curr_scope.head = new_symbol; 
        return;
    }


    // Push onto symbol stack 
    new_symbol->name = ident; 
    new_symbol->type = type; 
    new_symbol->next = curr_scope.head; 
    new_symbol->n_space = n_space; 
    curr_scope.head = new_symbol; 
}