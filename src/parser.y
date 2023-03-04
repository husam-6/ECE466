%code requires
{
   #include "lexer.h" 
   #include "parser.h"

   // Function prototypes
   void yyerror(const char* msg);
   int yylex();

   // Global symbol table
   /* struct scope global = {.head = NULL, .next = NULL,.outer = NULL}; */
}

%token IDENT CHARLIT STRING NUMBER INDSEL PLUSPLUS MINUSMINUS SHL SHR
%token LTEQ GTEQ EQEQ NOTEQ LOGAND LOGOR ELLIPSIS AUTO TIMESEQ DIVEQ MODEQ
%token PLUSEQ MINUSEQ SHLEQ SHREQ ANDEQ OREQ XOREQ BREAK CASE CHAR CONST
%token CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO IF INLINE
%token INT LONG REGISTER RESTRICT RETURN SHORT SIGNED SIZEOF STATIC
%token STRUCT SWITCH TYPEDEF UNION UNSIGNED VOID VOLATILE WHILE _BOOL
%token _COMPLEX _IMAGINARY
%start declaration_or_fndef_list
/* %start statement */


%union {
      int token;
      int op_type; 
      struct string_literal str;
      char charlit;
      char *ident;
      struct number num; 
      struct astnode *astnode_p;
      struct linked_list *ll_p;
};


%type <ident> IDENT 
%type <token> LTEQ GTEQ EQEQ NOTEQ LOGAND LOGOR TIMESEQ DIVEQ MODEQ PLUSEQ
%type <token> MINUSEQ SHLEQ SHREQ ANDEQ XOREQ OREQ assignment_operator
%type <op_type> unary_operator
%type <astnode_p> primary_expression postfix_expression unary_expression cast_expression multiplicative_expression
%type <astnode_p> additive_expression shift_expression relational_expression equality_expression AND_expression
%type <astnode_p> exclusive_OR_expression inclusive_OR_expression logical_AND_expression logical_OR_expression conditional_expression
%type <astnode_p> assignment_expression expression function_call // expression_list
/* %type <ll_p> function_arguments */
%type <ll_p> function_arguments


%%
// Top Level (From Hak)
declaration_or_fndef_list:    declaration_or_fndef
      |                       declaration_or_fndef_list declaration_or_fndef

declaration_or_fndef:         declaration
      |                       function_definition
;

function_definition: declaration_specifiers declarator compound_statement
;

statement:        compound_statement
      |           expression ';'                {print_ast($1, 0);}
;

compound_statement: '{' decl_or_stmt_list '}'
;

decl_or_stmt_list:      decl_or_stmt 
      |                 decl_or_stmt_list decl_or_stmt
;

decl_or_stmt:     declaration
      |           statement
;


// Declarations 6.7
declaration:            declaration_specifiers init_declarator_list ';'
      |                 declaration_specifiers  ';'
;

declaration_specifiers: storage_class_specifier declaration_specifiers
      |                 storage_class_specifier
      |                 type_specifier declaration_specifiers
      |                 type_specifier
      |                 type_qualifier declaration_specifiers
      |                 type_qualifier
      |                 function_specifier declaration_specifiers
      |                 function_specifier
;

init_declarator_list:   init_declarator
      |                 init_declarator_list ',' init_declarator
;

init_declarator:        declarator
;

// 6.7.1
storage_class_specifier:      //TYPEDEF
                              EXTERN
      |                       STATIC
      |                       AUTO
      |                       REGISTER
;

// 6.7.2
type_specifier:   VOID
      |           CHAR
      |           SHORT
      |           INT 
      |           LONG
      |           FLOAT
      |           DOUBLE
      |           SIGNED
      |           UNSIGNED
      |           _BOOL
      |           _COMPLEX
      |           struct_or_union_specifier
      //|           enum_specifier
      //|           typedef_name
;

struct_or_union_specifier:    struct_or_union IDENT '{' struct_declaration_list '}'
      |                       struct_or_union IDENT
      |                       struct_or_union '{' struct_declaration_list '}'
