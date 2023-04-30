// Helper file for functions related to bison parser
#include "parser.h"
#include "parser.tab.h"
#include "sym.h"
#include "type.h"

// Helper function for tabs 
void n_tabs(int n){
    for (int i = 0; i<n; i++){
        printf("\t");
    }
}


void print_fn_args(struct linked_list *head, int depth){
    int arg_i = 1;
    while (head != NULL){
        if (head->expr != NULL){
            n_tabs(depth);
            printf("arg %d=\n", arg_i);
            print_ast(head->expr, depth + 1);
        }
        head = head->next; 
        arg_i++;
    }
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
        case '<':           {printf("(<)\n"); break;}
        case '>':           {printf("(>)\n"); break;}
        case '=':           {printf("(=)\n"); break;}
        case '.':           {printf("(.)\n"); break;}
        default:            {fprintf(stderr, "Operator unknown...%d\n", operator); break;}
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
        case CAST_OP:               {printf("CAST\n"); break;}
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
        case U:         {return"unsigned"; break;}
        case UL:        {return("unsigned long"); break;}
        case ULL:       {return("unsigned long long"); break;}
        case L:         {return "long"; break;}
        case LL:        {return "long long"; break;}
        case I:         {return "int"; break;}
        case D:         {return "double"; break;}
        case F:         {return "float"; break;}
        case LD:        {return "long"; break;}
        case C:         {return "char"; break;}
        case UC:        {return "unsigned char"; break;}
        case SH:        {return "short"; break;}
        case S:         {return "signed"; break;}
        case V:         {return "void"; break;}
        default:        {fprintf(stderr, "Unknown number type...\n"); return ""; break;}
    }
}

