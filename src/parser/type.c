#include "parser.h"
#include "type.h"
#include "parser.tab.h"

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
            break;
        }
        case S_CLASS:{
            n_tabs(depth);
            printf("STORAGE CLASS: %s\n", print_s_class(head->scalar.s_class));
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

// Helper to allocate memory and initialize a function node
struct top_tail * create_function_node(struct top_tail * direct_declarator){
    // Could be definition 
    // Change namespace to func
    direct_declarator->top->ident.n_space = FUNC_S;
    direct_declarator->top->ident.s_class = EXTERN_S;
    struct type_node * tmp = push_next_type(FUNCTION_TYPE, direct_declarator->tail, NULL);
    tmp->func_node.return_type = create_scalar_node(I)->top;         // Default to int return type for a function
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

void new_function_defs(struct top_tail * specifiers, struct top_tail * declarator){
    // Remove temporary storage class node if it exists
    if (specifiers->top->type == S_CLASS){
        // Check if storage class is valid for a function...
        if (specifiers->top->scalar.s_class == AUTO_S || specifiers->top->scalar.s_class == REGISTER_S){
                yyerror("INVALID STORAGE CLASS FOR FUNCTION");
                exit(2);
        }
        declarator->top->ident.s_class = specifiers->top->scalar.s_class;
        specifiers->top = specifiers->top->next_type;
    }

    // Check if storage class is valid for a function...
    if (specifiers->top->scalar.s_class == AUTO_S || specifiers->top->scalar.s_class == REGISTER_S){
        yyerror("INVALID STORAGE CLASS FOR FUNCTION");
        exit(2);
    }

    // Chain types
    declarator->tail->next_type = specifiers->top;
    if (declarator->tail->type == FUNCTION_TYPE)
        declarator->tail->func_node.return_type = specifiers->top;


    // Tmp var for functions, top points to identifier type node
    struct type_node * tmp_func = declarator->top->next_type; 

    // Save function return type (next type gets saved in function node)
    tmp_func->func_node.return_type = tmp_func->next_type;  
    declarator->top->ident.s_class = EXTERN_S;

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

    // print_symbol_table();

    // create_new_scope();
}


void new_declaration(struct top_tail * specifiers, struct top_tail * declarator){
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

    declarator->tail->next_type = specifiers->top; 


    // If function, save return type
    struct type_node * tmp = declarator->top->next_type;
    if (tmp->type == FUNCTION_TYPE){
            tmp->func_node.return_type = tmp->next_type;
    }

    // Check if type is valid
    int r = check_type_specifier(specifiers->top);
    if (!r){
            yyerror("INVALID TYPE SPECIFIER");
            exit(2);
    }

    // Add to symbol table
    add_symbol_entry(declarator->top->ident.name, tmp, declarator->top->ident.n_space, 
                        declarator->top->ident.s_class, DECL);

    // Change this if you want to get prototypes working for real
    // if (curr_scope->s_type == PROTOTYPE_SCOPE)
    //         close_outer_scope();
}