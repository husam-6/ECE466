#include "quads.h"
#include "parser.h"
#include "parser.tab.h"
#include "stdio.h"
#include "stdlib.h"

// operator helper function
enum quad_opcode get_opcode(int operator) {
    switch(operator){
        case '+':           {return ADD; break;}
        case '-':           {return SUB; break;}
        case '*':           {return MUL; break;}
        case '/':           {return DIV; break;}
        case PLUSPLUS:      {return ADD; break;}
        case MINUSMINUS:    {return SUB; break;}
        default:            {fprintf(stderr, "Unsupported (currently) operator...%d\n", operator); exit(2);}
    }
}

// prints parameters of a quad
void print_generic_node(struct generic_node * node){
    switch(node->type){
        case (CONSTANT):        {
                                    if (node->num.type > 5) printf("%-9Lg", node->num.frac);
                                    else printf("%-9lld", node->num.integer);
                                    break;
                                }
        case (VARIABLE):        {printf("%-9s", node->var.name); break;}
        case (TEMPORARY):       {printf("%-9s", node->temp.ident); break;}
        case (STRING_LIT):      {printf("%-9s", node->str.content); break;}
        default:                {fprintf(stderr, "Unrecognized generic node type %d...\n", node->type);}
    }
}

void print_op_code(enum quad_opcode opcode){
    switch(opcode){
        case ADD:           {printf("%-9s", "ADD"); break;}
        case SUB:           {printf("%-9s", "SUB"); break;}
        case MUL:           {printf("%-9s", "MUL"); break;}
        case DIV:           {printf("%-9s", "DIV"); break;}
        case LOAD:          {printf("%-9s", "LOAD"); break;}
        case STORE:         {printf("%-9s", "STORE"); break;}
        case LEA:           {printf("%-9s", "LEA"); break;}
        case MOV:           {printf("%-9s", "MOV"); break;}
        default:            {fprintf(stderr, "Unsupported op code %d\n", opcode);}
    }

}

// Helper to print any given quad
void print_quad(struct quad * q){
    printf("\t");
    // If target is present
    if (q->result){
        print_generic_node(q->result);
        printf("%-5s", "= ");
    }
    else{
        printf("%-14s", " ");
    }
    print_op_code(q->opcode);
    printf(" ");
    print_generic_node(q->src1);

    // If theres a second src
    if (q->src2){
        printf(", ");
        print_generic_node(q->src2);
    }
    printf("\n");
}

// Generate r value of a given node
// Code from hak lecture notes 5, starting page 10
struct generic_node * gen_rvalue(struct astnode * node, struct generic_node * target){
    if (node->type==IDENT_NODE){
        struct generic_node * ident = make_generic_node(VARIABLE);
        ident->var = node->ident;
        if (node->ident.sym->type->type == ARRAY_TYPE){
            // Cast array to pointer
            struct type_node * point =  make_type_node(POINTER_TYPE);
            point->next_type = node->ident.sym->type->next_type;
            struct generic_node * temp = new_temporary(point);

            emit(LEA, ident, NULL, temp);
            return temp; 
        }
        return ident;
    }
    if (node->type==NUM){
        struct generic_node * constant = make_generic_node(CONSTANT);
        constant->num = node->num;
        return constant;
    }
    if (node->type==BINARY_NODE && !node->binary.abstract){
        // Assignment binary op
        if (node->binary.operator_type == ASSIGNMENT){
            gen_assign(node);
            return target; 
        }

        // Ordinary binary operator case, ignoring type conversions
        struct generic_node * left = gen_rvalue(node->binary.left, NULL);
        struct generic_node * right = gen_rvalue(node->binary.right, NULL);

        left->n_p = 0; 
        right->n_p = 0; 

        // Pointer arithmetic
        struct type_node * left_type = get_type_from_generic(left);
        struct type_node * right_type = get_type_from_generic(right);

        left->n_p = determine_if_pointer(left_type);
        right->n_p = determine_if_pointer(right_type);


        // Determine the overall operation
        struct type_node * tmp; 
        if (left->n_p == POINTER_VAR && right->n_p == POINTER_VAR){     // Pointer - pointer
            if (get_opcode(node->binary.operator) != SUB) {
                fprintf(stderr, "ERROR: Invalid operation on %s:%d. Can only subtract 2 pointers!\n", node->file_name, node->line_num);
                exit(2);
            }

            struct top_tail * tt = create_scalar_node(I);
            tmp = tt->top;
        }
        else if (left->n_p == NUM_VAR && right->n_p == POINTER_VAR){        // Num +- pointer
            if (get_opcode(node->binary.operator) != SUB && get_opcode(node->binary.operator) != ADD) {
                fprintf(stderr, "ERROR: Invalid operation on %s:%d. Can only add or subtract a pointer and a number!\n", node->file_name, node->line_num);
                exit(2);
            }
            left = make_tmp_type(left, right_type, MUL);
            tmp = right_type; 
            
            
        }
        else if (left->n_p == POINTER_VAR && right->n_p == NUM_VAR){        // Pointer += num
            if (get_opcode(node->binary.operator) != SUB && get_opcode(node->binary.operator) != ADD) {
                fprintf(stderr, "ERROR: Invalid operation on %s:%d. Can only add or subtract a pointer and a number!\n", node->file_name, node->line_num);
            }
            right = make_tmp_type(right, left_type, MUL);
            tmp = left_type; 
        }
        else{
            // Assume everything is an int
            struct top_tail * tt = create_scalar_node(I);
            tmp = tt->top;
        }

        if (!target)
            target = new_temporary(tmp);
        emit(get_opcode(node->binary.operator), left, right, target);

        if (left->n_p == POINTER_VAR && right->n_p == POINTER_VAR)
            make_tmp_type(target, tmp, DIV);

        return target;
    }

