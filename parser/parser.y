%code requires
{
   #include "lexer/lexer.h" 
   #include "parser/parser.h"
   //YYSTYPE yylval;
   // yylval
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
      struct string_literal str;
      char charlit;
      char *ident;
      struct number num; 
      struct astnode *astnode_p;
};


/* %union {
}; */

%type <astnode_p> primary_expression
// %type <astnode_p> postfix_expression
// %type <astnode_p> unary_expression
// %type <astnode_p> cast_expression
// %type <astnode_p> multiplicative_expression
// %type <astnode_p> additive_expression
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
                                                      struct astnode node; node.type = IDENT_NODE; 
                                                      node.ident.ident = yylval.ident;
                                                      $$ = &node; 
                                                   }       
      |               NUMBER                       {
                                                      struct astnode node; node.type = NUM; 
                                                      node.num.type = yylval.num.type;
                                                      if (yylval.num.type == F || yylval.num.type == D || yylval.num.type == LD){
                                                            node.num.frac = yylval.num.frac;
                                                      }
                                                      else{
                                                            node.num.integer = yylval.num.integer; 
                                                      }
                                                      $$ = &node; 
                                                   } 
      |               STRING                       {
                                                      struct astnode node; node.type = STR_LIT; 
                                                      node.str_lit.str = yylval.str.content;
                                                      $$ = &node; 
                                                   } 
      |               CHARLIT                      {
                                                      struct astnode node; node.type = CHAR_LIT; 
                                                      node.char_lit.c = yylval.charlit;
                                                      $$ = &node; 
                                                   } 
      |               '(' expression ')'           {
                                                      $$ = $2;
                                                   }
;

postfix_expression:  primary_expression   
      |              postfix_expression '[' expression ']'
      // |              postfix_expression '(' argument_expression_listopt ')'
      |              postfix_expression '.' IDENT
      |              postfix_expression INDSEL IDENT
      |              postfix_expression PLUSPLUS 
      |              postfix_expression MINUSMINUS
      //|              '(' type_name ')' '{' initializer_list '}'
      //|              '(' type_name ')' '{' initializer_list ',' '}'
;

// argument_expression_list: assignment_expression
//       |              argument_expression_list ',' assignment_expression
// ;

unary_expression: postfix_expression
      |              function_call
      |              PLUSPLUS unary_expression
      |              MINUSMINUS unary_expression 
      |              unary_operator cast_expression      
      |              SIZEOF '(' unary_expression ')'
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
      |              multiplicative_expression '*' cast_expression
      |              multiplicative_expression '/' cast_expression
      |              multiplicative_expression '%' cast_expression
;

additive_expression: multiplicative_expression
      |              additive_expression '+' multiplicative_expression        
      |              additive_expression '-' multiplicative_expression
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


int main(int argc, char **argv){
   yyparse();
}