;

// 6.7.2.1
struct_or_union:              STRUCT
      |                       UNION
;

struct_declaration_list:      struct_declaration
      |                       struct_declaration_list struct_declaration
;

struct_declaration:           specifier_qualifier_list struct_declarator_list ';'
;

specifier_qualifier_list:     type_specifier specifier_qualifier_list
      |                       type_specifier
      |                       type_qualifier specifier_qualifier_list
      |                       type_qualifier
;

struct_declarator_list:       struct_declarator
      |                       struct_declarator_list ',' struct_declarator
;

struct_declarator:      declarator
      |                 declarator ':' constant_expression
      |                 ':' constant_expression
;

// 6.7.3
type_qualifier:         CONST
      |                 RESTRICT
      |                 VOLATILE
;

// 6.7.4
function_specifier:     INLINE
;

// 6.7.5
declarator:             pointer direct_declarator
      |                 direct_declarator
;

direct_declarator:      IDENT
      |                 '(' declarator ')'
      |                 direct_declarator '[' type_qualifier_list assignment_expression ']'
      |                 direct_declarator '[' assignment_expression ']'
      |                 direct_declarator '[' type_qualifier_list   ']'
      |                 direct_declarator '['  ']'
      |                 direct_declarator '[' STATIC type_qualifier_list assignment_expression ']'
      |                 direct_declarator '[' STATIC assignment_expression ']'
      |                 direct_declarator '[' type_qualifier_list STATIC assignment_expression ']'
      |                 direct_declarator '[' type_qualifier_list '*' ']'
      |                 direct_declarator '['  '*' ']'
      |                 direct_declarator '(' parameter_type_list ')'
      |                 direct_declarator '(' identifier_list ')'
      |                 direct_declarator '(' ')'
;

pointer:                '*' type_qualifier_list
      |                 '*'   
      |                 '*' type_qualifier_list pointer
      |                 '*' pointer
;

type_qualifier_list:    type_qualifier
      |                 type_qualifier_list type_qualifier
;

parameter_type_list:    parameter_list
      |                 parameter_list ',' ELLIPSIS
;

parameter_list:         parameter_declaration
      |                 parameter_list ',' parameter_declaration
;

parameter_declaration:  declaration_specifiers declarator
      |                 declaration_specifiers abstract_declarator
      |                 declaration_specifiers
;

identifier_list:        IDENT
      |                 identifier_list ',' IDENT
;

// 6.7.6
type_name:              specifier_qualifier_list abstract_declarator
      |                 specifier_qualifier_list
;

abstract_declarator:    pointer
      |                 pointer direct_abstract_declarator
      |                 direct_abstract_declarator
;

direct_abstract_declarator:   '(' abstract_declarator ')'
      |                       direct_abstract_declarator '[' assignment_expression ']'
      |                       direct_abstract_declarator '['  ']'
      |                       '[' assignment_expression ']'
      |                       '['  ']'
      |                       direct_abstract_declarator '[' '*' ']'
      |                       '[' '*' ']'
      |                       direct_abstract_declarator '(' parameter_type_list ')'
      |                       direct_abstract_declarator '('  ')'
      |                       '(' parameter_type_list ')'
      |                       '('  ')'
;

/* block_item:             declaration
      |                 statement
; */


