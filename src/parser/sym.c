#include "parser.h"
#include "parser.tab.h"
#include "type.h"
#include "sym.h"
#include <string.h>

struct astnode_symbol * make_symbol_node() {
    struct astnode_symbol *node = (struct astnode_symbol *)malloc(sizeof(struct astnode_symbol));
    return node;
}

// Helper functions for printing symbol table elements
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
        case (GLOBAL_SCOPE):                {return "GLOBAL";};
        case (FUNC_SCOPE):                  {return "FUNCTION";};
        case (BLOCK_SCOPE):                 {return "BLOCK";};
        case (MEMBER_SCOPE):                {return "MEMBER";};
        case (PROTOTYPE_SCOPE):             {return "PROTOTYPE";};
        default:                            {return "UNKNOWN";}
    }
}

char * print_def_decl(enum symbol_kind k){
    switch (k){
        case DECL: {return "DECLARATION";}
        case DEF:  {return "DEFINITION";}
    }
}

char * print_union_struct(enum stu_type stu){
    switch(stu){
        case STRUCT_TYPE: {return "STRUCT";}
        case UNION_TYPE:  {return "UNION";}
    }
}

void print_scope_symbols(struct scope * curr_scope){
        struct astnode_symbol * tmp = curr_scope->head; 
        while (tmp != NULL){
            print_symbol(tmp, 0); 
            tmp = tmp->next; 
        }
}


// Print symbol table identifiers
void print_symbol_table(){
    printf("------------\tPRINTING SYMBOL TABLE...\t------------\n"); 
    struct scope * tmp_scope = curr_scope;
    while(tmp_scope != NULL){
        print_scope_symbols(tmp_scope);
        printf("EXITING %s SCOPE...\n", print_scope(tmp_scope->s_type));
        tmp_scope = tmp_scope->outer;
    }
    printf("------------\tEND OF SYMBOL TABLE\t------------\n");

}

void print_struct_members(struct astnode_symbol * mini_head){
    while (mini_head != NULL){
        print_symbol(mini_head, 2);
        mini_head = mini_head->next;
    }
}

void print_struct_symbol(struct astnode_symbol * sym){
    printf("%s\n", print_def_decl(sym->symbol_k));

    printf("\t-%s %s, at %s:%d,", print_union_struct(sym->type->stu_node.stu_type), sym->name, sym->file_name, sym->line_num);
    printf(" in Scope: %s\n", print_scope(sym->scope));
    if (sym->symbol_k == DECL){
        printf("\t-with Storage Class: %s, Namespace: %s\n", print_s_class(sym->s_class), print_namespace(sym->n_space));
    }
    if (sym->type->stu_node.mini_head != NULL){
        printf("\t-With members: \n");
        print_struct_members(sym->type->stu_node.mini_head);
    }
}

// Prints given declaration (astnode_symbol) (takes in a given symbol and the scope from which it came from)
void print_symbol(struct astnode_symbol * sym, int tabs){
    if (sym->type->type == STRUCT_UNION_TYPE && sym->n_space == TAG_S){
        print_struct_symbol(sym);
        return;
    }
    n_tabs(tabs);
    printf("%s\n", print_def_decl(sym->symbol_k));
    n_tabs(tabs);
    printf("\t- Symbol Ident: %s, Storage Class: %s, Namespace: %s, Scope: %s\n", 
            sym->name, print_s_class(sym->s_class), print_namespace(sym->n_space), print_scope(sym->scope));
    n_tabs(tabs);
    printf("\t- Declared in file %s:%d, with the following type: \n", sym->file_name, sym->line_num);
    print_type(sym->type, 2 + tabs);
}



// Returns 1 if types given are equivalent
// Returns 0 if not
// Doesn't account for functions for now... 
int check_types(struct type_node * type_1, struct type_node * type_2){
    if (type_1 == NULL && type_2 == NULL)
        return 1; 
    if (type_1 == NULL || type_2 == NULL)
        return 0; 
    if (type_1->type != type_2->type)
        return 0; 
    
    // Arrays
    if (type_1->type == ARRAY_TYPE){
        if (type_1->size != type_2->size)
            return 0; 
        else
            return check_types(type_1->next_type, type_2->next_type);
    }

    // Pointers
    if (type_1->type == POINTER_TYPE)
        return check_types(type_1->next_type, type_2->next_type);
    
    // Scalars
    if (type_1->type == SCALAR_TYPE){
        if (type_1->scalar.arith_type != type_2->scalar.arith_type || type_1->scalar.s_class != type_2->scalar.s_class){
            return 0;
        }
        return check_types(type_1->next_type, type_2->next_type);
    }

    // Storage class nodes
    if (type_1->type == S_CLASS){
        if (type_1->scalar.s_class != type_2->scalar.s_class)
            return 0; 
        return check_types(type_1->next_type, type_2->next_type);
    }


    return 0;
}


