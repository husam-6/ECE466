%code requires
{
   #include "lexer.h" 
   #include "parser.h"
   #include "sym.h"
   #include "type.h"

   // Function prototypes
   void yyerror(const char* msg);
   int yylex();
   struct astnode * asts;
}

/* To resolve if statement conflicts (recall lecture notes 2) */
%left IF
%left ELSE

/* Tokens */
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
      struct type_node *type_p;
      struct top_tail *tt; 
      struct decl_list * dl;
};


/* Tokens */
%type <ident> IDENT 
%type <token> LTEQ GTEQ EQEQ NOTEQ LOGAND LOGOR TIMESEQ DIVEQ MODEQ PLUSEQ
%type <token> MINUSEQ SHLEQ SHREQ ANDEQ XOREQ OREQ assignment_operator
%type <op_type> unary_operator

/* Expressions */
%type <astnode_p> primary_expression postfix_expression unary_expression cast_expression multiplicative_expression
%type <astnode_p> additive_expression shift_expression relational_expression equality_expression AND_expression
%type <astnode_p> exclusive_OR_expression inclusive_OR_expression logical_AND_expression logical_OR_expression conditional_expression
%type <astnode_p> assignment_expression expression function_call expression_statement

/* Declarations */
%type <tt> declarator declaration_specifiers declaration init_declarator
%type <tt> type_specifier pointer direct_declarator parameter_declaration parameter_list
/* %type <ll_p> function_arguments */
%type <tt> storage_class_specifier function_specifier type_qualifier struct_or_union struct_or_union_specifier 
%type <tt> specifier_qualifier_list struct_declarator_list struct_declarator
%type <tt> type_name abstract_declarator direct_abstract_declarator parameter_type_list
%type <ll_p> function_arguments
%type <dl> init_declarator_list; 

/* For loops, while loops, control statements */
%type <astnode_p> decl_or_stmt statement iteration_statement compound_statement
%type <ll_p> decl_or_stmt_list 


%%

// Expressions 6.5.1
primary_expression:   IDENT                                             {
                                                                              // Identifier node
                                                                              struct astnode *node = make_ast_node(IDENT_NODE);
                                                                              node->ident.name = $1;
                                                                              
                                                                              // Look up in symbol table
                                                                              resolve_identifier($1, VAR_S, node);
                                                                              $$ = node;
                                                                        }       
      |               NUMBER                                            {
                                                                           // Number node (struct stays the same)
                                                                           struct astnode *node = make_ast_node(NUM);
                                                                           node->num = yylval.num;
                                                                           $$ = node; 
                                                                        } 
      |               STRING                                            {
                                                                           // String node
                                                                           struct astnode *node = make_ast_node(STR_LIT);
                                                                           node->str_lit.content = yylval.str.content;
                                                                           node->str_lit.length = yylval.str.length;
                                                                           $$ = node; 
                                                                        } 
      |               CHARLIT                                           {
                                                                           // Charlit node
                                                                           struct astnode *node = make_ast_node(CHAR_LIT);
                                                                           node->char_lit = yylval.charlit;
                                                                           $$ = node; 
                                                                        } 
      |               '(' expression ')'                                {$$ = $2;}
;

// 6.5.2
postfix_expression:  primary_expression
      |              postfix_expression '[' expression ']'              {
                                                                              // Addition node
                                                                              struct astnode *add = create_binary(BINOP,'+', $1, $3);

                                                                              //Deref node
                                                                              struct astnode *deref = create_unary(DEREF, '*', add);
                                                                              $$ = deref;
                                                                        }
      |              function_call        
      |              postfix_expression '.' IDENT                       {
                                                                              // Ident node
                                                                              struct astnode *ident = make_ast_node(IDENT_NODE);
                                                                              ident->ident.name = $3;

                                                                              $$ = create_binary(SELECT, '.', $1, ident);

                                                                        }
      |              postfix_expression INDSEL IDENT                    {     
                                                                              // Ident node
                                                                              struct astnode *ident = make_ast_node(IDENT_NODE);
                                                                              ident->ident.name = $3;

                                                                              // Addition node
                                                                              struct astnode *add = create_binary(BINOP,'+', $1, ident);

                                                                              //Deref node
                                                                              $$ = create_unary(DEREF, '*', add);
                                                                        }
      |              postfix_expression PLUSPLUS                        {$$ = create_unary(UNARY_OP, PLUSPLUS, $1);}
      |              postfix_expression MINUSMINUS                      {$$ = create_unary(UNARY_OP, MINUSMINUS, $1);}
      
      /* |              '(' type_name ')' '{' initializer_list '}'
      |              '(' type_name ')' '{' initializer_list ',' '}' */
