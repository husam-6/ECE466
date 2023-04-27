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
      fprintf(stderr, "%serror%s: In file %s:%d: %s\n", RED, RESET, file_name, line_num, msg);
}

// Default filename to stdin
char * file_name = "<stdin>";

// Function counter, block counter, register counter
int func_counter, bb_counter, register_counter; 

// Quad and Basic Block linked list heads
struct basic_block * block_head;  
struct basic_block * curr_block;

int main(){
      // Initialize global scope symbol table
      curr_scope = make_new_scope(GLOBAL_SCOPE);
      curr_scope->head = NULL; 
      curr_scope->outer = NULL;

      // Initalize quad and basic block head
      block_head = NULL;
      curr_block = NULL;

      yyparse();
      // print_symbol_table(1);
}