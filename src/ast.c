// Helper file for functions related to bison parser
#include "parser.h"


// ast node helper function
struct astnode * make_ast_node(int type) {
      struct astnode *node = (struct astnode *)malloc(sizeof(struct astnode));
      node->type = type; 
      return node;
}

// Helper function to create unary node
struct astnode * create_unary(int op_type, int op, struct astnode *expr){
      // Set up type vars
      struct astnode * node = make_ast_node(UNARY_NODE);
      node->unary.operator_type = op_type; 
      node->unary.operator = op;

      // Assign child
      node->unary.expr = expr;
      return node;
}

// Helper function to create binary node
struct astnode * create_binary(int op_type, int op, struct astnode *left, struct astnode *right){
      // Assign node and operator type
      struct astnode * node = make_ast_node(BINARY_NODE);
      node->binary.operator_type = op_type;
      node->binary.operator = op;
      
      // Assign children
      node->binary.left = left;
      node->binary.right = right;
      return node; 
}

// Helper function to create ternary node
void create_ternary(){
      
}