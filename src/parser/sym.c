#include "parser.h"
#include "parser.tab.h"
#include <string.h>

struct astnode_symbol * make_symbol_node() {
    struct astnode_symbol *node = (struct astnode_symbol *)malloc(sizeof(struct astnode_symbol));
    return node;
}

struct type_node * make_type_node(enum Type type){
    struct type_node *node = (struct type_node *)malloc(sizeof(struct type_node));
    node->type = type;
    return node;
}


struct type_node * create_scalar_node(enum num_type arith){
    struct type_node *node = make_type_node(SCALAR_TYPE);
    node->scalar.arith_type = arith;
    return node; 
}

struct type_node * push_next_type(enum Type type, struct type_node * prev, struct type_node * next){
    if (prev == next){
        printf("Loop?\n");
    }

    // Make node
    struct type_node *node = make_type_node(type);

    // Point newly created node to next in sequence
    node->next_type = next;

    // Point previous node to newly created node
    prev->next_type = node;

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
int check_for_symbol(char * ident){
    struct astnode_symbol * tmp; 
    tmp = curr_scope.head; 
    if (tmp == NULL)
        return 2; 

    // Check all entries in symbol table
    while (tmp->next != NULL){
        // Check if we find the identifier
        if (!strcmp(tmp->name, ident)){
            // Still need to check if the same type + namespace (could be a valid redeclaration as well)
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
void add_symbol_entry(char * ident, struct type_node * type){
    int in_table = check_for_symbol(ident);     // Only checks in current scope for now

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
        curr_scope.head = new_symbol; 
        new_symbol->next = NULL; 
        return;
    }


    // Push onto symbol stack 
    new_symbol->name = ident; 
    new_symbol->type = type; 
    new_symbol->next = curr_scope.head; 
    curr_scope.head = new_symbol; 
}