    if (node->type==UNARY_NODE && !node->unary.abstract){
        if (node->unary.operator_type == DEREF){
            // if (node->unary.expr->ident.sym && node->unary.expr->ident.sym->type->type == ARRAY_TYPE)
            //     return gen_rvalue(node->unary.expr, NULL);
            struct generic_node * addr = gen_rvalue(node->unary.expr, NULL);

            // Check if the returned type is a pointer or not
            struct type_node * addr_type = get_type_from_generic(addr);
            addr->n_p = determine_if_pointer(addr_type);

            if (addr->n_p != POINTER_VAR){
                fprintf(stderr, "ERROR: On %s:%d: Cannot dereference given type!\n", node->file_name, node->line_num);
                exit(2);
            }
            
            if (!target){
                if (addr->type == TEMPORARY){
                    // If the next type is also an array, cast that to a pointer as well!
                    struct type_node * deref_type = addr->temp.operation_type->next_type; 
                    if (deref_type->type == ARRAY_TYPE){
                        struct type_node * point =  make_type_node(POINTER_TYPE);
                        point->next_type = deref_type->next_type; 
                        
                        // 'Dereference' type - remove top level type
                        addr->temp.operation_type = point;
                        return addr;
                    }
                    else
                        target = new_temporary(deref_type);       // 'Dereference' type - remove top level type
                }
                else if (addr->type == VARIABLE)
                    target = new_temporary(addr->var.sym->type->next_type);
            }
            emit(LOAD, addr, NULL, target);
        }
        if (node->unary.operator_type == SIZEOF_OP){
            struct generic_node * operand = gen_rvalue(node->unary.expr, NULL);
            struct generic_node * constant = make_generic_node(CONSTANT);
            struct type_node * tt;

            if (operand->type != TEMPORARY && operand->type != VARIABLE)
                fprintf(stderr, "ERROR: Invalid use of sizeof on %s:%d", node->file_name, node->line_num);

            tt = get_type_from_generic(operand);
            constant->num.integer = size_of(tt);
            constant->num.type = I;

            if (!target)
                target = new_temporary(tt);

            emit(MOV, constant, NULL, target);

            return target; 
        }
    }
    return target; 
}


// Return the type node of a given generic node used in a quad
// Assumes generic node is either a variable or temporary register
struct type_node * get_type_from_generic(struct generic_node * node){
    struct type_node * tt; 
    if (node->type == TEMPORARY)
        tt = node->temp.operation_type; 
    if (node->type == VARIABLE)
        tt = node->var.sym->type;
    else{
        // Assume int otherwise
        tt = make_type_node(SCALAR_TYPE); 
        tt->scalar.arith_type = I;
    }
    return tt; 
}

// Intermediate quad for pointer arithmetic 
struct generic_node * make_tmp_type(struct generic_node * node, struct type_node * t, enum quad_opcode opcode){

    // Create a constant node and new target
    struct generic_node * constant = make_generic_node(CONSTANT);
    struct generic_node * target;

    constant->num.integer = size_of(t);
    constant->num.type = I;
    target = new_temporary(t);

    emit(opcode, node, constant, target);
    return target;
}


