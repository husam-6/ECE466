%code requires
{
   #include "lexer.h" 
   #include "parser.h"

   // Function prototypes
   void yyerror(const char* msg);
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
      int op_type; 
      struct string_literal str;
      char charlit;
      char *ident;
      struct number num; 
      struct astnode *astnode_p;
};


%type <ident> IDENT
%type <astnode_p> primary_expression
%type <astnode_p> postfix_expression
%type <astnode_p> unary_expression
%type <op_type> unary_operator
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
                                                      struct astnode *node = make_ast_node(IDENT_NODE);
                                                      node->ident = yylval.ident;
                                                      $$ = node; 
                                                   }       
      |               NUMBER                       {
                                                      // Number node (struct stays the same)
                                                      struct astnode *node = make_ast_node(NUM);
                                                      node->num = yylval.num;
                                                      $$ = node; 
                                                   } 
      |               STRING                       {
                                                      // String node
                                                      struct astnode *node = make_ast_node(STR_LIT);
                                                      node->str_lit.content = yylval.str.content;
                                                      node->str_lit.length = yylval.str.length;
                                                      $$ = node; 
                                                   } 
      |               CHARLIT                      {
                                                      // Charlit node
                                                      struct astnode *node = make_ast_node(CHAR_LIT);
                                                      node->char_lit = yylval.charlit;
                                                      $$ = node; 
                                                   } 
      |               '(' expression ')'           {
                                                      $$ = $2;
                                                   }
;

postfix_expression:  primary_expression
      |              postfix_expression '[' expression ']'  {
                                                                  // Addition node
                                                                  struct astnode *add = create_binary(BINOP,'+', $1, $3);
                                                                  //Deref node
                                                                  struct astnode *deref = create_unary(DEREF, '*', add);
                                                                  $$ = deref;
                                                            }

      // |              postfix_expression '(' argument_expression_listopt ')'
      |              postfix_expression '.' IDENT           {
                                                                  // Ident node
                                                                  struct astnode *ident = make_ast_node(IDENT_NODE);
                                                                  ident->ident = $3;

                                                                  struct astnode *select = create_binary(SELECT, '.', $1, ident);
                                                                  $$ = select;
                                                            
                                                            }
      |              postfix_expression INDSEL IDENT        {
                                                                  // Ident node
                                                                  struct astnode *ident = make_ast_node(IDENT_NODE);

                                                                  // Addition node
                                                                  struct astnode *add = create_binary(BINOP,'+', $1, ident);
                                                                  
                                                                  //Deref node
                                                                  struct astnode *deref = create_unary(DEREF, '*', add);
                                                                  $$ = deref;
                                                            }
      |              postfix_expression PLUSPLUS            {
                                                                  // Create node for post plus plus operation
                                                                  struct astnode *plusplus = create_unary(UNARY_OP, PLUSPLUS, $1);
                                                                  $$ = plusplus;
                                                            }
      |              postfix_expression MINUSMINUS          {
                                                                  // Create node for minus minus post operation
                                                                  struct astnode *minusminus = create_unary(UNARY_OP, MINUSMINUS, $1);
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
                                                                  //Make dummy node with the number 1 in it
                                                                  struct astnode *one = make_ast_node(NUM);
                                                                  one->num.type = I; 
                                                                  one->num.integer = 1;

                                                                  // Set up binary node for ++expr
                                                                  struct astnode *plus_eq_1 = create_binary(ASSIGNMENT_COMPOUND, PLUSEQ, $2, one);

                                                                  $$ = plus_eq_1;
                                                            }
      |              MINUSMINUS unary_expression            {
                                                                  // Set up unary node for --expr

                                                                  //Make dummy node with the number 1 in it
                                                                  struct astnode *one = make_ast_node(NUM); 
                                                                  one->num.type = I; 
                                                                  one->num.integer = 1;

                                                                  struct astnode *minus_eq_1 = create_binary(ASSIGNMENT_COMPOUND, MINUSEQ, $2, one);; 
                                                                  $$ = minus_eq_1;
                                                            }
                                                            
      |              unary_operator cast_expression         {
                                                                  struct astnode *un_op;
                                                                  //Special Case For Deref
                                                                  if ($1 == '*')
                                                                        un_op = create_unary(DEREF, $1, $2);
                                                                  else
                                                                        un_op = create_unary(UNARY_OP, $1, $2);
                                                                  
                                                                  $$ = un_op;            

                                                            }    
      |              SIZEOF '(' unary_expression ')'        {
                                                                  struct astnode *size_of = create_unary(SIZEOF_OP, SIZEOF, $3);
                                                                  $$ = size_of;         
                                                            }
      //|               SIZEOF '(' type_name ')'
;

unary_operator:      '&'            {$$ = '&';}
      |              '*'            {$$ = '*';}
      |              '+'            {$$ = '+';}
      |              '-'            {$$ = '-';}
      |              '~'            {$$ = '~';}
      |              '!'            {$$ = '!';}        
;

cast_expression:  unary_expression
   //   |           '(' type_name ')' cast_expression
;

multiplicative_expression: cast_expression
      |              multiplicative_expression '*' cast_expression      {
                                                                              struct astnode *mult = create_binary(BINOP, '*', $1, $3);
                                                                              $$ = mult; 
                                                                        }
      |              multiplicative_expression '/' cast_expression      {
                                                                              struct astnode *div = create_binary(BINOP, '/', $1, $3);
                                                                              $$ = div; 
                                                                        }
      |              multiplicative_expression '%' cast_expression      {
                                                                              struct astnode *modulo = create_binary(BINOP, '%', $1, $3);
                                                                              $$ = modulo; 
                                                                        }
;

additive_expression: multiplicative_expression
      |              additive_expression '+' multiplicative_expression  {
                                                                              struct astnode *add = create_binary(BINOP, '+', $1, $3);
                                                                              $$ = add; 
                                                                        }    
      |              additive_expression '-' multiplicative_expression  {
                                                                              struct astnode *sub = create_binary(BINOP, '-', $1, $3);
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


void yyerror(const char* msg) {
      fprintf(stderr, "%s\n", msg);
}


int main(){
   yyparse();
}