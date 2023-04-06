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
      fprintf(stderr, "ERROR IN FILE %s:%d: %s\n", file_name, line_num, msg);
}


int main(){
      // Initialize global scope symbol table
      curr_scope = make_new_scope(GLOBAL_SCOPE);
      curr_scope->head = NULL; 
      curr_scope->outer = NULL;

      yyparse();
      // print_symbol_table(1);
}