char * print_jump_type(enum jump_stmt jump_type){
    switch(jump_type){
        case GOTO_JUMP:         {return "GOTO"; break;} 
        case CONTINUE_JUMP:     {return "CONTINUE"; break;} 
        case BREAK_JUMP:        {return "BREAK"; break;}
        case RETURN_JUMP:       {return "RETURN"; break;}
        default:                {fprintf(stderr, "Unknown jump type...\n"); return ""; break;}
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
            print_fn_args(head->fncall.head, depth + 1);
            break;
        }
        case UNARY_NODE:{
            n_tabs(depth);
            print_op_type(head->unary.operator_type);
            if (head->unary.operator != SIZEOF && !head->unary.abstract){
                print_operator(head->unary.operator);
            }
            if (!head->unary.abstract)
                print_ast(head->unary.expr, depth + 1);
            else{
                print_type(head->unary.type, depth + 1);
            }
            break;
        }
        case BINARY_NODE:{
            // printf("TEST = %d\n", head->binary.operator);
            n_tabs(depth);
            print_op_type(head->binary.operator_type);
            if (!head->binary.abstract){
                print_operator(head->binary.operator);
                print_ast(head->binary.left, depth + 1);
            }
            else
                print_type(head->binary.type, depth + 1);

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
            struct astnode_symbol * tmp = head->ident.sym;
            if (tmp != NULL){
                printf("%s identifier %s defined @ %s:%d\n", print_namespace(tmp->n_space), head->ident.name, tmp->file_name, tmp->line_num);
            }
            else{
                printf("undef identifier %s\n", head->ident.name);
                // fprintf(stderr, "USE OF UNDECLARED IDENTIFIER\n");
                // exit(2);
            }
            break;
        }
        case CHAR_LIT:{
            n_tabs(depth);
            printf("CONSTANT: (type=int)%d\n", head->char_lit);
            break;
        }
        case STR_LIT:{
            n_tabs(depth);
            printf("STRING \"", head->str_lit.content);
            for (int i = 0; i < head->str_lit.length; i++){
                // printf("%hhx", yylval.str.content[i]);
                char * tmp = to_char(head->str_lit.content[i]);
                printf("%s", tmp);
            }
            printf("\"");
            printf("\n");
            break;
        }
        case FOR_LOOP: {
            n_tabs(depth);
            printf("FOR\n");

            // Initial expression
            n_tabs(depth+1);
            printf("INIT:\n");
            print_ast(head->for_loop.init, depth+2);
            
            // Condition
            n_tabs(depth+1);
            printf("COND:\n");
            print_ast(head->for_loop.cond, depth+2);
            
            // Body expressions
            n_tabs(depth+1);
            printf("BODY:\n");
            print_ast(head->for_loop.body, depth+2);
            
            // Increment
            n_tabs(depth+1);
            printf("INCR:\n");
            print_ast(head->for_loop.inc, depth+2);
            break;
        }
        case WHILE_LOOP:{
            n_tabs(depth);
            if (head->while_loop.do_while == 1){
                printf("DO:\n");
                n_tabs(depth+1);
                printf("BODY:\n");
                print_ast(head->while_loop.stmt, depth+2);

                n_tabs(depth+1);
                printf("WHILE / COND:\n");
                print_ast(head->while_loop.cond, depth+2);

                break;
            }
            printf("WHILE:\n");
            n_tabs(depth+1);
            printf("COND:\n");
            print_ast(head->while_loop.cond, depth+2);

            n_tabs(depth+1);
            printf("BODY:\n");
            print_ast(head->while_loop.stmt, depth+2);

            break;
        }
        case IF_STMT: {
            n_tabs(depth);
            if (head->if_stmt.cond_type == IF_NODE)
                printf("IF:\n");
            else
                printf("SWITCH:\n");

            n_tabs(depth+1);
            printf("COND:\n");
            print_ast(head->if_stmt.cond, depth+2);

            n_tabs(depth+1);
            printf("BODY:\n");
            print_ast(head->if_stmt.stmt, depth+2);

            if (head->if_stmt.else_stmt){
                n_tabs(depth+1);
                printf("ELSE:\n");
                print_ast(head->if_stmt.else_stmt, depth+2);
            }
            break;
        }
        case LABEL_NODE:{
            n_tabs(depth);
            printf("LABEL(%s):\n", head->label.name);
            print_ast(head->label.stmt, depth+1);
            break;
        }
        case CASE_NODE:{
            n_tabs(depth);
            if (head->case_n.case_type == CASE_STMT){
                printf("CASE:\n");

                n_tabs(depth+1);
                printf("EXPR:\n");
                print_ast(head->case_n.expr, depth+2);
            }
            else
                printf("DEFAULT:\n");
            
            n_tabs(depth+1);
            printf("STMT:\n");
            print_ast(head->case_n.stmt, depth+2);
            break;
        }
        case JUMP_NODE:{
            n_tabs(depth);
            printf("%s ", print_jump_type(head->jump.jump_type));

            if (head->jump.jump_type == GOTO_JUMP){
                printf("%s ", head->jump.ident.name);
                if (head->jump.ident.sym && head->jump.ident.sym->type->complete)
                    printf("(def @ %s:%d)\n", head->jump.ident.sym->file_name, head->jump.ident.sym->line_num);
                else
                    printf("(undef)\n");
                break;
            }
            else if (head->jump.jump_type == RETURN_JUMP){
                printf("\n");
                if (head->jump.expr){
                    n_tabs(depth+1);
                    printf("EXPR:\n");
                    print_ast(head->jump.expr, depth+2);
                }
                break;
            }
            printf("\n");
            break;
        }
        case COMPOUND:{
            struct linked_list * tmp = head->ds_list;
            while(tmp != NULL){
                print_ast(tmp->expr, depth);
                tmp = tmp->next;
            }
            break;
        }
        case DECLARATION:{
            break;
        }
        default:{
            fprintf(stderr, "UNKNOWN NODE TYPE %d\n", head->type);
            break;
        }
    }
}

