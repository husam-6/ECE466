%code requires
{
   #include "lexer.h" 
   #include "parser.h"

   // Function prototypes
   void yyerror(const char* msg);
   void create_unary(struct astnode *node, int op_type, int operator);
   void create_binary(struct astnode *node, int op_type, int operator);
   int yylex();
}

%token IDENT CHARLIT STRING NUMBER INDSEL PLUSPLUS MINUSMINUS SHL SHR
%token LTEQ GTEQ EQEQ NOTEQ LOGAND LOGOR ELLIPSIS AUTO TIMESEQ DIVEQ MODEQ
%token PLUSEQ MINUSEQ SHLEQ SHREQ ANDEQ OREQ XOREQ BREAK CASE CHAR CONST
%token CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO IF INLINE
%token INT LONG REGISTER RESTRICT RETURN SHORT SIGNED SIZEOF STATIC
%token STRUCT SWITCH TYPEDEF UNION UNSIGNED VOID VOLATILE WHILE _BOOL
%token _COMPLEX _IMAGINARY

%start expression_list

%union {
      struct string_literal str;
      char charlit;
      char *ident;
      struct number num; 
      struct astnode *astnode_p;
};


/* %union {
}; */

%type <astnode_p> primary_expression
%type <astnode_p> postfix_expression
%type <astnode_p> unary_expression
%type <charlit> unary_operator
%type <astnode_p> cast_expression
%type <astnode_p> multiplicative_expression
%type <astnode_p> additive_expression
// %type <astnode_p> shift_expression
// %type <astnode_p> relational_expression
// %type <astnode_p> equality_expression
// %type <astnode_p> AND_expression
// %type <astnode_p> exclusive_OR_expression
// %type <astnode_p> inclusive_OR_expression
// %type <astnode_p> logical_AND_expression
// %type <astnode_p> logical_OR_expression
// %type <astnode_p> conditional_expression
// %type <astnode_p> assignment_expression
// %type <astnode_p> assignment_operator
%type <astnode_p> expression
// %type <astnode_p> function_call
// %type <astnode_p> function_arguments
// %type <astnode_p> expression_list


%%
// **** Preorder Traversal ****
primary_expression:   IDENT                        {
                                                      // Identifier node
                                                      struct astnode *node = (struct astnode *)malloc(sizeof(struct astnode));
                                                      node->type = IDENT_NODE; 
                                                      node->ident = yylval.ident;
                                                      $$ = node; 
                                                   }       
      |               NUMBER                       {
                                                      // Number node (struct stays the same)
                                                      struct astnode *node = (struct astnode *)malloc(sizeof(struct astnode));
                                                      node->type = NUM; 
                                                      node->num = yylval.num;
                                                      $$ = node; 
                                                   } 
      |               STRING                       {
                                                      // String node
                                                      struct astnode *node = (struct astnode *)malloc(sizeof(struct astnode));
                                                      node->type = STR_LIT; 
                                                      node->str_lit.content = yylval.str.content;
                                                      node->str_lit.length = yylval.str.length;
                                                      $$ = node; 
                                                   } 
      |               CHARLIT                      {
                                                      // Charlit node
                                                      struct astnode *node = (struct astnode *)malloc(sizeof(struct astnode));
                                                      node->type = CHAR_LIT; 
                                                      node->char_lit = yylval.charlit;
                                                      $$ = node; 
                                                   } 
      |               '(' expression ')'           {
                                                      $$ = $2;
                                                   }
;