;

function_call:       postfix_expression '(' function_arguments ')'      {$$ = create_fn_node($1 , $3);}

function_arguments:  /*EMPTY*/                                          {$$ = create_ll_node(NULL);}
      |              assignment_expression                              {$$ = create_ll_node($1);}//make linked list, return head
      |              function_arguments ',' assignment_expression       {push_ll($1, $3); $$ = $1;}//add to linked list in front, return head                 

// 6.5.3
unary_expression:    postfix_expression                       
      |              PLUSPLUS unary_expression                          {
                                                                              //Make dummy node with the number 1 in it
                                                                              struct astnode *one = make_ast_node(NUM);
                                                                              one->num.type = I; 
                                                                              one->num.integer = 1;

                                                                              // Set up binary node for ++expr
                                                                              $$ = create_binary(ASSIGNMENT_COMPOUND, PLUSEQ, $2, one);
                                                                        }
      |              MINUSMINUS unary_expression                        {
                                                                              // Set up unary node for --expr

                                                                              //Make dummy node with the number 1 in it
                                                                              struct astnode *one = make_ast_node(NUM); 
                                                                              one->num.type = I; 
                                                                              one->num.integer = 1;

                                                                              $$ = create_binary(ASSIGNMENT_COMPOUND, MINUSEQ, $2, one); 
                                                                        }

      |              unary_operator cast_expression                     {
                                                                              struct astnode *un_op;
                                                                              //Special Case For Deref
                                                                              if ($1 == '*')
                                                                                    un_op = create_unary(DEREF, $1, $2);
                                                                              else
                                                                                    un_op = create_unary(UNARY_OP, $1, $2);

                                                                              $$ = un_op;            

                                                                        }    
      |              SIZEOF '(' unary_expression ')'                    {$$= create_unary(SIZEOF_OP, SIZEOF, $3);}
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
      |              expression ',' assignment_expression                                 {$$ = create_binary(BINOP, ',', $1, $3);}
;


/* expression_list:     expression ';'                                                       {print_ast($1, 0);}
      |              expression_list expression ';'                                       {print_ast($2, 0);}
; */
// 6.6
constant_expression: conditional_expression
;


// Declarations 6.7
declaration:            declaration_specifiers init_declarator_list ';'                   {
                                                                                                struct decl_list * tmp = $2; 
                                                                                                while(tmp != NULL){
                                                                                                      new_declaration($1, tmp->item, 0);
                                                                                                      tmp = tmp->next_decl; 
                                                                                                }
                                                                                          }
      |                 declaration_specifiers  ';'                                       {
                                                                                                if ($1->top->type == STRUCT_UNION_TYPE)
                                                                                                      add_symbol_entry($1->top->stu_node.ident, $1->top, TAG_S, NON_VAR, DECL);
                                                                                                else{
                                                                                                      yyerror("INVALID DECLARATION");
                                                                                                      exit(2);
                                                                                                }
                                                                                          }              
;

declaration_specifiers: storage_class_specifier declaration_specifiers                    {
                                                                                                if ($2->top->type == S_CLASS){
                                                                                                      yyerror("ONLY ONE STORAGE CLASS SPECIFIER ALLOWED");
                                                                                                      exit(2);
                                                                                                }
                                                                                                // Save scalar value
                                                                                                $2->top->scalar.s_class = $1->top->scalar.s_class;
                                                                                                
                                                                                                $1->tail->next_type = $2->top; 
                                                                                                $1->tail = $2->tail; 
                                                                                                $$ = $1;
                                                                                          }
      |                 storage_class_specifier                                           
      |                 type_specifier declaration_specifiers                             {
                                                                                                $1->tail->next_type = $2->top;
                                                                                                $1->tail = $2->tail;
                                                                                                $$ = $1;
                                                                                          }       
      |                 type_specifier                                                    {$$ = $1;}
      |                 type_qualifier declaration_specifiers                             {$$ = $2;}
      |                 type_qualifier
      |                 function_specifier declaration_specifiers
      |                 function_specifier
