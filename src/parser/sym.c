#include "parser.h"
#include "parser.tab.h"
#include "type.h"
#include "sym.h"
#include <string.h>

struct astnode_symbol * make_symbol_node() {
    struct astnode_symbol *node = (struct astnode_symbol *)malloc(sizeof(struct astnode_symbol));
    return node;
}

char * print_s_class(enum storage_class s_class){
    switch(s_class){
        case (AUTO_S):              {return "AUTO";};
        case (REGISTER_S):          {return "REGISTER";};
        case (EXTERN_S):            {return "EXTERN";};
        case (STATIC_S):            {return "STATIC";};
        case (NON_VAR):             {return "NON_VAR";};
        default:                    {return "UNKNOWN";}
    } 
}

char * print_namespace(enum namespace n_space){
    switch(n_space){
        case (VAR_S):               {return "VAR";};
        case (FUNC_S):              {return "FUNC";};
        case (TAG_S):               {return "TAG";};
        case (LABEL_S):             {return "LABEL";};
        case (MEMBER_S):            {return "MEMBER";};
        default:                    {return "UNKNOWN";}
    }
}

char * print_scope(enum scope_type s_type){
    switch(s_type){
        case (S_GLOBAL):                {return "GLOBAL";};
        case (S_FUNC):                  {return "FUNCTION";};
        case (S_BLOCK):                 {return "BLOCK";};
        case (S_MEMBER):                {return "MEMBER";};
        case (S_PROTOTYPE):             {return "PROTOTYPE";};
        default:                        {return "UNKNOWN";}
    }
}


// Print symbol table identifiers
void print_symbol_table(){
    printf("------------\tPRINTING SYMBOL TABLE...\t------------\n");
    struct astnode_symbol * tmp; 
    struct scope * tmp_scope = curr_scope;
    while(tmp_scope->outer != NULL){
        tmp = tmp_scope->head; 
        while (tmp->next != NULL){
            print_declaration(tmp); 
            tmp = tmp->next; 
        }
        print_declaration(tmp);
    }
    tmp = tmp_scope->head; 
    while (tmp->next != NULL){
        print_declaration(tmp); 
        tmp = tmp->next; 
    }
    print_declaration(tmp);
}


// Prints given declaration (astnode_symbol)
void print_declaration(struct astnode_symbol * decl){
    printf("- Symbol Ident: %s, Storage Class: %s, Namespace: %s, Scope: %s\n", 
            decl->name, print_s_class(decl->s_class), print_namespace(decl->n_space), print_scope(curr_scope->s_type));
    printf("- Declared on line %d, with the following type: \n", decl->line_num);
    print_type(decl->type, 1);
}


// Symbol table helper function to check for an entry
// Returns 2 if table was empty when called
// Returns 1 if the entry is in the table
// Returns 0 if it isnt
int check_for_symbol(char * ident, enum namespace n_space, struct scope * scope){
    struct astnode_symbol * tmp; 
    tmp = scope->head; 
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
void add_symbol_entry(char * ident, struct type_node * type, enum namespace n_space, 
                      enum storage_class s_class, enum symbol_kind symbol_k)
{
    int in_table = check_for_symbol(ident, n_space, curr_scope);     // Only checks in current scope for now

    // Already in table
    if (in_table == 1){
        yyerror("Redeclaration of variable");                   // Still should check for valid redeclarations
        exit(2);
    }
    
    struct astnode_symbol * new_symbol = make_symbol_node(); 
    // Push onto symbol stack 
    if (in_table == 2)
        new_symbol->next = NULL;            // If empty
    else
        new_symbol->next = curr_scope->head; // Else push to top of stack


    // Set variable parameters
    new_symbol->name = ident; 
    new_symbol->type = type; 
    new_symbol->n_space = n_space; 
    new_symbol->symbol_k = symbol_k; 
    new_symbol->s_class = s_class;
    new_symbol->line_num = line_num;  
    curr_scope->head = new_symbol; 
}

// Scope helper function
struct scope * make_new_scope(enum scope_type s_type) {
    struct scope * node = (struct scope *)malloc(sizeof(struct scope));
    node->s_type = s_type; 
    return node;
}

// Function to push new scope on top of stack of scopes
void create_new_scope(){
    struct scope * tmp = NULL; 
    // Figure out new scope in sequence
    if (curr_scope->s_type == S_BLOCK || curr_scope->s_type == S_FUNC){
        // Make new block scope
        tmp = make_new_scope(S_BLOCK);
    }
    else if (curr_scope->s_type == S_GLOBAL){
        tmp = make_new_scope(S_PROTOTYPE);
    }
    else if(curr_scope->s_type == S_PROTOTYPE){
        // Just change prototype scope to now be functions cope
        curr_scope->s_type = S_FUNC;
        return;
    }
    tmp->outer = curr_scope;
    curr_scope = tmp;
}

void close_outer_scope(){
    // For now not freeing any memory...
    struct scope * tmp = curr_scope; 
    curr_scope = curr_scope->outer; 
    free(tmp);
}