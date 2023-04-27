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
        default:            {fprintf(stderr, "Unsupported (currently) operator...%d\n", operator); exit(2);}
    }
}

// prints parameters of a quad
void print_generic_node(struct generic_node * node, int assign){
    if (assign){
        switch(node->type){
            case (CONSTANT):        {
                                        if (node->num.type > 5) printf("%-9Lg", node->num.frac);
                                        else printf("%-9lld", node->num.integer);
                                        break;
                                    }
            case (VARIABLE):        {printf("%-9s", node->var.name); break;}
            case (TEMPORARY):       {printf("%-9s", node->temp.ident); break;}
            case (STRING_LITERAL):  {printf("%s", node->str.content); break;}
            case (CHAR_LITERAL):    {printf("'%c'", node->char_lit); break;}
            default:                {fprintf(stderr, "Unrecognized generic node type %d...\n", node->type);}
        }
    }
    else {
        switch(node->type){
            case (CONSTANT):        {
                                        if (node->num.type > 5) printf("%Lg", node->num.frac);
                                        else printf("%lld", node->num.integer);
                                        break;
                                    }
            case (VARIABLE):        {printf("%s", node->var.name); break;}
            case (TEMPORARY):       {printf("%s", node->temp.ident); break;}
            case (STRING_LITERAL):  {printf("%s", node->str.content); break;}
            case (CHAR_LITERAL):    {printf("'%c'", node->char_lit); break;}
            default:                {fprintf(stderr, "Unrecognized generic node type %d...\n", node->type);}
        }
    }
}

void print_op_code(enum quad_opcode opcode){
    switch(opcode){
        case ADD:           {printf("%-11s", "ADD"); break;}
        case SUB:           {printf("%-11s", "SUB"); break;}
        case MUL:           {printf("%-11s", "MUL"); break;}
        case DIV:           {printf("%-11s", "DIV"); break;}
        case LOAD:          {printf("%-11s", "LOAD"); break;}
        case STORE:         {printf("%-11s", "STORE"); break;}
        case LEA:           {printf("%-11s", "LEA"); break;}
        case MOV:           {printf("%-11s", "MOV"); break;}
        case CALL:          {printf("%-11s", "CALL"); break;}
        case ARG:           {printf("%-11s", "ARG"); break;}
        case ARGBEGIN:      {printf("%-11s", "ARGBEGIN"); break;}
        case BR:            {printf("%-11s", "BR"); break;}
        case RETURN_QUAD:        {printf("%-11s", "RETURN"); break;}
        default:            {fprintf(stderr, "Unsupported op code %d\n", opcode);}
    }

}

