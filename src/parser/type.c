#include "parser.h"
#include "type.h"
#include "parser.tab.h"
#include <string.h>

void print_type(struct type_node * head, int depth){
    if (head == NULL)
        return;
    switch(head->type){
        case IDENT_TYPE: {
            n_tabs(depth);
            printf("IDENT %s\n", head->ident.name);
            print_type(head->next_type, depth+1);
            break;
        }
        case ARRAY_TYPE:{
            n_tabs(depth);
            printf("ARRAY TYPE NODE: size = %d\n", head->size);
            print_type(head->next_type, depth+1);
            break;
        }
        case SCALAR_TYPE:{
            n_tabs(depth);
            printf("SCALAR TYPE NODE: %s\n", print_datatype(head->scalar.arith_type));
            print_type(head->next_type, depth+1);
            break;
        }
        case POINTER_TYPE:{
            n_tabs(depth);
            printf("POINTER TYPE NODE\n");
            print_type(head->next_type, depth+1);
            break;
        }
        case FUNCTION_TYPE:{
            n_tabs(depth);
            printf("FUNCTION TYPE NODE\n");

            // Print return type
            n_tabs(depth+1);
            printf("RETURN TYPE:\n");
            print_type(head->func_node.return_type, depth+2);

            // Also print parameter list
            n_tabs(depth+1);
            printf("PARAMETERS:\n");
            print_params(head->func_node.param_head, depth+2);
            print_type(head->next_type, depth+1);
            break;
        }
        case S_CLASS:{
            n_tabs(depth);
            printf("STORAGE CLASS: %s\n", print_s_class(head->scalar.s_class));
            print_type(head->next_type, depth+1);
            break;
        }
        case STRUCT_UNION_TYPE:{
            n_tabs(depth);
            printf("%s %s ", print_union_struct(head->stu_node.stu_type), head->stu_node.ident);
            if (head->stu_node.refers_to->type->stu_node.complete == INCOMPLETE){
                printf("(incomplete)\n");
            }
            else{
                printf("(defined at %s:%d)\n", head->stu_node.refers_to->file_name, head->stu_node.refers_to->line_num);
            }
            print_type(head->next_type, depth+1);


            break; 
        }
        default: {
            fprintf(stderr, "Unknown type node encountered...%d\n", head->type);
            exit(2);
        }
    }
}

// Helper function to print function parameters
void print_params(struct astnode_symbol * head, int depth){
    int i = 1;
    if (head == NULL){
        n_tabs(depth);
        printf("Unknown arguments\n");
        return; 
    }
    while(head->next != NULL){
        n_tabs(depth);
        printf("Argument %d: ident %s\n", i, head->name);
        print_type(head->type, depth+1);
        i++; head = head->next;
    }
    n_tabs(depth);
    printf("Argument %d: ident %s\n", i, head->name);
    print_type(head->type, depth+1);
}

// Allocate memory for a type node
struct type_node * make_type_node(enum Type type){
    struct type_node *node = (struct type_node *)malloc(sizeof(struct type_node));
    node->type = type;
    return node;
}

// Allocate memory for a top-tail node (for building types)
struct top_tail * make_tt_node(){
    return (struct top_tail *)malloc(sizeof(struct top_tail));
}

// Allocate memory for a scalar node
struct top_tail * create_scalar_node(enum num_type arith){
    struct top_tail * tt = init_tt_node(SCALAR_TYPE);
    tt->top->scalar.arith_type = arith;
    tt->top->scalar.s_class = EXTERN_S;     // Default to extern, change later on
    return tt; 
}

// Allocate memory for a storage class node
struct top_tail * create_s_class_node(enum storage_class s_class){
    struct top_tail * tt = init_tt_node(S_CLASS);
    tt->top->scalar.s_class = s_class;
    return tt; 
}

// Allocate memory for a struct/union type node
struct top_tail * create_stu_node(enum stu_type type){
    struct top_tail * tt = init_tt_node(STRUCT_UNION_TYPE);
    tt->top->stu_node.complete = INCOMPLETE;
    tt->top->stu_node.stu_type = type; 
    return tt; 
}

// Helper to allocate memory and initialize a function node
struct top_tail * create_function_node(struct top_tail * direct_declarator){
    // Could be definition 
    // Change namespace to function (unless its a pointer to a function!)
    if (direct_declarator->top->next_type == NULL){
        direct_declarator->top->ident.n_space = FUNC_S;
        direct_declarator->top->ident.s_class = EXTERN_S;
    }
    struct type_node * tmp = push_next_type(FUNCTION_TYPE, direct_declarator->tail, NULL);
    direct_declarator->tail = tmp;
    return direct_declarator;
}