;

init_declarator_list:   init_declarator                                                   {$$ = make_decl_list_node($1);}
      |                 init_declarator_list ',' init_declarator                          {
                                                                                                struct decl_list * tmp = make_decl_list_node($3);
                                                                                                tmp->next_decl = $1;
                                                                                                $$ = tmp;
                                                                                          }
;

init_declarator:        declarator
;

// 6.7.1
storage_class_specifier:      //TYPEDEF
                              EXTERN                                                {$$ = create_s_class_node(EXTERN_S);}
      |                       STATIC                                                {$$ = create_s_class_node(STATIC_S);}
      |                       AUTO                                                  {$$ = create_s_class_node(AUTO_S);}
      |                       REGISTER                                              {$$ = create_s_class_node(REGISTER_S);}
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
      |           _BOOL                                                             {yyerror("UNIMPLEMENTED"); exit(2);}
      |           _COMPLEX                                                          {yyerror("UNIMPLEMENTED"); exit(2);}
      |           struct_or_union_specifier
      //|           enum_specifier
      //|           typedef_name
;

struct_or_union_specifier:    struct_or_union IDENT '{'                             {
                                                                                          $1->top->stu_node.ident = $2;
                                                                                          $1->top->stu_node.complete = INCOMPLETE;
                                                                                          if (curr_scope->s_type == PROTOTYPE_SCOPE || curr_scope->s_type == FUNC_SCOPE ||
                                                                                          curr_scope->s_type == BLOCK_SCOPE)
                                                                                                      $1->top->ident.s_class = AUTO_S;
                                                                                          
                                                                                          add_symbol_entry($2, $1->top, TAG_S, NON_VAR, DEF);
                                                                                          $1->top->stu_node.refers_to = curr_scope->head; 

                                                                                          create_new_scope(MEMBER_S);
                                                                                    } 
                              struct_declaration_list '}'                           {
                                                                                          // Mark the struct ndoeo as complete (get just installed symbol from symbol table)
                                                                                          struct astnode_symbol * just_installed; 

                                                                                          // Install as complete
                                                                                          $1->top->stu_node.complete = COMPLETE;
                                                                                          $1->top->stu_node.ident = $2;
                                                                                          
                                                                                          search_all_tabs($2, TAG_S, curr_scope->outer, &just_installed);
                                                                                          
                                                                                          // Update symbol to be complete, save members in mini symbol table
                                                                                          just_installed->type->stu_node.complete = COMPLETE;
                                                                                          just_installed->type->stu_node.mini_head = curr_scope->head; 
                                                                                          close_outer_scope();
                                                                                          $$ = $1;

                                                                                          // print_symbol(just_installed, curr_scope);
                                                                                          // print_symbol_table();
                                                                                    }
      |                       struct_or_union IDENT                                 {
                                                                                          $1->top->stu_node.ident = $2;
                                                                                          $1->top->stu_node.complete = INCOMPLETE;
                                                                                          $$ = $1;

                                                                                          struct astnode_symbol * found; 
                                                                                          int in_table = search_all_tabs($2, TAG_S, curr_scope, &found);
                                                                                          
                                                                                          // If not in table...
                                                                                          if (in_table != 1)
                                                                                                add_symbol_entry($2, $1->top, TAG_S, NON_VAR, DECL);
                                                                                          else
                                                                                                $1->top->stu_node.refers_to = found;            // If it is, point the struct 
                                                                                    } 
      |                       struct_or_union '{' struct_declaration_list '}'       {yyerror("UNIMPLEMENTED"); exit(2);}
;

// 6.7.2.1
struct_or_union:              STRUCT                                                {$$ = create_stu_node(STRUCT_TYPE);}
      |                       UNION                                                 {$$ = create_stu_node(UNION_TYPE);}
;

struct_declaration_list:      struct_declaration
      |                       struct_declaration_list struct_declaration
;

struct_declaration:           specifier_qualifier_list struct_declarator_list ';'   {new_declaration($1, $2, 0);};
;