// Expressions 6.5.1
primary_expression:   IDENT                        {
                                                      // Identifier node
                                                      struct astnode *node = make_ast_node(IDENT_NODE);
                                                      node->ident = yylval.ident;
                                                      $$ = node; 
                                                      // Look up in symbol table
                                                      // Point to struct in symbol table
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

// 6.5.2
postfix_expression:  primary_expression
      |              postfix_expression '[' expression ']'  {
                                                                  // Addition node
                                                                  struct astnode *add = create_binary(BINOP,'+', $1, $3);
                                                                  
                                                                  //Deref node
                                                                  struct astnode *deref = create_unary(DEREF, '*', add);
                                                                  $$ = deref;
                                                            }
      |              function_call 
      |              postfix_expression '.' IDENT           {
                                                                  // Ident node
                                                                  struct astnode *ident = make_ast_node(IDENT_NODE);
                                                                  ident->ident = $3;

                                                                  $$ = create_binary(SELECT, '.', $1, ident);
                                                            
                                                            }
      |              postfix_expression INDSEL IDENT        {
                                                                  // Ident node
                                                                  struct astnode *ident = make_ast_node(IDENT_NODE);
                                                                  ident->ident = $3;

                                                                  // Addition node
                                                                  struct astnode *add = create_binary(BINOP,'+', $1, ident);
                                                                  
                                                                  //Deref node
                                                                  $$ = create_unary(DEREF, '*', add);
                                                            }
      |              postfix_expression PLUSPLUS            {$$ = create_unary(UNARY_OP, PLUSPLUS, $1);}
      |              postfix_expression MINUSMINUS          {$$ = create_unary(UNARY_OP, MINUSMINUS, $1);}
      
      /* |              '(' type_name ')' '{' initializer_list '}'
      |              '(' type_name ')' '{' initializer_list ',' '}' */
;

function_call:       postfix_expression '(' function_arguments ')'      {$$ = create_fn_node($1 , $3);}

function_arguments:  /*EMPTY*/                                            
      |              assignment_expression                              {$$ = create_ll_node($1);}//make linked list, return head
      |              function_arguments ',' assignment_expression       {push_ll($1, $3); $$ = $1;}//add to linked list in front, return head                 

// 6.5.3
unary_expression:    postfix_expression                       
      |              PLUSPLUS unary_expression              {
                                                                  //Make dummy node with the number 1 in it
                                                                  struct astnode *one = make_ast_node(NUM);
                                                                  one->num.type = I; 
                                                                  one->num.integer = 1;

                                                                  // Set up binary node for ++expr
                                                                  $$ = create_binary(ASSIGNMENT_COMPOUND, PLUSEQ, $2, one);
                                                            }
      |              MINUSMINUS unary_expression            {
                                                                  // Set up unary node for --expr

                                                                  //Make dummy node with the number 1 in it
                                                                  struct astnode *one = make_ast_node(NUM); 
                                                                  one->num.type = I; 
                                                                  one->num.integer = 1;

                                                                  $$ = create_binary(ASSIGNMENT_COMPOUND, MINUSEQ, $2, one); 
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
      |              SIZEOF '(' unary_expression ')'        {$$= create_unary(SIZEOF_OP, SIZEOF, $3);}
      |              SIZEOF '(' type_name ')'
;

unary_operator:      '&'            {$$ = '&';}
      |              '*'            {$$ = '*';}
      |              '+'            {$$ = '+';}
      |              '-'            {$$ = '-';}
      |              '~'            {$$ = '~';}
      |              '!'            {$$ = '!';}        
;

// 6.5.4
cast_expression:  unary_expression
      |           '(' type_name ')' cast_expression
;

// 6.5.5
multiplicative_expression: cast_expression
      |              multiplicative_expression '*' cast_expression      {$$ = create_binary(BINOP, '*', $1, $3);}
      |              multiplicative_expression '/' cast_expression      {$$ = create_binary(BINOP, '/', $1, $3);}
      |              multiplicative_expression '%' cast_expression      {$$ = create_binary(BINOP, '%', $1, $3);}
;

// 6.5.6
additive_expression: multiplicative_expression
      |              additive_expression '+' multiplicative_expression  {$$ = create_binary(BINOP, '+', $1, $3);}    
      |              additive_expression '-' multiplicative_expression  {$$ = create_binary(BINOP, '-', $1, $3);}
;

// 6.5.7
shift_expression:    additive_expression
      |              shift_expression SHL additive_expression           {$$ = create_binary(ASSIGNMENT_COMPOUND, SHL, $1, $3);}
      |              shift_expression SHR additive_expression           {$$ = create_binary(ASSIGNMENT_COMPOUND, SHR, $1, $3);}
;

// 6.5.8
relational_expression: shift_expression
      |              relational_expression '<' shift_expression         {$$ = create_binary(COMP_OP, '<', $1, $3);}
      |              relational_expression '>' shift_expression         {$$ = create_binary(COMP_OP, '>', $1, $3);}
      |              relational_expression LTEQ shift_expression        {$$ = create_binary(COMP_OP, LTEQ, $1, $3);}
      |              relational_expression GTEQ shift_expression        {$$ = create_binary(COMP_OP, GTEQ, $1, $3);}
;  

// 6.5.9
equality_expression: relational_expression
      |              equality_expression EQEQ relational_expression     {$$ = create_binary(COMP_OP, EQEQ, $1, $3);}
      |              equality_expression NOTEQ relational_expression    {$$ = create_binary(COMP_OP, NOTEQ, $1, $3);}   
;

// 6.5.10
AND_expression:      equality_expression
      |              AND_expression '&' equality_expression                                     {$$ = create_binary(LOGICAL_OP, '&', $1, $3);}
;

// 6.5.11
exclusive_OR_expression: AND_expression
      |              exclusive_OR_expression '^' AND_expression                                 {$$ = create_binary(LOGICAL_OP, '^', $1, $3);}
;

// 6.5.12
inclusive_OR_expression: exclusive_OR_expression
      |              inclusive_OR_expression '|' exclusive_OR_expression                        {$$ = create_binary(LOGICAL_OP, '|', $1, $3);}
;

// 6.5.13
logical_AND_expression: inclusive_OR_expression
      |              logical_AND_expression LOGAND inclusive_OR_expression                      {$$ = create_binary(LOGICAL_OP, LOGAND, $1, $3);}
;

// 6.5.14
logical_OR_expression:  logical_AND_expression
      |              logical_OR_expression LOGOR logical_AND_expression                         {$$ = create_binary(LOGICAL_OP, LOGOR, $1, $3);}
;

// 6.5.15
conditional_expression: logical_OR_expression                                        
      |              logical_OR_expression '?' expression ':' conditional_expression            {$$ = create_ternary(TERNARY_OP, $1, $3, $5);}
;

// 6.5.16
assignment_expression:  conditional_expression 
      |              unary_expression assignment_operator assignment_expression                 {
                                                                                                      if ($2 == '=')
                                                                                                            $$ = create_binary(ASSIGNMENT, '=', $1, $3);
                                                                                                      else
                                                                                                            $$ = create_binary(ASSIGNMENT_COMPOUND, $2, $1, $3);

                                                                                                }
;

assignment_operator: '='             {$$ = '=';}
      |              TIMESEQ         {$$ = TIMESEQ;}
      |              DIVEQ           {$$ = DIVEQ;}
      |              MODEQ           {$$ = MODEQ;}
      |              PLUSEQ          {$$ = PLUSEQ;}
      |              MINUSEQ         {$$ = MINUSEQ;}
      |              SHLEQ           {$$ = SHLEQ;}
      |              SHREQ           {$$ = SHREQ;}
      |              ANDEQ           {$$ = ANDEQ;}
      |              XOREQ           {$$ = XOREQ;}
      |              OREQ            {$$ = OREQ;}
;


// 6.5.17
expression:          assignment_expression                              
      |              expression ',' assignment_expression               {$$ = create_binary(BINOP, ',', $1, $3);}
;


/* expression_list:     expression ';'                                     {print_ast($1, 0);}
      |              expression_list expression ';'                     {print_ast($2, 0);}
; */
// 6.6
constant_expression: conditional_expression
;
%%


void yyerror(const char* msg) {
      fprintf(stderr, "ERROR: %s on line %d\n", msg, line_num);
}


int main(){
   yyparse();
}