// Initalize first top_tail node
struct top_tail * init_tt_node(enum Type type){
    // Make new type node
    struct type_node * tmp = make_type_node(type);

    // Create a top_tail struct that points top and tail towards it
    struct top_tail * tt = make_tt_node(); 
    tt->top = tmp; 
    tt->tail = tmp;
    tmp->next_type = NULL; 
    return tt; 
}

// Helper function to push a new type to a linked list of types (given previous and next node in sequence)
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


// Returns 1 if valid
// Returns 0 if invalid
int check_type_specifier(struct type_node * head){
    if (head->type == STRUCT_UNION_TYPE){
        return 1;
    }
    int tmp = head->scalar.arith_type;
    
    // If just a basic type, valid
    if (tmp == V || tmp == I || tmp == C || tmp == F || tmp == D){
        if (head->next_type != NULL)
            return check_type_specifier(head->next_type);
        return 1; 
    }


    // If signed...
    if (tmp == S){
        if (head->next_type == NULL)
            return 1; 
        head = head->next_type;
        tmp = head->scalar.arith_type; 

        // Can only have signed char, signed short, signed int, signed long
        if (tmp != C && tmp != SH && tmp != L && tmp != I)
            return 0;
        return check_type_specifier(head);
    }
        
    // If unsigned
    if (tmp == U){
        if (head->next_type == NULL)
            return 1; 

        head = head->next_type;
        tmp = head->scalar.arith_type;

        // Can only have unsigned char, unsigned short, unsigned int, unsigned long
        if (tmp != C && tmp != SH && tmp != L && tmp != I)
            return 0;

        return check_type_specifier(head);
    }

    // If long
    if (tmp == L){
        if (head->next_type == NULL)
            return 1; 
        
        head = head->next_type;
        tmp = head->scalar.arith_type;

        // can only have 2 consecutive longs
        if (tmp == L){
            if (head->next_type == NULL)
                return 1; 
        
            head = head->next_type;
            tmp = head->scalar.arith_type;

            if (tmp != L)
                return check_type_specifier(head);
        }
        else if (tmp != I && tmp != D && tmp != U)      // Must be long int, long long int, or long double
            return 0;
        
        return check_type_specifier(head);
    }

    // if short
    if (tmp == SH){
        if (head->next_type == NULL)
            return 1; 
        
        head = head->next_type;
        tmp = head->scalar.arith_type;

        // Can only have short int or short
        if (tmp != I){
            return 0; 
        }

        return check_type_specifier(head);
    }


    return 0;
}

// Prepare and add a new function definiton to the symbol table
void new_function_defs(struct top_tail * specifiers, struct top_tail * declarator){
    declarator->top->ident.s_class = EXTERN_S;                                // Functions should be extern by default
    
    // Remove temporary storage class node if it exists
    if (specifiers->top->type == S_CLASS){
        // Check if storage class is valid for a function...
        if (specifiers->top->scalar.s_class == AUTO_S || specifiers->top->scalar.s_class == REGISTER_S){
                yyerror("INVALID STORAGE CLASS FOR FUNCTION");
                exit(2);
        }
        // Save storage class info
        declarator->top->ident.s_class = specifiers->top->scalar.s_class;
        specifiers->top = specifiers->top->next_type;
    }

    // Check if storage class is valid for a function...
    if (specifiers->top->scalar.s_class == AUTO_S || specifiers->top->scalar.s_class == REGISTER_S){
        yyerror("INVALID STORAGE CLASS FOR FUNCTION");
        exit(2);
    }

    // Chain types
    if (declarator->tail->type == FUNCTION_TYPE){
        // Check if return value is currently null
        if (declarator->tail->func_node.return_type == NULL){
            declarator->tail->func_node.return_type = specifiers->top;
        }
        else{
            struct type_node * tmp_return_type = declarator->tail->func_node.return_type;
            // Append to end (not keeping track of tail properly here)
            while(tmp_return_type->next_type != NULL)
                tmp_return_type = tmp_return_type->next_type;
            tmp_return_type->next_type = specifiers->top;
        }
    }
    else{
        // If its not a function, just chain the types...
        declarator->tail->next_type = specifiers->top;
    }


    // Tmp var for functions, top points to identifier type node
    struct type_node * tmp_func = declarator->top->next_type; 
    if (tmp_func->func_node.return_type == NULL)
        tmp_func->func_node.return_type = create_scalar_node(I)->top;         // Default to int return type for a function

    // Check if type is valid
    int r = check_type_specifier(specifiers->top);
    if (!r){
        yyerror("INVALID TYPE SPECIFIER,");
        exit(2);
    }


    // Save parameter list in function node (if we have one...)
    if (curr_scope->s_type == PROTOTYPE_SCOPE){
        curr_scope->head = reverse(curr_scope->head);
        tmp_func->func_node.param_head = curr_scope->head;
    }

    add_symbol_entry(declarator->top->ident.name, declarator->top->next_type, declarator->top->ident.n_space, declarator->top->ident.s_class, DEF);
}