// ast node helper function
struct astnode * make_ast_node(int type) {
    struct astnode *node = (struct astnode *)malloc(sizeof(struct astnode));
    node->type = type;
    node->file_name = file_name;
    node->line_num = line_num;
    return node;
}

// Helper function to create unary node
struct astnode * create_unary(int op_type, int op, struct astnode *expr, struct type_node *type){

    // Set up type vars
    struct astnode * node = make_ast_node(UNARY_NODE);
    node->unary.operator_type = op_type; 
    node->unary.operator = op;

    // Assign child
    if (expr)
        node->unary.expr = expr;
    else if (type){
        node->unary.type = type;
        node->unary.abstract = 1;
    }
    return node;
}

// Helper function to create binary node
struct astnode * create_binary(int op_type, int op, struct astnode *left, struct astnode *right, struct type_node *type){

    // Assign node and operator type
    struct astnode * node = make_ast_node(BINARY_NODE);
    node->binary.operator_type = op_type;
    node->binary.operator = op;
    
    // Assign children
    if (left)
        node->binary.left = left;
    else{
        node->binary.abstract = 1;
        node->binary.type = type; 
    }
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

struct astnode * create_for_loop(struct astnode * init, struct astnode * cond, struct astnode * body, struct astnode * inc){
    struct astnode * node = make_ast_node(FOR_LOOP);    
    node->for_loop.init = init;
    node->for_loop.cond = cond; 
    node->for_loop.body = body; 
    node->for_loop.inc = inc;
    return node; 
}

struct astnode * create_while_loop(struct astnode * stmt, struct astnode * cond, int do_while){
    struct astnode * node = make_ast_node(WHILE_LOOP);
    node->while_loop.stmt = stmt;
    node->while_loop.cond = cond; 
    node->while_loop.do_while = do_while; 
    return node; 
}

struct astnode * create_if_stmt(struct astnode * stmt, struct astnode * cond, struct astnode * else_stmt, enum switch_or_if cond_type){
    struct astnode * node = make_ast_node(IF_STMT);
    node->if_stmt.cond_type = cond_type;
    node->if_stmt.stmt = stmt;
    node->if_stmt.cond = cond; 
    node->if_stmt.else_stmt = else_stmt; 
    return node; 
}


struct astnode * create_label_node(char * name, struct astnode * stmt){
    struct astnode * node = make_ast_node(LABEL_NODE);
    node->label.name = name;
    node->label.stmt = stmt;
    return node; 
}

struct astnode * create_case_node(struct astnode * expr, struct astnode * stmt, enum case_default case_type){
    struct astnode * node = make_ast_node(CASE_NODE);
    node->case_n.expr = expr;
    node->case_n.stmt = stmt;
    node->case_n.case_type = case_type; 
    return node; 
}

// Create linked list
struct linked_list * create_ll_node(struct astnode *expr){
    struct linked_list *head = (struct linked_list *)malloc(sizeof(struct linked_list));
    head->expr = expr; 
    head->next = NULL;
    head->num_args = 0;
    if (expr != NULL)
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

// Function to search symbol table for an identifier and point an ident node to the symbol
void resolve_identifier(char * ident, enum namespace n_space, struct astnode_symbol ** node){
    struct astnode_symbol * sym; 
    int in_table = search_all_tabs(ident, n_space, curr_scope, &sym);

    // Point to struct in symbol table
    if (in_table == 1)
          (*node) = sym;
    else if (n_space == VAR_S){
          yyerror("USE OF UNDECLARED IDENTIFIER");
          fprintf(stderr, "IDENTIFIER: %s\n", ident);
          exit(2);
    }
}


void dump_ast(struct linked_list *asthead, int tabs, char * name){
    if (tabs == 0)
        printf("************ AST DUMP: %s ************\nLIST { \n", name);
    int depth = 1; 
    while(asthead != NULL){
        print_ast(asthead->expr, depth);
        asthead = asthead->next;
    }
    printf("}\n");
    if (tabs == 0)
        printf("************ END OF AST DUMP ************\n");
}