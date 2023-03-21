%code requires
{
   #include "lexer.h" 
   #include "parser.h"

   // Function prototypes
   void yyerror(const char* msg);
   int yylex();
   struct astnode * top;
   struct astnode * tail;  
   struct astnode * tmp_ptr_head;  
   struct astnode * tmp_ptr_tail;  

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
%type <astnode_p> assignment_expression expression function_call direct_declarator declarator // expression_list
%type <astnode_p> declaration_specifiers type_specifier declaration init_declarator init_declarator_list pointer
/* %type <ll_p> function_arguments */
%type <ll_p> function_arguments


%%
// Top Level (From Hak)
declaration_or_fndef_list:    declaration_or_fndef
      |                       declaration_or_fndef_list declaration_or_fndef              //{print_symbol_table();}

declaration_or_fndef:         declaration                                                 {
                                                                                                print_ast(top, 0);
                                                                                                print_ast(tail, 0);
                                                                                          }
      |                       function_definition
;

function_definition: declaration_specifiers declarator compound_statement                 
;

statement:        compound_statement
      |           expression ';'                                                          {print_ast($1, 0);}
;

compound_statement: '{' decl_or_stmt_list '}'                                            
;

decl_or_stmt_list:      decl_or_stmt 
      |                 decl_or_stmt_list decl_or_stmt
;

decl_or_stmt:     declaration                      
      |           statement
;


// Expressions 6.5.1
primary_expression:   IDENT                                 {
                                                                  // Identifier node
                                                                  // Look up in symbol table
                                                                  // Point to struct in symbol table
                                                                  struct astnode *node = make_ast_node(IDENT_NODE);
                                                                  node->ident = yylval.ident;
                                                                  $$ = node; 
                                                            }       
      |               NUMBER                                {
                                                               // Number node (struct stays the same)
                                                               struct astnode *node = make_ast_node(NUM);
                                                               node->num = yylval.num;
                                                               $$ = node; 
                                                            } 
      |               STRING                                {
                                                               // String node
                                                               struct astnode *node = make_ast_node(STR_LIT);
                                                               node->str_lit.content = yylval.str.content;
                                                               node->str_lit.length = yylval.str.length;
                                                               $$ = node; 
                                                            } 
      |               CHARLIT                               {
                                                               // Charlit node
                                                               struct astnode *node = make_ast_node(CHAR_LIT);
                                                               node->char_lit = yylval.charlit;
                                                               $$ = node; 
                                                            } 
      |               '(' expression ')'                    {
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
      |              multiplicative_expression '*' cast_expression                              {$$ = create_binary(BINOP, '*', $1, $3);}
      |              multiplicative_expression '/' cast_expression                              {$$ = create_binary(BINOP, '/', $1, $3);}
      |              multiplicative_expression '%' cast_expression                              {$$ = create_binary(BINOP, '%', $1, $3);}
;                       

// 6.5.6                      
additive_expression: multiplicative_expression                    
      |              additive_expression '+' multiplicative_expression                          {$$ = create_binary(BINOP, '+', $1, $3);}    
      |              additive_expression '-' multiplicative_expression                          {$$ = create_binary(BINOP, '-', $1, $3);}
;                       

// 6.5.7                      
shift_expression:    additive_expression                    
      |              shift_expression SHL additive_expression                                   {$$ = create_binary(ASSIGNMENT_COMPOUND, SHL, $1, $3);}
      |              shift_expression SHR additive_expression                                   {$$ = create_binary(ASSIGNMENT_COMPOUND, SHR, $1, $3);}
;                       

// 6.5.8                      
relational_expression: shift_expression                     
      |              relational_expression '<' shift_expression                                 {$$ = create_binary(COMP_OP, '<', $1, $3);}
      |              relational_expression '>' shift_expression                                 {$$ = create_binary(COMP_OP, '>', $1, $3);}
      |              relational_expression LTEQ shift_expression                                {$$ = create_binary(COMP_OP, LTEQ, $1, $3);}
      |              relational_expression GTEQ shift_expression                                {$$ = create_binary(COMP_OP, GTEQ, $1, $3);}
;                       

// 6.5.9                      
equality_expression: relational_expression                        
      |              equality_expression EQEQ relational_expression                             {$$ = create_binary(COMP_OP, EQEQ, $1, $3);}
      |              equality_expression NOTEQ relational_expression                            {$$ = create_binary(COMP_OP, NOTEQ, $1, $3);}   
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


// Declarations 6.7
declaration:            declaration_specifiers init_declarator_list ';'                         {
                                                                                                      $$ = $2;
                                                                                                      $2->t_node.next_type = $1; 

                                                                                                      // Update tail
                                                                                                      tail = $1; 
                                                                                                      $$ = $1;
                                                                                                }
      |                 declaration_specifiers  ';'                                             
;

declaration_specifiers: storage_class_specifier declaration_specifiers
      |                 storage_class_specifier
      |                 type_specifier declaration_specifiers                             
      |                 type_specifier                                                    {$$ = $1;}
      |                 type_qualifier declaration_specifiers
      |                 type_qualifier
      |                 function_specifier declaration_specifiers
      |                 function_specifier
;

// for now require declarations to be on separate lines...
init_declarator_list:   init_declarator                                                   //{printf("name: %s, node type: %d\n", $1->ident, $1->type);}
      |                 init_declarator_list ',' init_declarator                          //{printf("name: %s, node type: %d\n", $1->ident, $1->type);}
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
type_specifier:   VOID                                                              {$$ = create_scalar_node(V);}
      |           CHAR                                                              {$$ = create_scalar_node(C);}
      |           SHORT                                                             {$$ = create_scalar_node(SH);}
      |           INT                                                               {$$ = create_scalar_node(I);}
      |           LONG                                                              {$$ = create_scalar_node(L);}
      |           FLOAT                                                             {$$ = create_scalar_node(F);}
      |           DOUBLE                                                            {$$ = create_scalar_node(D);}
      |           SIGNED                                                            {$$ = create_scalar_node(S);}
      |           UNSIGNED                                                          {$$ = create_scalar_node(U);}
      |           _BOOL                                                             {yyerror("Unimplemented");}
      |           _COMPLEX                                                          {yyerror("Unimplemented");}
      |           struct_or_union_specifier                                         {yyerror("Unimplemented");}
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
declarator:             pointer direct_declarator                                   {
                                                                                          $2->t_node.next_type = $1;
                                                                                          struct astnode *ptr_tail = $1;
                                                                                          while(ptr_tail->t_node.next_type != NULL){
                                                                                                ptr_tail = ptr_tail->t_node.next_type; 
                                                                                          } 
                                                                                          // Update tail node (end of declarator now)
                                                                                          tail = ptr_tail; 
                                                                                          $$ = tail;
                                                                                    }
      |                 direct_declarator                                           //{print_ast($1, 0);}
;

// Assume array declarations are either empty or constant
// Assume function declarations take unknown args
direct_declarator:      IDENT                                                       {
                                                                                          // Identifier node
                                                                                          struct astnode * node = make_ast_node(IDENT_TYPE);
                                                                                          node->t_node.ident = $1;

                                                                                          $$ = node;
                                                                                          // Set top and tail nodes
                                                                                          top = $$; 
                                                                                          tail = $$;  
                                                                                    }           
      |                 '(' declarator ')'                                          {$$ = $2;}
      |                 direct_declarator '['  ']'                                  {
                                                                                          // Add array type node
                                                                                          struct astnode * tmp = push_next_type(ARRAY_TYPE, $1, NULL);
                                                                                          tmp->t_node.array_node.size = -1;         // Make size -1 for now to distinguish between known and unknown sizes

                                                                                          // Update tail node to be latest added node, return end of the declarator (ident)
                                                                                          tail = tmp;
                                                                                          $$ = tmp; 
                                                                                    }
      |                 direct_declarator   '[' NUMBER ']'                          {
                                                                                          if (yylval.num.type >= 8){
                                                                                                yyerror("Non-integer size provided in array declaration"); exit(1);
                                                                                          }
                                                                                          // Add array type node
                                                                                          struct astnode * tmp = push_next_type(ARRAY_TYPE, $1, NULL);
                                                                                          tmp->t_node.array_node.size = yylval.num.integer;

                                                                                          // Update tail node to be latest added node, return end of the declarator (ident)
                                                                                          tail = tmp;
                                                                                          $$ = tmp; 
                                                                                    }
      |                 direct_declarator '(' ')'                                    
;

pointer:                '*' type_qualifier_list                                     //{$$ = create_pointer_node(tail); tail = $$;}
      |                 '*'                                                         {     
                                                                                          struct astnode *tmp_ptr = make_ast_node(POINTER_TYPE);
                                                                                          $$ = tmp_ptr;

                                                                                    }
      |                 '*' type_qualifier_list pointer
      |                 '*' pointer                                                 {
                                                                                          // Nested pointer
                                                                                          push_next_type(POINTER_TYPE, $2, NULL);
                                                                                          $$ = $2;
                                                                                    }
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

/* identifier_list:        IDENT
      |                 identifier_list ',' IDENT
; */

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





%%

// Global symbol table
struct scope curr_scope = {.head = NULL, .outer = NULL}; 

void yyerror(const char* msg) {
      fprintf(stderr, "ERROR: %s on line %d\n", msg, line_num);
}


int main(){
      /* struct astnode * tmp = make_ast_node(POINTER_TYPE);
      struct astnode * tmp2 = make_ast_node(POINTER_TYPE);
      struct astnode *node = make_ast_node(IDENT_NODE);
      node->ident = "TMP";
      tmp->t_node.next_type = tmp2; 
      tmp2->t_node.next_type = node; 

      print_ast(tmp, 0); */
      yyparse();
}