specifier_qualifier_list:     type_specifier specifier_qualifier_list               {$$ = $1;}       
      |                       type_specifier                                        
      |                       type_qualifier specifier_qualifier_list               {$$ = $1;}
      |                       type_qualifier
;

struct_declarator_list:       struct_declarator
      |                       struct_declarator_list ',' struct_declarator
;

struct_declarator:      declarator
      |                 declarator ':' constant_expression                          {yyerror("Unimplemented"); exit(2);}
      |                 ':' constant_expression                                     {yyerror("Unimplemented"); exit(2);}
;

// 6.7.3
type_qualifier:         CONST                                                       //{yyerror("qualifiers unimplemented");}
      |                 RESTRICT                                                    //{yyerror("qualifiers unimplemented");}
      |                 VOLATILE                                                    //{yyerror("qualifiers unimplemented");}
;

// 6.7.4
function_specifier:     INLINE                                                      {yyerror("inline function specifier unimplemented");}
;

// 6.7.5
declarator:             pointer direct_declarator                                   {
                                                                                          if ($2->tail->type == FUNCTION_TYPE){
                                                                                                $2->tail->func_node.return_type = $1->top;
                                                                                          }
                                                                                          else
                                                                                                $2->tail->next_type = $1->top;
                                                                                          // Update tail node (end of declarator now)
                                                                                          $2->tail = $1->tail; 
                                                                                          $$ = $2;
                                                                                    }
      |                 direct_declarator                                           //{print_ast($1, 0);}
;

// Assume array declarations are either empty or constant
// Assume function declarations take unknown args
direct_declarator:      IDENT                                                       {
                                                                                          // Identifier node
                                                                                          struct type_node * node = make_type_node(IDENT_TYPE);
                                                                                          node->ident.name = $1;

                                                                                          // Save namespace + s_class (change later if applicable)
                                                                                          node->ident.n_space = VAR_S;

                                                                                          // Default to EXTERN - edit later 
                                                                                          node->ident.s_class = EXTERN_S;

                                                                                          if (curr_scope->s_type == FUNC_SCOPE || curr_scope->s_type == PROTOTYPE_SCOPE)
                                                                                                node->ident.s_class = AUTO_S;

                                                                                          // Set top and tail nodes
                                                                                          struct top_tail * tt = make_tt_node(); 
                                                                                          tt->top = node; 
                                                                                          tt->tail = node; 
                                                                                          $$ = tt;
                                                                                    }           
      |                 '(' declarator ')'                                          {$$ = $2;}
      |                 direct_declarator '['  ']'                                  {
                                                                                          // Add array type node
                                                                                          struct type_node * tmp = push_next_type(ARRAY_TYPE, $1->tail, NULL);
                                                                                          tmp->size = -1;         // Make size -1 for now to distinguish between known and unknown sizes
                                                                                          
                                                                                          // Update tail node to be latest added node, return end of the declarator (ident)
                                                                                          $1->tail = tmp;
                                                                                          $$ = $1; 
                                                                                    }
      |                 direct_declarator   '[' NUMBER ']'                          {
                                                                                          if (yylval.num.type >= 8){
                                                                                                yyerror("NON-INTEGER SIZE PROVIDED IN ARRAY DECLARATION");
                                                                                                exit(2);
                                                                                          }
                                                                                          // Add array type node
                                                                                          struct type_node * tmp = push_next_type(ARRAY_TYPE, $1->tail, NULL);
                                                                                          tmp->size = yylval.num.integer;

                                                                                          // Update tail node to be latest added node, return end of the declarator (ident)
                                                                                          $1->tail = tmp;
                                                                                          $$ = $1; 
                                                                                    }
      |                 direct_declarator '(' ')'                                   {$$ = create_function_node($1);}
      |                 direct_declarator   '('                                     {create_new_scope(PROTOTYPE_SCOPE);}
                        parameter_type_list ')'                                     {
                                                                                          $$ = create_function_node($1);
                                                                                          // if ($$->top->type != )
                                                                                          while (curr_scope->outer->s_type == PROTOTYPE_SCOPE)
                                                                                                close_outer_scope(); 
                                                                                    }
;

