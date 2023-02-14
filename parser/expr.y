%{
   #include <stdio.h>
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
%right '='
%left '+' '-'
%left '*' '/'

%%
expr:   NUMBER             {printf("GOT HERE NUM\n");}
   |    expr '+' expr      {printf("GOT HERE\n");}
   |    expr '-' expr
   |    expr '/' expr
   |    expr '*' expr
   | '(' expr ')'

%%

int main(int argc, char **argv)
{
  yyparse();
}