// Helper to print any given quad
void print_quad(struct quad * q){
    printf("\t");
    // If target is present
    if (q->result){
        print_generic_node(q->result, 1);
        printf("%-5s", "= ");
    }
    else{
        printf("%-14s", " ");
    }
    print_op_code(q->opcode);
    printf(" ");
    print_generic_node(q->src1, 0);

    // If theres a second src
    if (q->src2){
        printf(", ");
        print_generic_node(q->src2, 0);
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
    else if (node->type==NUM){
        struct generic_node * constant = make_generic_node(CONSTANT);
        constant->num = node->num;
        return constant;
    }
    else if (node->type==CHAR_LIT){
        struct generic_node * char_lit = make_generic_node(CHAR_LITERAL);
        char_lit->char_lit = node->char_lit; 
        return char_lit;
    }
    else if (node->type==STR_LIT){
        struct generic_node * str_lit = make_generic_node(STRING_LITERAL);
        str_lit->str = node->str_lit;
        return str_lit;  
    }
    else if (node->type==BINARY_NODE && !node->binary.abstract)
        return gen_binary_node(node, target);
    else if (node->type==UNARY_NODE && !node->unary.abstract)
        return gen_unary_node(node, target);
    else if (node->type == FN_CALL)
        return gen_fn_call(node, target);
    else if (node->type == JUMP_NODE){
        if (node->jump.jump_type == RETURN_JUMP){
            struct generic_node * ret_value = gen_rvalue(node->jump.expr, NULL);
            emit(RETURN_QUAD, ret_value, NULL, NULL);
        }
    }
    else if (node->type == IF_STMT){
        gen_if(node);
    }
    return target; 
}

// Generate binary node quads
struct generic_node * gen_binary_node(struct astnode * node, struct generic_node * target){
    // Assignment binary op
    if (node->binary.operator_type == ASSIGNMENT){
        gen_assign(node);
        return target; 
    }
    else if (node->binary.operator_type == ASSIGNMENT_COMPOUND){
        // TODO: this is actually the same as a++ and ++a right now... fix later
        // Create a temporary binary node with the compounded operation
        struct astnode * tmp_binary = make_ast_node(BINARY_NODE); 
        tmp_binary->binary.operator_type = BINOP;
        tmp_binary->binary.left = node->binary.left;
        tmp_binary->binary.right = node->binary.right;

        // Get the proper operator (replace *= with *)
        switch (node->binary.operator){
            case TIMESEQ:       {tmp_binary->binary.operator = '*'; break;}
            case DIVEQ:         {tmp_binary->binary.operator = '/'; break;}
            case PLUSEQ:        {tmp_binary->binary.operator = '+'; break;}
            case MINUSEQ:       {tmp_binary->binary.operator = '-'; break;}
        }

        // Create temporary assignment node
        struct astnode * tmp_assign = make_ast_node(BINARY_NODE);
        tmp_assign->binary.operator_type = ASSIGNMENT;
        tmp_assign->binary.operator = '=';
        tmp_assign->binary.left = node->binary.left;
        tmp_assign->binary.right = tmp_binary;

        return gen_assign(tmp_assign);
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
            fprintf(stderr, "%serror%s: Invalid operation on %s:%d. Can only subtract 2 pointers!\n", RED, RESET, node->file_name, node->line_num);
            exit(2);
        }

        struct top_tail * tt = create_scalar_node(I);
        tmp = tt->top;
    }
    else if (left->n_p == NUM_VAR && right->n_p == POINTER_VAR){        // Num +- pointer
        if (get_opcode(node->binary.operator) != SUB && get_opcode(node->binary.operator) != ADD) {
            fprintf(stderr, "%serror%s: Invalid operation on %s:%d. Can only add or subtract a pointer and a number!\n", RED, RESET, node->file_name, node->line_num);
            exit(2);
        }
        left = make_tmp_type(left, right_type, MUL);
        tmp = right_type; 
        
        
    }
    else if (left->n_p == POINTER_VAR && right->n_p == NUM_VAR){        // Pointer += num
        if (get_opcode(node->binary.operator) != SUB && get_opcode(node->binary.operator) != ADD) {
            fprintf(stderr, "%serror%s: Invalid operation on %s:%d. Can only add or subtract a pointer and a number!\n", RED, RESET, node->file_name, node->line_num);
            exit(2);
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


// Generate quads for unary nodes
struct generic_node * gen_unary_node(struct astnode * node, struct generic_node * target){
    if (node->unary.operator_type == DEREF){
        // if (node->unary.expr->ident.sym && node->unary.expr->ident.sym->type->type == ARRAY_TYPE)
        //     return gen_rvalue(node->unary.expr, NULL);
        struct generic_node * addr = gen_rvalue(node->unary.expr, NULL);

        // Check if the returned type is a pointer or not
        struct type_node * addr_type = get_type_from_generic(addr);
        addr->n_p = determine_if_pointer(addr_type);

        if (addr->n_p != POINTER_VAR){
            fprintf(stderr, "%serror%s: On %s:%d: Cannot dereference given type!\n", RED, RESET, node->file_name, node->line_num);
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
    else if (node->unary.operator_type == SIZEOF_OP){
        struct generic_node * operand = gen_rvalue(node->unary.expr, NULL);
        struct generic_node * constant = make_generic_node(CONSTANT);
        struct type_node * tt;

        if (operand->type != TEMPORARY && operand->type != VARIABLE)
            fprintf(stderr, "%serror%s: Invalid use of sizeof on %s:%d", RED, RESET, node->file_name, node->line_num);

        tt = get_type_from_generic(operand);
        constant->num.integer = size_of(tt);
        constant->num.type = I;

        if (!target)
            target = new_temporary(tt);

        emit(MOV, constant, NULL, target);

        return target; 
    }
    else if (node->unary.operator == '&'){
        int mode; 
        struct generic_node * addr = gen_lvalue(node->unary.expr, &mode);
        
        // Update type node -> now becomes pointer to prev type
        struct type_node * t = get_type_from_generic(addr); 
        struct type_node * pointer = make_type_node(POINTER_TYPE);
        pointer->next_type = t; 
        if (!target)
            target = new_temporary(pointer);
        emit(LEA, addr, NULL, target);
    }
    else if (node->unary.operator == PLUSPLUS || node->unary.operator == MINUSMINUS){
        struct generic_node * src1 = gen_rvalue(node->unary.expr, NULL);
        struct generic_node * constant = make_generic_node(CONSTANT);
        constant->num.integer = 1; 

        enum quad_opcode tmp;
        if (node->unary.operator == PLUSPLUS)   tmp = ADD; 
        else                                    tmp = SUB;
        emit(tmp, src1, constant, src1);
        return src1; 
    }
    return target; 
}


// Generate quads for function calls
struct generic_node * gen_fn_call(struct astnode * node, struct generic_node * target){
    // Start function call quads
    struct generic_node * constant = make_generic_node(CONSTANT);
    constant->num.integer = node->fncall.head->num_args;
    emit(ARGBEGIN, constant, NULL, NULL);


    // Generate quad for each argument
    // check for syntax errors, throw an error if types don't match 
    struct linked_list * tmp = node->fncall.head; 
    struct generic_node * arg; 
    struct generic_node * arg_num;
    struct type_node * arg_type; 
    int arg_idx = 1;
    int defined_args = 0; 
    
    // Get function node
    struct generic_node * fn_ident = gen_rvalue(node->fncall.postfix, NULL);
    struct type_node * function_def = get_type_from_generic(fn_ident);
    struct astnode_symbol * tmp_args = function_def->func_node.param_head; 
    while(tmp){
        arg = gen_rvalue(tmp->expr, NULL);
        arg_num = make_generic_node(CONSTANT);
        arg_type = get_type_from_generic(arg);
        arg_num->num.integer = arg_idx; 
        if (tmp_args && function_def->func_node.param_head && !check_types(tmp_args->type, arg_type)){
            fprintf(stderr, "%serror%s: Type mismatch in %s:%d. Type of given in function call doesn't match the definition for argument %d\n", RED, RESET, node->file_name, node->line_num, arg_idx);
            exit(2);
        }

        emit(ARG, arg_num, arg, NULL);
        arg_idx++; tmp = tmp->next;

        if (tmp_args){
            tmp_args = tmp_args->next;
            defined_args++; 
        }
    }
    
    if (defined_args != node->fncall.head->num_args && function_def->func_node.param_head){
        fprintf(stderr, "%serror%s: Function call on %s:%d expected %d argument(s), received %d\n", RED, RESET, node->file_name, node->line_num, defined_args, node->fncall.head->num_args);
        exit(2);
    }
    struct type_node * tt = get_type_from_generic(fn_ident);

    if (!target)
        target = new_temporary(tt);

    // Call quad
    emit(CALL, fn_ident, NULL, target);

    // End basic block
    create_basic_block();
    return target;
}


// based off notes, page 25
void gen_if(struct astnode * if_node){
    if (if_node->if_stmt.cond_type == SWITCH_NODE){
        fprintf(stderr, "%serror%s: Use of switch statements in %s:%d unsupported", RED, RESET, if_node->file_name, if_node->line_num);
        exit(2);
    }

    // Save current block 
    struct basic_block * start_block = curr_block; 

    // True / False branch
    struct basic_block *Bt, *Bf, *Bn; 
    Bt = create_basic_block();              // Updates curr_block
    Bf = create_basic_block();

    // Else
    if (if_node->if_stmt.else_stmt)
        Bn = create_basic_block();
    else
        Bn=Bf;
    

    gen_condexpr(if_node, Bt, Bf); //creates branches to Bt,Bf cur_bb=Bt;

    curr_block = Bt; 
    gen_stmt(if_node->if_stmt.stmt);
    link_bb(start_block, Bn);
    if (if_node->if_stmt.else_stmt){
        curr_block = Bf;
        gen_stmt(if_node->if_stmt.else_stmt);
        link_bb(curr_block, Bn);
    }
    curr_block = Bn;
}

// Generate quads for a given statement / astnode
void gen_stmt(struct astnode * asthead){
    if (asthead->type == COMPOUND){
        struct linked_list * ll = asthead->ds_list; 
        while(ll != NULL){
            gen_rvalue(ll->expr, NULL);
            ll = ll->next;
        }
    } 
    else {
        gen_rvalue(asthead, NULL);
    }
}

void gen_condexpr(){

}

void link_bb(struct basic_block * block1, struct basic_block * block2){
    // Update the branch at the end of the basic block to be the next in sequence
    // block1->tail->src1->str.content = block2->label; 
}

// Return the type node of a given generic node used in a quad
// Assumes generic node is either a variable or temporary register
struct type_node * get_type_from_generic(struct generic_node * node){
    struct type_node * tt; 
    if (node->type == TEMPORARY)
        tt = node->temp.operation_type; 
    else if (node->type == VARIABLE)
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
    if (node->type==NUM || node->type==STR_LIT || node->type == CHAR_LIT) return NULL;
    if (node->type == UNARY_NODE){
        // If dereference, save the mode as an indirect reference
        if (node->unary.operator_type == DEREF){
            *mode=INDIRECT;
            if (!node->unary.abstract)
                return gen_rvalue(node->unary.expr, NULL);
        }
    }
    return NULL; 
}

// Generate assignment, pseudocode on page 12
struct generic_node * gen_assign(struct astnode * node){
    int dstmode; 
    struct generic_node * dst = gen_lvalue(node->binary.left, &dstmode);
    if (dst==NULL){
        fprintf(stderr, "%serror%s: invalid LHS of assignment on %s:%d\n", RED, RESET, node->file_name, node->line_num); 
        exit(2);
    }
    if (dstmode==DIRECT){
        struct generic_node * src = gen_rvalue(node->binary.right, dst);

        if (!check_types(get_type_from_generic(src), get_type_from_generic(dst))){
            fprintf(stderr, "%serror%s: Type mismatch in assignment in %s:%d\n", RED, RESET, node->file_name, node->line_num);
            exit(2);
        }

        if (src != dst)
            emit(MOV, src, NULL, dst);
    }
    else {
        struct generic_node *t1 = gen_rvalue(node->binary.right, NULL);
        emit(STORE, t1, dst, NULL);
    }
    return dst; 
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

    // Following BB convention of .BB(function_counter).(basic_block_counter)
    asprintf(&block->label, ".BB%d.%d", func_counter, bb_counter++);

    // Append to linked list of blocks, update current block
    if (curr_block){
        curr_block->next_block = block;

        // End last basic block with a branch...
        struct generic_node * branch_num = make_generic_node(STRING_LITERAL);
        branch_num->str.content = block->label; 
        emit(BR, branch_num, NULL, NULL);
        curr_block = block;
    }
    else{
        curr_block = block; 
        block_head = block;
    }
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
void gen_quads(struct astnode * asthead, char * func_name){
    // Reset basic block counter and blocks
    bb_counter = 0;
    block_head = NULL; 
    curr_block = NULL; 

    // Print basic block 
    printf(".%s\n", func_name);
    create_basic_block();
    gen_stmt(asthead);

    // Display all basic blocks + their quads...
    struct basic_block * tmp = block_head;
    while(tmp){
        print_basic_block(tmp);
        tmp = tmp->next_block;
    }
    func_counter++;
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