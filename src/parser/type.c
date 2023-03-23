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
            printf("SCALAR TYPE NODE: %s\n", print_datatype(head->arith_type));
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
            // print_type(head->next_type, depth+1);
            // Print return type
            n_tabs(depth+1);
            printf("RETURN TYPE:\n");
            print_type(head->func_node.return_type, depth+1);

            // Also print parameter list
            break;
        }
        default: {
            fprintf(stderr, "Unknown type node encountered...%d\n", head->type);
            exit(2);
        }
    }
}
        


struct type_node * make_type_node(enum Type type){
    struct type_node *node = (struct type_node *)malloc(sizeof(struct type_node));
    node->type = type;
    return node;
}


struct type_node * create_scalar_node(enum num_type arith){
    struct type_node *node = make_type_node(SCALAR_TYPE);
    node->arith_type = arith;
    node->next_type = NULL;
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