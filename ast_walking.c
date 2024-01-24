#include "ast_walking.h"
#include "hashmap.h"
#include "lex.h"
#include "parse.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *builtins[] = {"+", "var", "begin", "<", "if"};

ast_node plus(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  // Ignore the first node in arr as it is the ident '+'
  int64_t accumulator = 0;
  for (int i = 1; i < ast.size; i++) {
    ast_node v = auto_ast_walk(ast.child_ast[i], ctx);
    accumulator += v.value.integer;
  }
  ast_node a = {
      .type = literal_t, .lit_t = integer_t, .value.integer = accumulator};
  return a;
}

ast_node var(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  assert(ast.child_ast[1].lit_t == ident_t);
  char *variable_name = ast.child_ast[1].value.ident;
  ast_node value = auto_ast_walk(ast.child_ast[2], ctx);
  return bind_ident(ctx, variable_name, value);
}

ast_node begin(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  ast_node last;
  for (int i = 1; i < ast.size; i++) {
    last = auto_ast_walk(ast.child_ast[i], ctx);
  }
  return last;
}

ast_node lt(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  ast_node left = auto_ast_walk(ast.child_ast[1], ctx);
  ast_node right = auto_ast_walk(ast.child_ast[2], ctx);
  ast_node n = {.type = literal_t, .lit_t = bool_t};
  switch (left.lit_t) {
  case integer_t:
    n.value.boolean = left.value.integer < right.value.integer;
    break;
  case floating_t:
    n.value.boolean = left.value.floating < right.value.floating;
    break;
  default:
    assert("Unsupported type for less than" == false);
  }

  return n;
}

ast_node if_expr(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  // [0] = 'if'
  // [1] = condition
  // [2] = then
  // [3] = else
  ast_node condition = auto_ast_walk(ast.child_ast[1], ctx);
  if (condition.lit_t == bool_t) {
    if (condition.value.boolean) {
      return auto_ast_walk(ast.child_ast[2], ctx);
    } else {
      return auto_ast_walk(ast.child_ast[3], ctx);
    }
  } else {
    assert("If expression condition must be of type bool" == false);
  }
}

builtin *builtin_arr[] = {plus, var, begin, lt, if_expr};

builtin *is_builtin(char *ident) {
  for (int i = 0; i < sizeof(builtins) / sizeof(char *); i++) {
    if (!strcmp(ident, builtins[i])) {
      return builtin_arr[i];
    }
  }
  return NULL;
}

ast_node bind_ident(hashmap *ctx, char *key, ast_node value) {
  hashmap_insert(ctx, key, value);
  return value;
}

ast_node get_ident(hashmap *ctx, char *key) {
  ast_node ret = hashmap_get(ctx, key);
  if (ret.type == tombstone_t) {
    fprintf(stderr, "No variable associated with identifier %s\n", key);
    exit(EXIT_FAILURE);
  }
  return ret;
}

// Evaluates a list by taking the first value as a
// function, and the remaining values as arguments
ast_node ast_walk(ast_node ast, hashmap *ctx) {
  assert(ast.type == list_t);
  struct ast_arr children = ast.child;
  char *function_name = children.child_ast[0].value.string;
  builtin *b;
  if ((b = is_builtin(function_name)) != NULL) {
    return b(children, ctx);
  }

  fprintf(stderr, "%d %s\n", children.child_ast[0].type, function_name);
  assert("nothing but builtins implemented yet" == false);
}

// Can differentiate between a list or a literal
// returns a literal value if given a literal
// returns an evaluated literal value if given a list
ast_node auto_ast_walk(ast_node ast, hashmap *ctx) {
  if (ast.type == literal_t) {
    if (ast.lit_t == ident_t) {
      return get_ident(ctx, ast.value.ident);
    } else {
      return ast;
    }
  }
  return ast_walk(ast, ctx);
}

/* int main() { */
/*   hashmap ctx = hashmap_init(fnv_string_hash, str_equals, 0.5, 0); */

/*   string program = str_auto("(begin (var a 5) (var b 6) (+ a b))"); */
/*   token_arr ta = lex(&program); */
/*   str_free(&program); */

/*   int cursor = 0; */
/*   ast_node ast = parse(ta, &cursor); */
/*   ast_print(ast); */
/*   puts(""); */

/*   ast_node result = ast_walk(ast, &ctx); */
/*   ast_print(result); */

/*   ast_node_free(&ast); */
/*   hashmap_free(&ctx); */
/*   ta_free(&ta); */
/* } */
