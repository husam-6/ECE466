// Helper file for functions related to bison parser
#include "parser.h"
#include "parser.tab.h"


// ast node helper function
struct astnode * make_ast_node(int type) {
      struct astnode *node = (struct astnode *)malloc(sizeof(struct astnode));
      node->type = type; 
      return node;
}

// Helper function to create unary node
struct astnode * create_unary(int op_type, int op, struct astnode *expr){
      // Set up type vars
      struct astnode * node = make_ast_node(UNARY_NODE);
      node->unary.operator_type = op_type; 
      node->unary.operator = op;

      // Assign child
      node->unary.expr = expr;
      return node;
}

// Helper function to create binary node
struct astnode * create_binary(int op_type, int op, struct astnode *left, struct astnode *right){
      // Assign node and operator type
      struct astnode * node = make_ast_node(BINARY_NODE);
      node->binary.operator_type = op_type;
      node->binary.operator = op;
      
      // Assign children
      node->binary.left = left;
      node->binary.right = right;
      return node; 
}

// Helper function to create ternary node
struct astnode * create_ternary(int op_type, struct astnode *left, struct astnode *middle, struct astnode *right){
    struct astnode * node = make_ast_node(TERNARY_NODE);
    // Node and op type
    node->ternary.operator_type = op_type;

    // Children pointers
    node->ternary.left = left;
    node->ternary.middle = middle;
    node->ternary.right = right;

    return node;
      
}

// Helper function to create ternary node
struct astnode * create_fn_node(struct astnode *postfix, struct linked_list *head){
    struct astnode * node = make_ast_node(FN_CALL);

    // Save function identifier and head of linked list
    node->fncall.postfix = postfix;
    node->fncall.head = head;

    return node;
}

// Create linked list
struct linked_list * create_ll_node(struct astnode *expr){
    struct linked_list *head = (struct linked_list *)malloc(sizeof(struct linked_list));
    head->expr = expr; 
    head->next = NULL;
    head->num_args = 1;  
    return head; 
}

// Append to linked list at end (loop even though its inefficient)
void push_ll(struct linked_list *head, struct astnode *expr){
    head->num_args++; 
    // Loop to end
    while (head->next != NULL){
        head = head->next; 
    }

    // Once at end, add new node 
    head->next = create_ll_node(expr);
}


// Helper function for tabs 
void n_tabs(int n){
    for (int i = 0; i<n; i++){
        printf("\t");
    }
}


void print_fn_args(struct linked_list *head, int depth){
    int arg_i = 1;
    while (head->next != NULL){
        n_tabs(depth);
        printf("arg %d=\n", arg_i);
        print_ast(head->expr, depth + 1);
        head = head->next; 
        arg_i++;
    }
    n_tabs(depth);
    printf("arg %d=\n", arg_i);
    print_ast(head->expr, depth + 1);
}





// operator helper function
void print_operator(int operator) {
    switch(operator){
        case INDSEL:        {printf("(->)\n"); break;}
        case PLUSPLUS:      {printf("(++)\n"); break;}
        case MINUSMINUS:    {printf("(--)\n"); break;}
        case SHL:           {printf("(<<)\n"); break;}
        case SHR:           {printf("(>>)\n"); break;}
        case LTEQ:          {printf("(<=)\n"); break;}
        case GTEQ:          {printf("(>=)\n"); break;}
        case EQEQ:          {printf("(==)\n"); break;}
        case NOTEQ:         {printf("(!=)\n"); break;}
        case LOGAND:        {printf("(&&)\n"); break;}
        case LOGOR:         {printf("(||)\n"); break;}
        case ELLIPSIS:      {printf("(...)\n"); break;}
        case TIMESEQ:       {printf("(*=)\n"); break;}
        case DIVEQ:         {printf("(/=)\n"); break;}
        case MODEQ:         {printf("(%%=)\n"); break;}
        case PLUSEQ:        {printf("(+=)\n"); break;}
        case MINUSEQ:       {printf("(-=)\n"); break;}
        case SHLEQ:         {printf("(<<=)\n"); break;}
        case SHREQ:         {printf("(>>=)\n"); break;}
        case ANDEQ:         {printf("(&=)\n"); break;}
        case OREQ:          {printf("(|=)\n"); break;}
        case XOREQ:         {printf("(^=)\n"); break;}
        case '+':           {printf("(+)\n"); break;}
        case '-':           {printf("(-)\n"); break;}
        case '*':           {printf("(*)\n"); break;}
        case '/':           {printf("(/)\n"); break;}
        case '&':           {printf("(&)\n"); break;}
        case '^':           {printf("(^)\n"); break;}
        case '|':           {printf("(|)\n"); break;}
        case '~':           {printf("(~)\n"); break;}
        case '!':           {printf("(!)\n"); break;}
        case ',':           {printf("(,)\n"); break;}
        case '=':           {printf("\n"); break;}
        case '.':           {printf("\n"); break;}
        default:            {fprintf(stderr, "Operator unknown...\n"); break;}
    }

}