postfix_expression:  primary_expression
      |              postfix_expression '[' expression ']'  {
                                                                  //Deref node
                                                                  struct astnode *deref = (struct astnode *)malloc(sizeof(struct astnode));
                                                                  create_unary(deref, DEREF,'*');
                                                                  
                                                                  //Add node after
                                                                  struct astnode *add = (struct astnode *)malloc(sizeof(struct astnode));
                                                                  deref->unary.expr = add;
                                                                  create_binary(add, BINOP,'+');

                                                                  // Assign addition node children
                                                                  add->binary.left = $1;
                                                                  add->binary.right = $3;

                                                                  $$ = deref;
                                                            }

      // |              postfix_expression '(' argument_expression_listopt ')'
      |              postfix_expression '.' IDENT           {
                                                                  //MIGHT HAVE TO FIX BASED ON OUTPUT FILE
                                                                  //BINARY but on same line
                                                                  // struct astnode *select = (struct astnode *)malloc(sizeof(struct astnode));  
                                                                  // create_binary(select, SELECT, '.')
                                                                  // select->binary.left = $1;
                                                                  // select->binary.right = $3;
                                                                  // $$ = select;
                                                            
                                                            }
      |              postfix_expression INDSEL IDENT        {
                                                                  //MIGHT HAVE TO FIX BASED ON OUTPUT FILE
                                                                  //BINARY but on same line
                                                                  // struct astnode *indselect = (struct astnode *)malloc(sizeof(struct astnode));  
                                                                  // indselect->type = BINARY_NODE; 
                                                                  // indselect->binary.op_type = INDIRECT_SELECT; 
                                                                  // indselect->binary.operator = INDSEL;
                                                                  // indselect->binary.left = $1;
                                                                  // indselect->binary.right = $3;
                                                                  // $$ = indselect;
                                                            }
      |              postfix_expression PLUSPLUS            {
                                                                  struct astnode *plusplus = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                  // plusplus->type = UNARY_NODE; 
                                                                  // plusplus->unary.op_type = UNARY_OP; 
                                                                  // plusplus->unary.operator = PLUSPLUS;
                                                                  create_unary(plusplus, UNARY_OP, PLUSPLUS);
                                                                  plusplus->unary.expr = $1;
                                                                  $$ = plusplus;
                                                            }
      |              postfix_expression MINUSMINUS          {

                                                                  struct astnode *minusminus = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                  // minusminus->type = UNARY_NODE; 
                                                                  // minusminus->unary.op_type = UNARY_OP; 
                                                                  // minusminus->unary.operator = MINUSMINUS;
                                                                  create_unary(minusminus, UNARY_OP, MINUSMINUS);
                                                                  minusminus->unary.expr = $1;
                                                                  $$ = minusminus;
                                                            }
      //|              '(' type_name ')' '{' initializer_list '}'
      //|              '(' type_name ')' '{' initializer_list ',' '}'
;

// argument_expression_list: assignment_expression
//       |              argument_expression_list ',' assignment_expression
// ;

unary_expression:    postfix_expression
      |              function_call                          
      |              PLUSPLUS unary_expression              {
                                                                  struct astnode *plus_eq_1 = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                  create_binary(plus_eq_1, ASSIGNMENT_COMPOUND, PLUSEQ);

                                                                  plus_eq_1->binary.left = $2;


                                                                  //Make dummy node with the number 1 in it
                                                                  struct astnode *one = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                  one->type = NUM; 
                                                                  one->num.type = I; 
                                                                  one->num.integer = 1;


                                                                  plus_eq_1->binary.right = one;
                                                                  $$ = plus_eq_1;
                                                            }
      |              MINUSMINUS unary_expression            {
                                                                  struct astnode *minus_eq_1 = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                  create_binary(minus_eq_1, ASSIGNMENT_COMPOUND, MINUSEQ);
                                                                  minus_eq_1->binary.left = $2;


                                                                  //Make dummy node with the number 1 in it
                                                                  struct astnode *one = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                  one->type = NUM; 
                                                                  one->num.type = I; 
                                                                  one->num.integer = 1;


                                                                  minus_eq_1->binary.right = one;
                                                                  $$ = minus_eq_1;
                                                            }
                                                            
      |              unary_operator cast_expression         {
                                                                  //Special Case For Deref???
                                                                  struct astnode *un_op = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                  create_unary(un_op, UNARY_OP, $1);
                                                                  un_op->unary.expr = $2;
                                                                  $$ = un_op;               

                                                            }    
      |              SIZEOF '(' unary_expression ')'        {
                                                                  struct astnode *size_of = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                  create_unary(size_of, SIZEOF_OP, SIZEOF);
                                                                  size_of->unary.expr = $3;
                                                                  $$ = size_of;         
                                                            }
      //|               SIZEOF '(' type_name ')'
;

unary_operator:      '&'
      |              '*'
      |              '+'
      |              '-'
      |              '~'
      |              '!'                    
;

