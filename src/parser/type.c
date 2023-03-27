#include "parser.h"
#include "type.h"

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
    return tt; 
}

struct top_tail * create_s_class_node(enum storage_class s_class){
    struct top_tail * tt = init_tt_node(S_CLASS);
    tt->top->scalar.s_class = s_class;
    return tt; 
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