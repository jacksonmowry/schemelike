#include "ast_walking.h"
#include "hashmap.h"
#include "lex.h"
#include "parse.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *builtins[] = {"+", "-", "var", "begin", "<", "if", "func"};

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

ast_node minus(struct ast_arr ast, hashmap *ctx) {
  puts("inside minus");
  for (int i = 0; i < ast.size; i++) {
    ast_print(ast.child_ast[i]);
    puts("");
  }
  puts("done minus");
  assert(ast.child_ast[0].lit_t == ident_t);
  // Ignore the first node in arr as it is the ident '+'
  puts("getting a inside minus");
  ast_print(get_ident(ctx, "a"));
  puts("");
  int64_t accumulator = auto_ast_walk(ast.child_ast[1], ctx).value.integer;
  printf("starting accumulator %ld\n", accumulator);
  for (int i = 2; i < ast.size; i++) {
    ast_node v = auto_ast_walk(ast.child_ast[i], ctx);
    accumulator -= v.value.integer;
  }
  ast_node a = {
      .type = literal_t, .lit_t = integer_t, .value.integer = accumulator};
  printf("accumulator %ld\n", accumulator);
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

ast_node func(struct ast_arr ast, hashmap *ctx) {
  // (func ident (a b) (+ a b))
  //  0    1      2     3
  assert(ast.child_ast[0].lit_t == ident_t);
  assert(ast.child_ast[1].lit_t == ident_t);
  assert(ast.child_ast[2].type == list_t);
  char **params = calloc(ast.child_ast[2].child.size,
                         sizeof(char **)); // this seg faults < 2
  for (int i = 0; i < ast.child_ast[2].child.size; i++) {
    params[i] = ast.child_ast[2].child.child_ast[i].value.ident;
  }
  ast_node new_func = {.type = function_t,
                       .child = ast.child_ast[3].child,
                       .lit_t = params_t,
                       .value.params = params};
  // This will make a node representing our function
  // The child_ast is the actual body of the function to be executed
  // The value.params is another ast of idents with the params
  // at runtime we will create a new context binding the arguments
  // to these params and executing the function through ast walking
  puts("new function");
  ast_print(new_func);
  puts("");
  return bind_ident(ctx, ast.child_ast[1].value.ident, new_func);
}

builtin *builtin_arr[] = {plus, minus, var, begin, lt, if_expr, func};

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
  // Now we are looking for a user defined func
  ast_node user_func = get_ident(ctx, function_name);
  assert(user_func.type == function_t);
  // We really need to copy the outer ctx, this prevents functions from calling
  // other functions or even themselves
  /* hashmap child_ctx = hashmap_init(fnv_string_hash, str_equals, 0.5, 0); */
  hashmap child_ctx = hashmap_copy(ctx);
  /* assert(((ast_node *)user_func.value.params)->child.size == children.size -
   * 1); */
  for (int i = 0; i < children.size - 1; i++) {
    bind_ident(&child_ctx, user_func.value.params[i],
               children.child_ast[i + 1]);
  }

  ast_node func_to_execute = {.type = list_t, .child = user_func.child};
  puts("func to execute");
  ast_print(func_to_execute);
  puts("");
  puts("value of a");
  ast_print(get_ident(&child_ctx, "a"));
  puts("");
  puts("evaluated a");
  auto_ast_walk(get_ident(&child_ctx, "a"), &child_ctx);
  puts("");

  ast_node result = auto_ast_walk(func_to_execute, &child_ctx);
  hashmap_free(&child_ctx);
  return result;
  /* fprintf(stderr, "%d %s\n", children.child_ast[0].type, function_name); */
  /* assert("nothing but builtins implemented yet" == false); */
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
