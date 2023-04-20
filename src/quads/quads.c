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
        // case '<':           {printf("(<)\n"); break;}
        // case '>':           {printf("(>)\n"); break;}
        // case '=':           {printf("(=)\n"); break;}
        // case LTEQ:          {printf("(<=)\n"); break;}
        // case GTEQ:          {printf("(>=)\n"); break;}
        // case EQEQ:          {printf("(==)\n"); break;}
        // case NOTEQ:         {printf("(!=)\n"); break;}
        // case TIMESEQ:       {printf("(*=)\n"); break;}
        // case DIVEQ:         {printf("(/=)\n"); break;}
        // case MODEQ:         {printf("(%%=)\n"); break;}
        // case PLUSEQ:        {printf("(+=)\n"); break;}
        // case MINUSEQ:       {printf("(-=)\n"); break;}
        // case LOGAND:        {printf("(&&)\n"); break;}
        // case LOGOR:         {printf("(||)\n"); break;}

        // OPTIONAL - EXCLUDING FOR NOW
        // case INDSEL:        {printf("(->)\n"); break;}
        // case SHL:           {printf("(<<)\n"); break;}
        // case SHR:           {printf("(>>)\n"); break;}
        // case ',':           {printf("(,)\n"); break;}
        // case ELLIPSIS:      {printf("(...)\n"); break;}
        // case SHLEQ:         {printf("(<<=)\n"); break;}
        // case SHREQ:         {printf("(>>=)\n"); break;}
        // case ANDEQ:         {printf("(&=)\n"); break;}
        // case OREQ:          {printf("(|=)\n"); break;}
        // case XOREQ:         {printf("(^=)\n"); break;}
        // case '&':           {printf("(&)\n"); break;}
        // case '^':           {printf("(^)\n"); break;}
        // case '|':           {printf("(|)\n"); break;}
        // case '~':           {printf("(~)\n"); break;}
        // case '.':           {printf("(.)\n"); break;}
        // case '!':           {printf("(!)\n"); break;}
        default:            {fprintf(stderr, "Unsupported (currently) operator...%d\n", operator); break;}
    }
}

// Generate r value of a given node
// Code from hak lecture notes 5, starting page 10
struct generic_node * gen_rvalue(struct astnode * node, struct generic_node * target){
    if (node->type==IDENT_NODE){
        struct generic_node * ident = make_generic_node(VARIABLE);
        ident->var = node->ident;
        return ident;
    }
    if (node->type==NUM){
        struct generic_node * constant = make_generic_node(CONSTANT);
        constant->num = node->num;
        return constant;
    }
    if (node->type==BINARY_NODE && !node->binary.abstract){
        //Ordinary binary operator case, ignoring type conversions
        struct generic_node * left = gen_rvalue(node->binary.left, NULL);
        struct generic_node * right = gen_rvalue(node->binary.right, NULL);
        if (!target)
            target = new_temporary();
        emit(get_opcode(node->binary.operator), left, right, target);
        return target;
    }

    return target; 
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
    struct quad * tmp = block_head->head; 
    if (!tmp) block_head->head = q; 
    else{
        while(tmp && tmp->next_quad)
            tmp = tmp->next_quad;
        tmp->next_quad = q; 
    }
    // block_head->head = q;
    // print_quad(q);
}

// prints parameters of a quad
void print_generic_node(struct generic_node * node){
    switch(node->type){
        case (CONSTANT):        {
                                    if (node->num.type > 5) printf("%Lg", node->num.frac);
                                    else printf("%lld", node->num.integer);
                                    break;
                                }
        case (VARIABLE):        {printf("%s", node->var.name); break;}
        case (TEMPORARY):       {printf("%s", node->temp.ident); break;}
        case (STRING_LIT):      {printf("%s", node->str.content); break;}
        default:                {fprintf(stderr, "Unrecognized generic node type %d...\n", node->type);}
    }
}

void print_op_code(enum quad_opcode opcode){
    switch(opcode){
        case ADD:           {printf("ADD"); break;}
        case SUB:           {printf("SUB"); break;}
        case MUL:           {printf("MUL"); break;}
        case DIV:           {printf("DIV"); break;}
        default:            {fprintf(stderr, "Unsupported op code\n");}
    }

}

// Helper to print any given quad
void print_quad(struct quad * q){
    printf("\t");
    // If target is present
    if (q->result){
        print_generic_node(q->result);
        printf(" = ");
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

// Helper function to create a new temporary node / register
struct generic_node * new_temporary(){
    struct generic_node * node = make_generic_node(TEMPORARY);
    node->temp.reg_num = register_counter++;
    char * tmp; 
    asprintf(&tmp, "%%T%06d", node->temp.reg_num);
    node->temp.ident = tmp;
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

    char * tmp; 
    asprintf(&tmp, ".BB%d.%d", func_counter++, bb_counter++);
    block->label = tmp;
    // block->label = ".BB0.0";
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
    print_basic_block(block_head);
    // printf("#####\t\t End of Quads \t\t #####\n");
}