cast_expression:  unary_expression
   //   |           '(' type_name ')' cast_expression
;

multiplicative_expression: cast_expression
      |              multiplicative_expression '*' cast_expression      {
                                                                              struct astnode *mult = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                              create_binary(mult, BINOP, '*');
                                                                              mult->binary.left = $1;
                                                                              mult->binary.right = $3;
                                                                              $$ = mult; 
                                                                        }
      |              multiplicative_expression '/' cast_expression      {
                                                                              struct astnode *div = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                              create_binary(div, BINOP, '/');
                                                                              div->binary.left = $1;
                                                                              div->binary.right = $3;
                                                                              $$ = div; 
                                                                        }
      |              multiplicative_expression '%' cast_expression      {
                                                                              struct astnode *modulo = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                              create_binary(modulo, BINOP, '%');
                                                                              modulo->binary.left = $1;
                                                                              modulo->binary.right = $3;
                                                                              $$ = modulo; 
                                                                        }
;

additive_expression: multiplicative_expression
      |              additive_expression '+' multiplicative_expression  {
                                                                              struct astnode *add = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                              create_binary(add, BINOP, '+');
                                                                              add->binary.left = $1;
                                                                              add->binary.right = $3;
                                                                              $$ = add; 
                                                                        }    
      |              additive_expression '-' multiplicative_expression  {
                                                                              struct astnode *sub = (struct astnode *)malloc(sizeof(struct astnode)); 
                                                                              create_binary(sub, BINOP, '-');
                                                                              sub->binary.left = $1;
                                                                              sub->binary.right = $3;
                                                                              $$ = sub; 
                                                                        }
;

shift_expression:    additive_expression
      |              shift_expression SHL additive_expression
      |              shift_expression SHR additive_expression
;

relational_expression: shift_expression
      |              relational_expression '<' shift_expression
      |              relational_expression '>' shift_expression
      |              relational_expression LTEQ shift_expression 
      |              relational_expression GTEQ shift_expression
;  

equality_expression: relational_expression
      |              equality_expression EQEQ relational_expression
      |              equality_expression NOTEQ relational_expression       
;

AND_expression:      equality_expression
      |              AND_expression '&' equality_expression
;

exclusive_OR_expression: AND_expression
      |              exclusive_OR_expression '^' AND_expression
;

inclusive_OR_expression: exclusive_OR_expression
      |              inclusive_OR_expression '|' exclusive_OR_expression
;

logical_AND_expression: inclusive_OR_expression
      |              logical_AND_expression LOGAND inclusive_OR_expression
;

logical_OR_expression:  logical_AND_expression
      |              logical_OR_expression LOGOR logical_AND_expression
;

conditional_expression: logical_OR_expression                                        
      |              logical_OR_expression '?' expression ':' conditional_expression 
;

assignment_expression:  conditional_expression 
      |              unary_expression assignment_operator assignment_expression //{make new binary opp node left pointer = $1, $3 = right}
;

assignment_operator: '='
      |              TIMESEQ
      |              DIVEQ
      |              MODEQ 
      |              PLUSEQ
      |              MINUSEQ
      |              SHLEQ
      |              SHREQ
      |              ANDEQ
      |              XOREQ
      |              OREQ
;


expression:           assignment_expression
      |               expression ',' assignment_expression  
;

function_call:       postfix_expression '(' function_arguments ')'

function_arguments:  //EMPTY
      |              expression

expression_list:     expression ';'                                {printf("START RULE REACHED\n");}
      |              expression_list expression ';'                {printf("START RULE REACHED\n");}
;
// constant_expression: conditional_expression
//;
%%

// Helper function to create unary node
void create_unary(struct astnode *node, int op_type, int operator){
      node->type = UNARY_NODE; 
      node->unary.operator_type = op_type; 
      node->unary.operator = operator;
}

// Helper function to create binary node
void create_binary(struct astnode *node, int op_type, int operator){
      node->type = BINARY_NODE; 
      node->binary.operator_type = op_type;
      node->binary.operator = operator;
}

// Helper function to create ternary node
void create_ternary(){
      
}


void yyerror(const char* msg) {
      fprintf(stderr, "%s\n", msg);
}


int main(int argc, char **argv){
   yyparse();
}