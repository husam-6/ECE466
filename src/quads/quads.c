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
        default:            {fprintf(stderr, "Unsupported (currently) operator...%d\n", operator); die("bruh?\n"); exit(2);}
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
            case (STRING_LITERAL):  {
                                        printf("\"");
                                        for (int i = 0; i < node->str.length; i++){
                                            char * tmp = to_char(node->str.content[i]);
                                            printf("%s", tmp);
                                        }
                                        printf("\"");
                                        break;
                                    }
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
            case (STRING_LITERAL):  {
                                        printf("\"");
                                        for (int i = 0; i < node->str.length; i++){
                                            char * tmp = to_char(node->str.content[i]);
                                            printf("%s", tmp);
                                        }
                                        printf("\"");
                                        break;
                                    }
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
        case CMP:           {printf("%-11s", "CMP"); break;}
        case RETURN_QUAD:   {printf("%-11s", "RETURN"); break;}
        case POSTINC:       {printf("%-11s", "POSTINC"); break;}
        case POSTDEC:       {printf("%-11s", "POSTDEC"); break;}
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

// Print all the quads and information in a basic block
void print_basic_block(struct basic_block * bb){
    printf("%s\n", bb->label);
    struct quad * tmp = bb->head;
    while (tmp != NULL){
        print_quad(tmp);
        tmp = tmp->next_quad;
    }
    
    // Branch
    if (bb && bb->branch && bb->tail && bb->tail->opcode != RETURN_QUAD){     // Don't branch if return (or do differently than other blocks)
        printf("\t%-14s", " ");
        switch(bb->branch){
            case BR:        {printf("%-11s %s\n", "BR", bb->left->label); break;}
            case BREQ:      {printf("%-11s %s, %s\n", "BREQ", bb->left->label, bb->right->label); break;}
            case BRNEQ:     {printf("%-11s %s, %s\n", "BRNEQ", bb->left->label, bb->right->label); break;}
            case BRGT:      {printf("%-11s %s, %s\n", "BRGT", bb->left->label, bb->right->label); break;}
            case BRLT:      {printf("%-11s %s, %s\n", "BRLT", bb->left->label, bb->right->label); break;}
            case BRGEQ:     {printf("%-11s %s, %s\n", "BRGEQ", bb->left->label, bb->right->label); break;}
            case BRLEQ:     {printf("%-11s %s, %s\n", "BRLEQ", bb->left->label, bb->right->label); break;}
            default:        {fprintf(stderr, "%serror%s: UNKNOWN BRANCH TYPE %d\n", RED, RESET, bb->branch); break;}
        }
    }
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
    else if (node->type == FOR_LOOP){
        gen_for_loop(node);
    }
    else if (node->type == WHILE_LOOP){
        gen_while_loop(node);
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
    else if (node->binary.operator_type == COMP_OP){        // Treat like an if statement
        // Save current block 
        struct basic_block * start_block = curr_block; 

        // True / False branch
        struct basic_block *Bt, *Bf, *Bn; 
        Bt = create_basic_block();              // Updates curr_block
        Bf = create_basic_block();
        Bn = create_basic_block();
        curr_block = start_block;

        gen_condexpr(node, Bt, Bf); //creates branches to Bt,Bf cur_bb=Bt;

        struct type_node * int_type = create_scalar_node(I)->top;
        if (!target)
            target = new_temporary(int_type);

        struct generic_node * constant = make_generic_node(CONSTANT);
        constant->num.integer = 1; 

        // True => value should be a 1
        curr_block = Bt; 
        emit(MOV, constant, NULL, target);
        link_bb(curr_block, BR, Bn, NULL);

        // 0 otherwise
        curr_block = Bf;
        struct generic_node * zero = make_generic_node(CONSTANT);
        zero->num.integer = 0; 
        emit(MOV, zero, NULL, target);
        link_bb(curr_block, BR, Bn, NULL);
        
        curr_block = Bn;

        return target;
    }
    else if (node->binary.operator_type == LOGICAL_OP && (node->binary.operator == LOGAND || node->binary.operator == LOGOR)){
        struct basic_block *Bt, *Bf, *Bn, *start_block;
        start_block = curr_block;
        Bt = create_basic_block();
        Bf = create_basic_block();
        Bn = create_basic_block();

        curr_block = start_block; 

        gen_condexpr(node, Bt, Bf);
        link_bb(Bt, BR, Bn, NULL);
        link_bb(Bf, BR, Bn, NULL);
        
        struct type_node * t = create_scalar_node(I)->top;
        if (!target)
            target = new_temporary(t);

        struct generic_node *true_eval, *false_eval; 
        true_eval = make_generic_node(CONSTANT);
        true_eval->num.integer = 1;
        false_eval = make_generic_node(CONSTANT);
        false_eval->num.integer = 0;

        // 1 if true
        curr_block = Bt;
        emit(MOV, true_eval, NULL, target);
        
        // 0 if false
        curr_block = Bf;
        emit(MOV, false_eval, NULL, target);

        curr_block = Bn;

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
    else if (node->unary.operator == PLUSPLUS || node->unary.operator == MINUSMINUS){       // if parsed correctly -> this is a preinc
        struct generic_node * src1 = gen_rvalue(node->unary.expr, NULL);
        struct generic_node * constant = make_generic_node(CONSTANT);
        constant->num.integer = 1; 

        enum quad_opcode tmp;
        if (node->unary.operator == PLUSPLUS)   tmp = POSTINC; 
        else                                    tmp = POSTDEC;

        if (!target)
            target = new_temporary(get_type_from_generic(src1));
        emit(tmp, src1, NULL, target);
        return src1; 
    }
    else if (node->unary.operator == '-'){
        struct generic_node * src1 = gen_rvalue(node->unary.expr, NULL);
        struct generic_node * constant = make_generic_node(CONSTANT);
        constant->num.integer = -1;

        if (!target)
            target = new_temporary(get_type_from_generic(constant));

        emit(MUL, src1, constant, target);
        return target;  
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
    while(tmp_args){
        tmp_args = tmp_args->next;
        defined_args++; 
    }

    tmp_args = function_def->func_node.param_head;
    while(tmp && tmp->expr){
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
        
        if (tmp_args)
            tmp_args = tmp_args->next;
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
    struct basic_block *last = curr_block;
    create_basic_block();
    link_bb(last, BR, curr_block, NULL);
    return target;
}


// based off notes, page 25
void gen_if(struct astnode * if_node){
    if (if_node->if_stmt.cond_type == SWITCH_NODE){
        fprintf(stderr, "%serror%s: Use of switch statements in %s:%d unsupported\n", RED, RESET, if_node->file_name, if_node->line_num);
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


    curr_block = start_block; 
    gen_condexpr(if_node->if_stmt.cond, Bt, Bf); //creates branches to Bt,Bf cur_bb=Bt;

    // Generate statements of true block
    curr_block = Bt;   
    gen_stmt(if_node->if_stmt.stmt);
    curr_block = Bt;   
    link_bb(curr_block, BR, Bn, NULL);

    if (if_node->if_stmt.else_stmt){
        curr_block = Bf;
        gen_stmt(if_node->if_stmt.else_stmt);
        curr_block = Bf;
        link_bb(curr_block, BR, Bn, NULL);
    }
    curr_block = Bn;
}


void gen_for_loop(struct astnode * node){
    // Initialization
    struct basic_block *start_loop, *init, *Bbreak, *Btmp; 
    gen_stmt(node->for_loop.init);
    init = curr_block;

    // New block for body of for loop
    curr_block = create_basic_block();
    start_loop = curr_block;
    gen_stmt(node->for_loop.body);

    // New block for increment
    curr_block = create_basic_block();
    gen_stmt(node->for_loop.inc);

    // New block for comparison
    curr_block = create_basic_block();
    link_bb(init, BR, curr_block, NULL);
    // dump_basic_blocks(block_head);
    Btmp = curr_block;
    Bbreak = create_basic_block();
    curr_block = Btmp;
    gen_condexpr(node->for_loop.cond, start_loop, Bbreak);
    curr_block = Bbreak; 
}

void gen_while_loop(struct astnode * node){
    struct basic_block *Bbody, *Bbreak, *Bstart;
    Bstart = create_basic_block(); 
    if (!node->while_loop.do_while){
        // Create blocks for the loop and at the end of the iterations
        Bbody = create_basic_block();
        Bbreak = create_basic_block();

        // Generate conditional statement
        curr_block = Bstart;
        gen_condexpr(node->while_loop.cond, Bbody, Bbreak);

        // Body 
        curr_block = Bbody;
        gen_stmt(node->while_loop.stmt);
        link_bb(Bbody, BR, Bstart, NULL);
        curr_block = Bbreak; 
        return; 
    }

    // Generate body first
    curr_block = Bstart;
    gen_stmt(node->while_loop.stmt);

    // new block for comparisons and then after all iteratons
    Bbody = create_basic_block();
    Bbreak = create_basic_block();

    // Conditional
    curr_block = Bbody; 
    gen_condexpr(node->while_loop.cond, Bstart, Bbreak);
    curr_block = Bbreak; 
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

// Routine for conditional expressions
struct generic_node * gen_condexpr(struct astnode * node, struct basic_block * Bt, struct basic_block * Bf){
    struct generic_node * constant = make_generic_node(CONSTANT);
    constant->num.integer = 0;
    if (node->type==IDENT_NODE){        // Compare to 0 
        struct generic_node * ident = make_generic_node(VARIABLE);
        ident->var = node->ident;
        emit(CMP, ident, constant, NULL);
        link_bb(curr_block, BRNEQ, Bt, Bf);
        return ident;
    }
    else if (node->type==NUM){
        struct generic_node * number = make_generic_node(CONSTANT);
        number->num = node->num;

        emit(CMP, number, constant, NULL);
        link_bb(curr_block, BRNEQ, Bt, Bf);
        return number;
    }
    else if (node->type==CHAR_LIT){
        struct generic_node * char_lit = make_generic_node(CHAR_LITERAL);
        char_lit->char_lit = node->char_lit; 

        emit(CMP, char_lit, constant, NULL);
        link_bb(curr_block, BRNEQ, Bt, Bf);
        return char_lit;
    }
    else if (node->type==STR_LIT){
        struct generic_node * str_lit = make_generic_node(STRING_LITERAL);
        str_lit->str = node->str_lit;

        emit(CMP, str_lit, constant, NULL);
        link_bb(curr_block, BRNEQ, Bt, Bf);
        return str_lit;  
    }
    else if (node->type==BINARY_NODE && !node->binary.abstract){
        // Compare assignment to 0
        if (node->binary.operator_type == ASSIGNMENT){
            struct generic_node * dst = gen_assign(node);
            emit(CMP, dst, constant, NULL);
            link_bb(curr_block, BRNEQ, Bt, Bf);
            return dst; 
        }
        else if (node->binary.operator_type == LOGICAL_OP && (node->binary.operator == LOGAND || node->binary.operator == LOGOR)){
            // Extra basic block now (technically doing 2 separate comparisons)
            struct basic_block *Btmp, *start_block;
            
            //save curr block, create tmp block for logical operation
            start_block = curr_block; 
            Btmp = create_basic_block(); 
            // Bn = create_basic_block();
            curr_block = start_block;

            if (node->binary.operator == LOGAND){
                // Both expressions must evaluate != 0 to be true
                gen_condexpr(node->binary.left, Btmp, Bf);
                curr_block = Btmp; 
                gen_condexpr(node->binary.right, Bt, Bf);
            }
            else if (node->binary.operator == LOGOR){
                // Only one has to be true
                gen_condexpr(node->binary.left, Bt, Btmp);
                curr_block = Btmp; 
                gen_condexpr(node->binary.right, Bt, Bf);
            }
            return NULL;
        }


        // Compare left and right otherwise
        struct generic_node * left = gen_rvalue(node->binary.left, NULL);
        struct generic_node * right = gen_rvalue(node->binary.right, NULL);

        emit(CMP, left, right, NULL);

        // Invert comparison
        enum branch_type tmp; 
        switch(node->binary.operator){
            case '<':               {tmp = BRGEQ; break;}
            case '>':               {tmp = BRLEQ; break;}
            case EQEQ:              {tmp = BRNEQ; break;}
            case NOTEQ:             {tmp = BREQ; break;}
            case GTEQ:              {tmp = BRLT; break;}
            case LTEQ:              {tmp = BRGT; break;}
            default:                {fprintf(stderr, "%serror%s: Unknown operator in comparison in %s:%d. Operator given: %d\n", RED, RESET, node->file_name, node->line_num, node->binary.operator); die("bruh?\n"); exit(2);}
        }
        
        // Inversion of Bf and Bt
        link_bb(curr_block, tmp, Bf, Bt);
        
        return NULL; 
    }
    else if (node->type==UNARY_NODE && !node->unary.abstract){
        // Get rvalue
        struct generic_node * unary = gen_rvalue(node->unary.expr, NULL);

        // Compare to 0
        emit(CMP, unary, constant, NULL);
        link_bb(curr_block, BRNEQ, Bt, Bf);

        return NULL; 
    }
    else{
        fprintf(stderr, "%serror%s: Invalid type in %s:%d. Type given: %d", RED, RESET, node->file_name, node->line_num, node->type);
        die("Invalid Type?");
    }

}


void link_bb(struct basic_block * og_block, enum branch_type branch, struct basic_block * block1, struct basic_block * block2){
    // Update the branch at the end of the basic block to be the next in sequence
    og_block->branch= branch; 
    og_block->left = block1; 
    og_block->right = block2; 
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
    if (block_tail){
        // Set up branch params
        if (block_tail->branch < 2 || block_tail->branch > 7)       // if we already have a branch command set up don't overwrite
            link_bb(block_tail, BR, block, NULL);
        block_tail->next_block = block;
        block_tail = block;
        curr_block = block; 
    }
    else{
        curr_block = block; 
        block_head = block;
        block_tail = block; 
    }
    return block;

}

// Loop through list of ASTs and generate quads
void gen_quads(struct astnode * asthead, char * func_name){
    // Reset basic block counter and blocks
    bb_counter = 0;
    block_head = NULL; 
    block_tail = NULL;
    curr_block = NULL; 

    // Print basic block 
    printf(".%s\n", func_name);
    create_basic_block();
    gen_stmt(asthead);

    dump_basic_blocks(block_head);
    func_counter++;
    // printf("#####\t\t End of Quads \t\t #####\n");
}

void dump_basic_blocks(struct basic_block * head){
    // Display all basic blocks + their quads...
    struct basic_block * tmp = head;
    while(tmp){
        print_basic_block(tmp);
        tmp = tmp->next_block;
    }
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