// Operator type helper function
void print_op_type(int op_type) {
    switch (op_type) {
        case BINOP:                 {printf("BINARY OP "); break;}
        case ASSIGNMENT:            {printf("ASSIGNMENT "); break;}
        case ASSIGNMENT_COMPOUND:   {printf("ASSIGNMENT COMPOUND "); break;}
        case DEREF:                 {printf("DEREF "); break;}
        case ADDR_OF:               {printf("ADDRESSOF "); break;}
        case UNARY_OP:              {printf("UNARY OP "); break;}
        case SIZEOF_OP:             {printf("SIZEOF\n"); break;}
        case COMP_OP:               {printf("COMPARISON OP "); break;}
        case SELECT:                {printf("SELECT "); break;}
        case LOGICAL_OP:            {printf("LOGICAL OP "); break;}
        case TERNARY_OP:            {printf("TERNARY OP "); break;}
        case FUNC:                  {printf("FNCALL "); break;}
        default:                    {fprintf(stderr, "Unknown operator type...\n"); break;}
    }
}

char * print_datatype(int type){
    switch (type) {
        case U:         {return"int"; break;}
        case UL:        {return("unsigned long"); break;}
        case ULL:       {return("unsigned long long"); break;}
        case L:         {return "long"; break;}
        case LL:        {return "long long"; break;}
        case I:         {return "int"; break;}
        case D:         {return "double"; break;}
        case F:         {return "float"; break;}
        case LD:        {return "long"; break;}
        default:        {fprintf(stderr, "Unknown number type...\n"); return ""; break;}
    }
}


void print_ast(struct astnode * head, int depth){
    
    char * node_type;
    switch (head->type){
        case FN_CALL: {
            node_type = "FNCALL";
            n_tabs(depth);
            printf("FNCALL, %d arguments\n", head->fncall.head->num_args);
            print_ast(head->fncall.postfix, depth + 1);
            print_fn_args(head->fncall.head, depth);
            break;
        }
        case UNARY_NODE:{
            n_tabs(depth);
            print_op_type(head->unary.operator_type);
            if (head->unary.operator_type != SIZEOF_OP)
                print_operator(head->unary.operator);
            print_ast(head->unary.expr, depth + 1);
            break;
        }
        case BINARY_NODE:{
            // printf("TEST = %d\n", head->binary.operator);
            n_tabs(depth);
            print_op_type(head->binary.operator_type);
            print_operator(head->binary.operator);
            print_ast(head->binary.left, depth + 1);
            print_ast(head->binary.right, depth + 1);
            break;
        }
        case TERNARY_NODE:{
            n_tabs(depth);
            print_op_type(head->ternary.operator_type);
            printf("IF:\n");
            print_ast(head->ternary.left, depth + 1);
            n_tabs(depth);
            printf("THEN:\n");
            print_ast(head->ternary.middle, depth + 1);
            n_tabs(depth);
            printf("ELSE:\n");
            print_ast(head->ternary.right, depth + 1);
            break;
        }
        case NUM:{
            n_tabs(depth);
            printf("CONSTANT: (type=%s)", print_datatype(head->num.type));
            if (head->num.type > 5){
                printf("%Lg\n", head->num.frac);
            }
            else{
                printf("%lld\n", head->num.integer);
            }
            break;
        }
        case IDENT_NODE:{
            n_tabs(depth);
            printf("IDENT %s\n", head->ident);
            break;
        }
        case CHAR_LIT:{
            n_tabs(depth);
            printf("CONSTANT: (type=int)%d\n", head->char_lit);
            break;
        }
        case STR_LIT:{
            n_tabs(depth);
            printf("STRING %s\n", head->str_lit.content);
            break;
        }
        case ARRAY_TYPE:{
            n_tabs(depth);
            printf("ARRAY TYPE NODE: size = %d\n", head->t_node.array_node.size);
            print_ast(head->t_node.next_type, depth+1);
            break;
        }
        case SCALAR_TYPE:{
            n_tabs(depth);
            printf("SCALAR TYPE NODE: %s\n", print_datatype(head->t_node.scalar.arith_type));
            print_ast(head->t_node.next_type, depth+1);
            break;
        }
        default:{
            fprintf(stderr, "UNKNOWN NODE TYPE %d\n", head->type);
            break;
        }
    }
    
      

}