%{
   #include <stdio.h>
   #include "parser/expr.h"
   // #include "lexer/lexer.h"
   // YYSTYPE yylval;
   void yyerror(const char* msg) {
      fprintf(stderr, "%s\n", msg);
   }
   int yylex();
%}

%token IDENT CHARLIT STRING NUMBER INDSEL PLUSPLUS MINUSMINUS SHL SHR
%token LTEQ GTEQ EQEQ NOTEQ LOGAND LOGOR ELLIPSIS AUTO TIMESEQ DIVEQ MODEQ
%token PLUSEQ MINUSEQ SHLEQ SHREQ ANDEQ OREQ XOREQ BREAK CASE CHAR CONST
%token CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO IF INLINE
%token INT LONG REGISTER RESTRICT RETURN SHORT SIGNED SIZEOF STATIC
%token STRUCT SWITCH TYPEDEF UNION UNSIGNED VOID VOLATILE WHILE _BOOL
%token _COMPLEX _IMAGINARY
%start expression_list
%%

primary_expression:   IDENT {printf("FOUND IDENT\n");}
      |               NUMBER {printf("FOUND NUM\n");}
      |               STRING  {printf("FOUND STRING\n");}
      |               CHARLIT 
      |               '(' expression ')'
;

postfix_expression:  primary_expression   
      |              postfix_expression '[' expression ']'
      // |              postfix_expression '(' argument_expression_listopt ')'
      |              postfix_expression '.' IDENT
      |              postfix_expression INDSEL IDENT
      |              postfix_expression PLUSPLUS {printf("PlusPlus\n");}
      |              postfix_expression MINUSMINUS
      //|              '(' type_name ')' '{' initializer_list '}'
      //|              '(' type_name ')' '{' initializer_list ',' '}'
;

argument_expression_list: assignment_expression
      |              argument_expression_list ',' assignment_expression
;

unary_expression: postfix_expression
      |              PLUSPLUS unary_expression
      |              MINUSMINUS unary_expression {printf("MinusMinus\n");}
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
      |              additive_expression '+' multiplicative_expression        {printf("ADDITIVE EXPR\n");}
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
      |              equality_expression NOTEQ relational_expression       {printf("NOTEQ\n");}
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

conditional_expression: logical_OR_expression                                        {printf("LOGOR\n");}
      |              logical_OR_expression '?' expression ':' conditional_expression {printf("Testing ternary\n");}
;

assignment_expression:  conditional_expression 
      |              unary_expression assignment_operator assignment_expression
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


expression_list:     expression ';'                                {printf("START RULE REACHED\n");}
      |              expression_list expression ';'                {printf("START RULE REACHED\n");}
;
// constant_expression: conditional_expression
//;
%%

int main(int argc, char **argv){
   yyparse();
}