// Function to check whether a re-declaration is valid
// Returns 1 if valid
// Returns 0 if not
int valid_redecl(struct astnode_symbol * first, struct astnode_symbol * second){
    if (first->scope == MEMBER_SCOPE)
        return 0; 

    // If functions, check if the return types are the same
    if (first->type->type == FUNCTION_TYPE){
        if (first->s_class != second->s_class)
            return 0; 
        return check_types(first->type->func_node.return_type, second->type->func_node.return_type);
    }

    // If variable with extern (assume extern is always the first type node)
    // Valid if second is not declared with auto, register, or static and types match
    if (first->type->type == S_CLASS && first->type->scalar.s_class == EXTERN_S){
        if (second->type->type == S_CLASS && second->type->scalar.s_class != EXTERN_S)       
            return 0; 
        
        return check_types(first->type->next_type, second->type);
    }
    

    // variables with no storage class specifier and no initializer are valid if types match 
    if (first->type->type != S_CLASS && second->type->type != S_CLASS && curr_scope->s_type == GLOBAL_SCOPE)
        return check_types(first->type, second->type);


    return 0; 
}

// Symbol table helper function to check for an entry
// Returns 2 if table was empty when called
// Returns 1 if the entry is in the table
// Returns 0 if it isnt
int check_for_symbol(char * ident, enum namespace n_space, struct scope * scope, struct astnode_symbol ** symbol_found){
    struct astnode_symbol * tmp; 
    tmp = scope->head;
    if (tmp == NULL)
        return 2; 

    // Check all entries in symbol table
    while (tmp->next != NULL){
        // Check if we find the identifier with the same namespace...
        if ((!strcmp(tmp->name, ident)) && (tmp->n_space == n_space)){
            if (symbol_found != NULL)
                (*symbol_found) = tmp; 
            return 1;
        }
        tmp = tmp->next; 
    }

    // Check the last node as well
    if (((!strcmp(tmp->name, ident))) && (tmp->n_space == n_space)){
        if (symbol_found != NULL)
            (*symbol_found) = tmp; 
        return 1;
    }

    return 0;
}

// Search entire symbol table for an identifier
// Returns 2 if table was empty when called
// Returns 1 if the entry is in the table
// Returns 0 if it isnt
int search_all_tabs(char * ident, enum namespace n_space, struct scope * tmp_scope, struct astnode_symbol ** symbol_found){
    int in_table;
    while((in_table = check_for_symbol(ident, n_space, tmp_scope, symbol_found)) != 1){
        tmp_scope = tmp_scope->outer;
        if (tmp_scope == NULL)
            break;
    }
    // int in_table = check_for_symbol(ident, n_space, tmp_scope, symbol_found);
    if (in_table == 1){
        return 1; 
    }
    return 0; 
}

// Add symbol to symbol table
void add_symbol_entry(char * ident, struct type_node * type, enum namespace n_space, 
                      enum storage_class s_class, enum symbol_kind symbol_k)
{
    struct scope * tmp_scope = curr_scope;
    int proto = 0;
    if ((curr_scope->s_type == PROTOTYPE_SCOPE && n_space == FUNC_S) || (curr_scope->s_type == FUNC_SCOPE && n_space == FUNC_S)){
        while(tmp_scope->outer != NULL)
            tmp_scope = tmp_scope->outer; 
        proto = 1; 
        // s_class = EXTERN_S; 
    }

    // Structs must be installed outside of member scope
    if (curr_scope->s_type == MEMBER_SCOPE && n_space == TAG_S){
        while(tmp_scope->s_type == MEMBER_SCOPE)
            tmp_scope = tmp_scope->outer;
        proto = 1;
    }

    struct astnode_symbol * symbol_found;
    int in_table;
    in_table = check_for_symbol(ident, n_space, tmp_scope, &symbol_found);     // Only checks in current scope for now

    if (n_space == TAG_S){
        in_table = search_all_tabs(ident, n_space, tmp_scope, &symbol_found);
    }

    // int in_table = check_for_symbol(ident, n_space, tmp_scope, NULL);     // Only checks in current scope for now

    struct astnode_symbol * new_symbol = make_symbol_node(); 