pointer:                '*' type_qualifier_list                                     {$$ = init_tt_node(POINTER_TYPE);}
      |                 '*'                                                         {$$ = init_tt_node(POINTER_TYPE);}
      |                 '*' type_qualifier_list pointer                             {
                                                                                          // Nested pointers
                                                                                          struct type_node * tmp = push_next_type(POINTER_TYPE, $3->tail , NULL);
                                                                                          $3->tail = tmp; 
                                                                                          $$ = $3;
                                                                                    }
      |                 '*' pointer                                                 {
                                                                                          // Nested pointers
                                                                                          struct type_node * tmp = push_next_type(POINTER_TYPE, $2->tail , NULL);
                                                                                          $2->tail = tmp; 
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

// Assume function declarations only take unknonw arguments, this just accepts it and allows for function definitions
parameter_declaration:  declaration_specifiers declarator                           {
                                                                                          if ($2->top->ident.n_space == FUNC_S){
                                                                                                yyerror("CANNOT ACCEPT A FUNCTION AS AN ARGUMENT");
                                                                                                exit(2);
                                                                                          }
                                                                                          new_declaration($1, $2, 1);
                                                                                    }
      |                 declaration_specifiers abstract_declarator                  {
                                                                                          // Create dummy ident node
                                                                                          struct top_tail * node = init_tt_node(IDENT_TYPE);
                                                                                          node->top->ident.name = "1UNDEF";
                                                                                          node->top->ident.s_class = AUTO_S;
                                                                                          node->top->ident.n_space = VAR_S; 
                                                                                          node->top->next_type = $2->top; 
                                                                                          node->tail = $2->tail; 
                                                                                          
                                                                                          new_declaration($1, node, 1);

                                                                                    }
      |                 declaration_specifiers                                      {
                                                                                          // Create dummy ident node
                                                                                          struct top_tail * node = init_tt_node(IDENT_TYPE);
                                                                                          node->top->ident.name = "1UNDEF";
                                                                                          node->top->ident.s_class = AUTO_S;
                                                                                          node->top->ident.n_space = VAR_S; 
                                                                                          
                                                                                          new_declaration($1, node, 1);
                                                                                          
                                                                                    }
;

/* identifier_list:        IDENT
      |                 identifier_list ',' IDENT
; */

// 6.7.6
type_name:              specifier_qualifier_list abstract_declarator                            {$$ = $2;}
      |                 specifier_qualifier_list
;

abstract_declarator:    pointer                                                                 
      |                 pointer direct_abstract_declarator                                      {
                                                                                                      if ($2->tail->type == FUNCTION_TYPE){
                                                                                                            $2->tail->func_node.return_type = $1->top;
                                                                                                      }
                                                                                                      else
                                                                                                            $2->tail->next_type = $1->top;
                                                                                                      
                                                                                                      // Update tail node (end of declarator now)
                                                                                                      $2->tail = $1->tail; 
                                                                                                      $$ = $2;
                                                                                                }
      |                 direct_abstract_declarator
;

direct_abstract_declarator:   '(' abstract_declarator ')'                                       {$$ = $2;}
      |                       direct_abstract_declarator '['  ']'                               {
                                                                                                      // Add array type node
                                                                                                      struct type_node * tmp = push_next_type(ARRAY_TYPE, $1->tail, NULL);
                                                                                                      tmp->size = -1;         // Make size -1 for now to distinguish between known and unknown sizes
                                                                                                      
                                                                                                      // Update tail node to be latest added node, return end of the declarator (ident)
                                                                                                      $1->tail = tmp;
                                                                                                      $$ = $1; 
                                                                                                }
      |                       '['  ']'                                                          {$$ = init_tt_node(ARRAY_TYPE); $$->top->size = -1;}
      |                       direct_abstract_declarator '('                                    {create_new_scope(PROTOTYPE_SCOPE);} parameter_type_list ')'          
                                                                                                {
                                                                                                      $$ = create_function_node($1);
                                                                                                      // if ($$->top->type != )
                                                                                                      while (curr_scope->outer->s_type == PROTOTYPE_SCOPE)
                                                                                                            close_outer_scope(); 
                                                                                                }
      |                       direct_abstract_declarator '('  ')'                               {$$ = create_function_node($1);}
      |                       '(' parameter_type_list ')'                                       {$$ = $2;}
      |                       '('  ')'                                                          {$$ = init_tt_node(FUNCTION_TYPE);}
;

/* block_item:             declaration
      |                 statement
; */



// 6.8
statement:              compound_statement                                    {$$ = $1; close_outer_scope();}                              
      |                 expression_statement                                  //{print_ast($1, 0);}
      |                 labeled_statement
      |                 selection_statement
      |                 iteration_statement
      |                 jump_statement      
;

// 6.8.1
labeled_statement:      IDENT ':'         {struct type_node * tt = make_type_node(LABEL_TYPE); add_symbol_entry($1, tt, LABEL_S, NON_VAR, DEF);}   statement
      |                 CASE constant_expression ':' statement
      |                 DEFAULT ':' statement


// 6.8.2
compound_statement:     '{'   {create_new_scope();}   decl_or_stmt_list '}'   {$$ = make_ast_node(COMPOUND); $$->ds_list = $3;} 
      |                 '{'   '}'                                             {$$ = NULL;}
;


// 6.8.3
expression_statement:   expression ';'
      |                 ';'

// 6.8.4
selection_statement:    IF '(' expression ')' statement                       %prec IF
      |                 IF '(' expression ')' statement ELSE statement        %prec ELSE
      |                 SWITCH '('  expression ')' statement


// 6.8.5
iteration_statement:    WHILE '(' expression ')' statement                                        {$$ = create_while_loop($5, $3, 0);}
      |                 DO statement WHILE '(' expression ')'                                     {$$ = create_while_loop($2, $5, 1);}
      |                 FOR '(' expression ';' expression ';' expression ')' statement            {$$ = create_for_loop($3, $5, $9, $7);}
      |                 FOR '(' ';' expression ';' expression ')' statement                       {$$ = create_for_loop(NULL, $4, $8, $6);}
      |                 FOR '(' ';' ';' expression ')' statement                                  {$$ = create_for_loop(NULL, NULL, $7, $5);}
      |                 FOR '(' ';' ';' ')' statement                                             {$$ = create_for_loop(NULL, NULL, $6, NULL);}
      |                 FOR '(' expression ';' ';' expression ')' statement                       {$$ = create_for_loop($3, NULL, $8, $6);}
      |                 FOR '(' expression ';' ';' ')' statement                                  {$$ = create_for_loop($3, NULL, $7, NULL);}
      |                 FOR '(' expression ';' expression ';' ')' statement                       {$$ = create_for_loop($3, $5, $8, NULL);}
      /* |                 FOR '(' declaration expression ';' expression ')' statement */


// 6.8.6
jump_statement:         GOTO IDENT ';'
      |                 CONTINUE ';'
      |                 BREAK ';'
      |                 RETURN expression ';'
      |                 RETURN ';'


// Top Level (From Hak)
declaration_or_fndef_list:    declaration_or_fndef                                        //{print_symbol_table();}
      |                       declaration_or_fndef_list declaration_or_fndef              //{print_symbol_table();}             // For debugging printing symbol table at top level

declaration_or_fndef:         declaration                                                 
      |                       function_definition
;

// Declaration Specifier int, extern int
// Declarator is the ident and any pointers/array info
// Compound statement is everything in the brackets
function_definition:    declaration_specifiers declarator                     {
                                                                                    if ($2->top == NULL || $2->top->next_type == NULL || $2->top->next_type->type != FUNCTION_TYPE) {
                                                                                          yyerror("INVALID FUNCTION DEFINITION");
                                                                                          exit(2);
                                                                                    };
                                                                                    new_function_defs($1, $2);
                                                                              }
                        compound_statement                                    {
                                                                                    print_symbol_table();
                                                                                    close_outer_scope(); 
                                                                                    dump_ast($4->ds_list, 0);
                                                                              }// Dump ast list
;                 

decl_or_stmt_list:      decl_or_stmt                                          {$$ = create_ll_node($1);}
      |                 decl_or_stmt_list decl_or_stmt                        {$$ = $1; push_ll($1, $2);}
;

decl_or_stmt:     declaration                                                  {$$ = make_ast_node(DECLARATION);} // Dummy astnode for declarations
      |           statement
;

%%