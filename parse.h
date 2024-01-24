#ifndef PARSE_H_
#define PARSE_H_
#include "lex.h"
#include <stdbool.h>

typedef enum ast_type {
  literal_t,
  list_t,
  tombstone_t,
} ast_type;

typedef enum literal_type {
  integer_t,
  floating_t,
  bool_t,
  string_t,
  ident_t
} literal_type;

typedef union literal_value {
  char *string;
  char *ident;
  int64_t integer;
  double floating;
  bool boolean;
} literal_value;

typedef struct ast_node {
  ast_type type;
  struct ast_arr {
    struct ast_node *child_ast;
    int size;
    int cap;
  } child;
  literal_type lit_t;
  literal_value value;
} ast_node;

ast_node ast_node_int();
void ast_node_free(ast_node*);
void ast_print(ast_node);
void ast_node_pb(ast_node*, ast_node);
ast_node parse(token_arr, int*);

#endif // PARSE_H_