// Function returns if a given type is a pointer / array or not
int determine_if_pointer(struct type_node * tt){
    if (tt->type == POINTER_TYPE || tt->type == ARRAY_TYPE)
        return POINTER_VAR;                
    return NUM_VAR;
}

// More code from hak, generate the lvalue of a given astnode
struct generic_node * gen_lvalue(struct astnode * node, int * mode){
    if (node->type==IDENT_NODE){
        *mode=DIRECT;
        struct generic_node * ident = make_generic_node(VARIABLE);
        ident->var = node->ident;
        return ident;
    }
    if (node->type==NUM) return NULL;
    if (node->type == UNARY_NODE){
        // If dereference, save the mode as an indirect reference
        if (node->unary.operator_type == DEREF){
            *mode=INDIRECT;
            if (!node->unary.abstract)
                return gen_rvalue(node->unary.expr, NULL);
        }
    }
}

// Generate assignment, pseudocode on page 12
struct generic_node * gen_assign(struct astnode * node){
    int dstmode; 
    struct generic_node * dst = gen_lvalue(node->binary.left, &dstmode);
    if (dst==NULL)
        fprintf(stderr, "Error, invalid LHS of assignment on %s:%d\n", node->file_name, node->line_num); 
    if (dstmode==DIRECT)
        gen_rvalue(node->binary.right, dst);
    else {
        struct generic_node *t1 = gen_rvalue(node->binary.right, NULL);
        emit(STORE, t1, dst, NULL);
    }
}


// Emit a quad and append to linked list of quads
void emit(enum quad_opcode opcode, struct generic_node * src1, struct generic_node * src2, struct generic_node * dest){
    struct quad * q = create_quad();

    // Initialize quad parameters
    q->opcode = opcode;
    q->src1 = src1; 
    q->src2 = src2;
    q->result = dest;

    // Append to end quad linked list
    if (!curr_block->head){
        // Initialize head and tail pointers
        curr_block->head = q;
        curr_block->tail = q; 
    }
    else{
        // Append to end of linked list
        curr_block->tail->next_quad = q; 
        curr_block->tail = q;
    }
    // block_head->head = q;
    // print_quad(q);
}

// Helper function to create a new temporary node / register
struct generic_node * new_temporary(struct type_node * t){
    struct generic_node * node = make_generic_node(TEMPORARY);
    node->temp.reg_num = register_counter++;
    asprintf(&node->temp.ident, "%%T%06d", node->temp.reg_num);
    node->temp.operation_type = t; 
    return node;
}

// Allocator for quads
struct quad * create_quad(){
    struct quad *q = (struct quad *)malloc(sizeof(struct quad));
    q->src1 = NULL;
    q->src2 = NULL;
    q->result = NULL;
    return q; 
}

// Helper to allocate memory for a generic node
struct generic_node * make_generic_node(enum generic_type type){
    struct generic_node * node = (struct generic_node *)malloc(sizeof(struct generic_node));
    node->type = type;
    return node;
}

// Allocate basic block
struct basic_block * create_basic_block(){
    struct basic_block * block = (struct basic_block *)malloc(sizeof(struct basic_block));
    
    // Initialize name and children
    block->next_block = NULL;
    block->head = NULL;

    asprintf(&block->label, ".BB%d.%d", func_counter++, bb_counter++);
    curr_block = block; 
    return block;

}

// Print all the quads and information in a basic block
void print_basic_block(struct basic_block * bb){
    printf("%s\n", bb->label);
    struct quad * tmp = bb->head;
    while (tmp != NULL){
        print_quad(tmp);
        tmp = tmp->next_quad;
    }
}

// Loop through list of ASTs and generate quads
void gen_quads(struct linked_list * asthead, char * func_name){
    // printf("#####\t\t Generating Quads \t\t #####\n");
    printf(".%s\n", func_name);
    block_head = create_basic_block();
    while(asthead != NULL){
        gen_rvalue(asthead->expr, NULL);
        asthead = asthead->next;
    }

    // Display all basic blocks + their quads...
    struct basic_block * tmp = block_head;
    while(tmp){
        print_basic_block(tmp);
        tmp = tmp->next_block;
    }
    // printf("#####\t\t End of Quads \t\t #####\n");
}

// Returns the size of a given type (as an integer)
int size_of(struct type_node * type){
    int size = 1; 
    while(type){
        if (type->type == ARRAY_TYPE)
            size *= type->size;
        else if (type->type == SCALAR_TYPE) {
            if (type->scalar.arith_type == SH)
                size *= 2;
            if (type->scalar.arith_type == I)
                size *= 4;
        } 
        type = type->next_type;
    }
    return size; 
}