void struct_union_decl(struct top_tail * specifiers, struct top_tail * declarator){
    struct astnode_symbol * refers_to; 
    
    // Look for the struct type
    int in_table = search_all_tabs(specifiers->top->stu_node.ident, TAG_S, curr_scope, &refers_to);

    // Not in table, so declare a an incomplete struct type here
    if (in_table == 2 || in_table == 0){
        add_symbol_entry(specifiers->top->stu_node.ident, specifiers->top, TAG_S, NON_VAR, declarator->top->ident.s_class, DECL);
        specifiers->top->stu_node.refers_to = curr_scope->head; 
        // yyerror("INVALID STRUCT DECLARATION");
        // exit(2);
    }
    
    // Point the type of the new struct being declared to the one in the symbol table
    if (in_table == 1){
        specifiers->top->stu_node.refers_to = refers_to; 
    }

    // Chain types together
    declarator->tail->next_type = specifiers->top;

    add_symbol_entry(declarator->top->ident.name, declarator->top->next_type, declarator->top->ident.n_space, declarator->top->ident.s_class, DECL);
}

// Prepare and add a new declaration to the symbol table
void new_declaration(struct top_tail * specifiers, struct top_tail * declarator, int param){    

    if (specifiers->top->type == STRUCT_UNION_TYPE){
        struct_union_decl(specifiers, declarator);
        return;
    }
    if (param == 0 && curr_scope->s_type == PROTOTYPE_SCOPE)
        close_outer_scope();

    // Check if storage class should be assumed as AUTO
    if ((curr_scope->s_type == PROTOTYPE_SCOPE || curr_scope->s_type == FUNC_SCOPE || curr_scope->s_type == BLOCK_SCOPE) 
            && declarator->top->ident.n_space != FUNC_S){    // To verify we aren't setting a function node to have auto storage class... 
            declarator->top->ident.s_class = AUTO_S; 
    }

    // Check for explicit storage class node
    if (specifiers->top->type == S_CLASS){
            // Check if storage class is valid 
            int tmp = specifiers->top->scalar.s_class;
            if ((tmp == AUTO_S || tmp == REGISTER_S) && curr_scope->s_type == GLOBAL_SCOPE){
                yyerror("INVALID STORAGE CLASS SPECIFIERS IN GLOBAL SCOPE");
                exit(2);
            }

            // Update storage class before removing if function
            declarator->top->ident.s_class = specifiers->top->scalar.s_class;
            specifiers->top = specifiers->top->next_type;
    }

    // Chain types
    if (declarator->tail->type == FUNCTION_TYPE){
        // Check if null... 
        if (declarator->tail->func_node.return_type == NULL){
            declarator->tail->func_node.return_type = specifiers->top;
        }
        else{
            // Add to end if there already is a return type (again not keeping track of tail, should fix)
            struct type_node * tmp_return_type = declarator->tail->func_node.return_type;
            while(tmp_return_type->next_type != NULL)
                tmp_return_type = tmp_return_type->next_type;
            tmp_return_type->next_type = specifiers->top;
        }
    }
    else
        declarator->tail->next_type = specifiers->top;      // If its just a regular node, append types


    struct type_node * tmp = declarator->top->next_type;
    if (tmp->func_node.return_type == NULL)
        tmp->func_node.return_type = create_scalar_node(I)->top;

    // Check if type is valid
    int r = check_type_specifier(specifiers->top);
    if (!r && declarator->top->ident.n_space != NON_VAR){
            yyerror("INVALID TYPE SPECIFIER");
            exit(2);
    }

    // Add to symbol table
    add_symbol_entry(declarator->top->ident.name, tmp, declarator->top->ident.n_space, 
                        declarator->top->ident.s_class, DECL);
}