    // Push onto symbol stack 
    if (in_table == 2)
        new_symbol->next = NULL;            // If empty
    else
        new_symbol->next = tmp_scope->head; // Else push to top of stack


    // Change temporary function namespace to variable...
    if (n_space == FUNC_S)
        n_space = VAR_S;

    // Set variable parameters
    new_symbol->name = ident; 
    new_symbol->type = type; 
    new_symbol->n_space = n_space; 
    new_symbol->symbol_k = symbol_k; 
    new_symbol->s_class = s_class;
    new_symbol->line_num = line_num;  
    new_symbol->file_name = file_name;
    new_symbol->scope = tmp_scope->s_type; 

    // Already in table
    if (in_table == 1){
        // Check if the redeclaration is valid
        if (symbol_k == DECL && strcmp(ident, "1UNDEF") && n_space != TAG_S){
            if (!valid_redecl(symbol_found, new_symbol)){
                // print_symbol(new_symbol, 0); 
                yyerror("INVALID REDECLARATION");                   // Still should check for valid redeclarations
                exit(2);
            }
            return; 

        }
        if (symbol_k == DEF && symbol_found->symbol_k == DEF){
            // if were redefining a tag but in a new scope, its valid
            if (n_space != TAG_S || (n_space == TAG_S && symbol_found->scope == tmp_scope->s_type)){
                yyerror("INVALID REDEFINITION");                   // Still should check for valid redeclarations
                exit(2);
            }
        }
        // If we are defining a previously declared symbol
        if ((n_space == FUNC_S || n_space == TAG_S) && (symbol_k == DEF && symbol_found->symbol_k == DECL)){
            struct astnode_symbol * tmp = symbol_found->next; 
            (*symbol_found) = (*new_symbol);
            symbol_found->next = tmp; 
            // print_symbol(symbol_found, 0);
            return; 
        }
        if (n_space == TAG_S){
            // Declaring a previously defined symbol?
            if (symbol_k == DECL && symbol_found->symbol_k == DEF && (symbol_found->scope == tmp_scope->s_type))
                return; 
            // Declaring previously declared symbol?
            if (symbol_k == DECL && symbol_found->symbol_k == DECL && symbol_found->scope == tmp_scope->s_type)
                return; 
            
        }
    }
    tmp_scope->head = new_symbol; 

    // Print the newly inputted symbol 
    // print_symbol(new_symbol, 0);

    // Update curr_scope if it was altered
    if (proto)
        curr_scope->outer = tmp_scope; 
    else
        curr_scope = tmp_scope; 
}

// Scope helper function
struct scope * make_new_scope(enum scope_type s_type) {
    struct scope * node = (struct scope *)malloc(sizeof(struct scope));
    node->s_type = s_type;
    node->outer = NULL; 
    node->head = NULL; 
    return node;
}

// Function to push new scope on top of stack of scopes
void create_new_scope(enum scope_type s_type){
    struct scope * tmp = NULL; 
    // Figure out new scope in sequence
    if (s_type == MEMBER_SCOPE)
        tmp = make_new_scope(MEMBER_SCOPE);
    else if (curr_scope->s_type == BLOCK_SCOPE || curr_scope->s_type == FUNC_SCOPE){
        // Make new block scope
        tmp = make_new_scope(BLOCK_SCOPE);
    }
    else if (curr_scope->s_type == GLOBAL_SCOPE){
        if (s_type == PROTOTYPE_SCOPE)
            tmp = make_new_scope(PROTOTYPE_SCOPE);
        else
            tmp = make_new_scope(FUNC_SCOPE);
    }
    else if(curr_scope->s_type == PROTOTYPE_SCOPE){
        if (s_type != PROTOTYPE_SCOPE){
            // Just change prototype scope to now be functions cope
            curr_scope->s_type = FUNC_SCOPE;
            return;
        }
        tmp = make_new_scope(PROTOTYPE_SCOPE);
    }
    // Push new scope onto stack
    tmp->outer = curr_scope;
    curr_scope = tmp;
}

void close_outer_scope(){
    // For now not freeing any memory...
    struct scope * tmp = curr_scope; 
    curr_scope = curr_scope->outer; 
    free(tmp);
}

// Reverses a given astnode symbol linked list / stack
struct astnode_symbol * reverse(struct astnode_symbol * head){
    struct astnode_symbol * current = head;
    struct astnode_symbol * prev = NULL;
    struct astnode_symbol * next = NULL;

    while (current->next != NULL){
        next = current->next; 
        current->next = prev; 
        prev = current; 
        current = next; 
    }
    current->next = prev;

    return current;
}