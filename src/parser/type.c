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
            print_type(head->func_node.return_type, depth+1);

            // Also print parameter list
            n_tabs(depth+1);
            printf("PARAMETERS:\n");
            print_params(head->func_node.param_head, depth+2);
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
    while(head->next != NULL){
        n_tabs(depth);
        printf("Argument %d: \n", i);
        print_type(head->type, depth+1);
        i++; head = head->next;
    }
    n_tabs(depth);
    printf("Argument %d: \n", i);
    print_type(head->type, depth+1);
}

struct type_node * make_type_node(enum Type type){
    struct type_node *node = (struct type_node *)malloc(sizeof(struct type_node));
    node->type = type;
    return node;
}

struct top_tail * make_tt_node(){
    return (struct top_tail *)malloc(sizeof(struct top_tail));
}


struct top_tail * create_scalar_node(enum num_type arith){
    struct type_node *node = make_type_node(SCALAR_TYPE);
    node->scalar.arith_type = arith;
    node->next_type = NULL;
    struct top_tail * tt = make_tt_node();
    tt->top = node; 
    tt->tail = node;  
    return tt; 
}

struct top_tail * create_pointer_node(){
    // Make new type node
    struct type_node * tmp = make_type_node(POINTER_TYPE);

    // Create a top_tail struct that points top and tail towards it
    struct top_tail * tt = make_tt_node(); 
    tt->top = tmp; 
    tt->tail = tmp; 
    return tt; 
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