#include "lexer.h" 
#include "parser.h"
#include "parser.tab.h"
#include "sym.h"
#include "type.h"

// Global symbol table
struct scope * curr_scope; 

// tmp storage class
enum storage_class tmp_s_class = -1; 

void yyerror(const char* msg) {
      fprintf(stderr, "ERROR ON LINE %d: %s\n", line_num, msg);
}


int main(){
      // Initialize global scope symbol table
      curr_scope = make_new_scope(GLOBAL_SCOPE);
      curr_scope->head = NULL; 
      curr_scope->outer = NULL;

      /* struct astnode * tmp = make_ast_node(POINTER_TYPE);
      struct astnode * tmp2 = make_ast_node(POINTER_TYPE);
      struct astnode *node = make_ast_node(IDENT_NODE);
      node->ident = "TMP";
      tmp->t_node.next_type = tmp2; 
      tmp2->t_node.next_type = node; 

      print_ast(tmp, 0); */
      yyparse();
}