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


// Print symbol table identifiers
void print_symbol_table(){
    printf("------------\tPRINTING SYMBOL TABLE...\t------------\n");
    struct astnode_symbol * tmp; 
    tmp = curr_scope.head; 
    while (tmp->next != NULL){
        printf("- Symbol Ident: %s, Storage Class: %s, Namespace: %s, Scope: %d\n", 
               tmp->name, print_s_class(tmp->s_class), print_namespace(tmp->n_space), curr_scope.s_type);
        printf("- Declared on line %d, with the following type: \n", tmp->line_num);
        print_type(tmp->type, 0);
        tmp = tmp->next; 
    }
    printf("- Symbol Ident: %s, Storage Class: %s, Namespace: %s, Scope: %d\n", 
            tmp->name, print_s_class(tmp->s_class), print_namespace(tmp->n_space), curr_scope.s_type);
    printf("- Declared on line %d, with the following type: \n", tmp->line_num);
    print_type(tmp->type, 0);
}


// Print declaration
void print_declaration(struct astnode_symbol * decl){
    decl = curr_scope.head; 
    printf("- Symbol Ident: %s, Storage Class: %s, Namespace: %s, Scope: %d\n", 
            decl->name, print_s_class(decl->s_class), print_namespace(decl->n_space), curr_scope.s_type);
    printf("- Declared on line %d, with the following type: \n", decl->line_num);
    print_type(decl->type, 0);
}


// Symbol table helper function to check for an entry
// Returns 2 if table was empty when called
// Returns 1 if the entry is in the table
// Returns 0 if it isnt
int check_for_symbol(char * ident, enum namespace n_space, struct scope scope){
    struct astnode_symbol * tmp; 
    tmp = scope.head; 
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
    
    // Only saving identifier and type for now...
    struct astnode_symbol * new_symbol = make_symbol_node(); 
    
    // Push onto symbol stack 
    if (in_table == 2)
        new_symbol->next = NULL;            // If empty
    else
        new_symbol->next = curr_scope.head; // Else push to top of stack


    // Set variable parameters
    new_symbol->name = ident; 
    new_symbol->type = type; 
    new_symbol->n_space = n_space; 
    new_symbol->symbol_k = symbol_k; 
    new_symbol->s_class = s_class;
    new_symbol->line_num = line_num;  
    curr_scope.head = new_symbol; 
}


void create_new_scope(){
    if (curr_scope.s_type < 2){
        
    }

}