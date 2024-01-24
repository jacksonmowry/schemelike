#include "parse.h"
#include "lex.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ast_node ast_node_init() {
  return (struct ast_node){
      .type = list_t,
      .child = (struct ast_arr){
          .child_ast = calloc(sizeof(ast_node), 4), .size = 0, .cap = 4}};
}

void ast_node_free(ast_node *a) {
  if (a->type == list_t) {
    for (int i = 0; i < a->child.size; i++) {
      ast_node_free(&a->child.child_ast[i]);
    }
    free(a->child.child_ast);
  }
}

void ast_print(ast_node node) {
  if (node.type == literal_t) {
    switch (node.lit_t) {
    case integer_t:
      printf("%ld", node.value.integer);
      return;
    case floating_t:
      printf("%f", node.value.floating);
      return;
    case bool_t:
      printf("%s", node.value.boolean ? "true" : "false");
      return;
    case string_t:
      printf("\"%s\"", node.value.string);
      return;
    case ident_t:
      printf("%s", node.value.ident);
      return;
    default:
      fprintf(stderr, "Unreachable\n");
      exit(1);
    }
  }

  printf("(");
  for (int i = 0; i < node.child.size; i++) {
    ast_print(node.child.child_ast[i]);
    if (i != node.child.size - 1) {
      printf(" ");
    }
  }
  printf(")");
  return;
}

void ast_node_pb(ast_node *outer, ast_node child) {
  assert(outer->type == list_t);
  if (outer->child.size + 1 >= outer->child.cap) {
    outer->child.cap *= 2;
    outer->child.child_ast = reallocarray(outer->child.child_ast,
                                          outer->child.cap, sizeof(ast_node));
  }
  outer->child.child_ast[outer->child.size++] = child;
}

ast_node parse(token_arr tokens, int *index) {
  ast_node ast = ast_node_init();
  token t = tokens.tokens[*index];
  if (t.type != syntax_type || strcmp(t.value, "(")) {
    fprintf(stderr, "Error parsing, must start with `(`\n");
    exit(EXIT_FAILURE);
  }
  (*index)++; // skip past `(` token

  while (*index < tokens.size) {
    t = tokens.tokens[*index];
    if (t.type == syntax_type && !strcmp(t.value, "(")) {
      // recursive parse
      ast_node child = parse(tokens, index);
      ast_node_pb(&ast, child);
      continue;
    }

    if (t.type == syntax_type && !strcmp(t.value, ")")) {
      (*index)++; // skip past `)`
      return ast;
    }

    ast_node literal = {.type = literal_t};
    switch (t.type) {
    case integer_type:
      literal.lit_t = integer_t;
      literal.value.integer = atoi(t.value);
      break;
    case floating_type:
      literal.lit_t = floating_t;
      literal.value.floating = atof(t.value);
      break;
    case bool_type:
      literal.lit_t = bool_t;
      literal.value.boolean = !strncmp(t.value, "true", 4);
      break;
    case string_type:
      literal.lit_t = string_t;
      literal.value.string = t.value;
      break;
    case ident_type:
      literal.lit_t = ident_t;
      literal.value.ident = t.value;
      break;
    default:
      fprintf(stderr, "Unreachable, token: %s\n", t.value);
      exit(EXIT_FAILURE);
    }
    ast_node_pb(&ast, literal);
    (*index)++;
  }

  return ast;
}

/* int main() { */
/*   string program = str_auto("(+ 5 5.5 (var a 5) \"hi mom\" ident)"); */
/*   token_arr ta = lex(&program); */
/*   str_free(&program); */

/*   int cursor = 0; */
/*   ast_node ast = parse(ta, &cursor); */
/*   ast_print(ast); */
/*   printf("\n"); */

/*   ast_node_free(&ast); */
/*   ta_free(&ta); */
/* } */
