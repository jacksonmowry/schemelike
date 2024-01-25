#include "ast_walking.h"
#include "hashmap.h"
#include "lex.h"
#include "parse.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool any_double(struct ast_arr ast, hashmap *ctx) {
  for (int i = 0; i < ast.size; i++) {
    ast_node a = auto_ast_walk(ast.child_ast[i], ctx);
    if (a.lit_t == floating_t) {
      return true;
    }
  }
  return false;
}

double to_double(ast_node a) {
  switch (a.lit_t) {
  case integer_t:
    return (double)a.value.integer;
  case floating_t:
    return a.value.floating;
  default:
    exit(69);
  }
}

char *builtins[] = {"+",     "-", "*",  "/",    "var",     "const",
                    "begin", "<", "if", "func", "average", "abs"};

ast_node plus(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  // Ignore the first node in arr as it is the ident '+'
  if (auto_ast_walk(ast.child_ast[1], ctx).lit_t == integer_t) {
    int64_t accumulator = 0;
    for (int i = 1; i < ast.size; i++) {
      ast_node v = auto_ast_walk(ast.child_ast[i], ctx);
      accumulator += v.value.integer;
    }
    ast_node a = {
        .type = literal_t, .lit_t = integer_t, .value.integer = accumulator};
    return a;
  } else if (auto_ast_walk(ast.child_ast[1], ctx).lit_t == floating_t) {
    double accumulator = 0;
    for (int i = 1; i < ast.size; i++) {
      ast_node v = auto_ast_walk(ast.child_ast[i], ctx);
      accumulator += v.value.floating;
    }
    ast_node a = {
        .type = literal_t, .lit_t = floating_t, .value.floating = accumulator};
    return a;
  }
  printf("unsupported operation for ");
  ast_print(ast.child_ast[1]);
  printf("\n");
  exit(1);
}

ast_node minus(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  // Ignore the first node in arr as it is the ident '-'
  if (auto_ast_walk(ast.child_ast[1], ctx).lit_t == integer_t) {
    int64_t accumulator = auto_ast_walk(ast.child_ast[1], ctx).value.integer;
    for (int i = 2; i < ast.size; i++) {
      ast_node v = auto_ast_walk(ast.child_ast[i], ctx);
      accumulator -= v.value.integer;
    }
    ast_node a = {
        .type = literal_t, .lit_t = integer_t, .value.integer = accumulator};
    return a;
  } else if (auto_ast_walk(ast.child_ast[1], ctx).lit_t == floating_t) {
    double accumulator = auto_ast_walk(ast.child_ast[1], ctx).value.floating;
    for (int i = 2; i < ast.size; i++) {
      ast_node v = auto_ast_walk(ast.child_ast[i], ctx);
      accumulator -= v.value.floating;
    }
    ast_node a = {
        .type = literal_t, .lit_t = floating_t, .value.floating = accumulator};
    return a;
  }

  printf("unsupported operation for ");
  ast_print(ast.child_ast[1]);
  printf("\n");
  exit(1);
}

ast_node mul(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  // Ignore the first node in arr as it is the ident '*'
  if (auto_ast_walk(ast.child_ast[1], ctx).lit_t == integer_t) {
    int64_t accumulator = auto_ast_walk(ast.child_ast[1], ctx).value.integer;
    for (int i = 2; i < ast.size; i++) {
      ast_node v = auto_ast_walk(ast.child_ast[i], ctx);
      accumulator *= v.value.integer;
    }
    ast_node a = {
        .type = literal_t, .lit_t = integer_t, .value.integer = accumulator};
    return a;
  } else if (auto_ast_walk(ast.child_ast[1], ctx).lit_t == floating_t) {
    double accumulator = auto_ast_walk(ast.child_ast[1], ctx).value.floating;
    for (int i = 2; i < ast.size; i++) {
      ast_node v = auto_ast_walk(ast.child_ast[i], ctx);
      accumulator *= v.value.floating;
    }
    ast_node a = {
        .type = literal_t, .lit_t = floating_t, .value.floating = accumulator};
    return a;
  }

  printf("unsupported operation for ");
  ast_print(ast.child_ast[1]);
  printf("\n");
  exit(1);
}

ast_node division(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  // Ignore the first node in arr as it is the ident '/'
  if (auto_ast_walk(ast.child_ast[1], ctx).lit_t == integer_t) {
    int64_t accumulator = auto_ast_walk(ast.child_ast[1], ctx).value.integer;
    for (int i = 2; i < ast.size; i++) {
      ast_node v = auto_ast_walk(ast.child_ast[i], ctx);
      accumulator /= v.value.integer;
    }
    ast_node a = {
        .type = literal_t, .lit_t = integer_t, .value.integer = accumulator};
    return a;
  } else if (auto_ast_walk(ast.child_ast[1], ctx).lit_t == floating_t) {
    double accumulator = auto_ast_walk(ast.child_ast[1], ctx).value.floating;
    for (int i = 2; i < ast.size; i++) {
      ast_node v = auto_ast_walk(ast.child_ast[i], ctx);
      accumulator /= v.value.floating;
    }
    ast_node a = {
        .type = literal_t, .lit_t = floating_t, .value.floating = accumulator};
    return a;
  }

  printf("unsupported operation for ");
  ast_print(ast.child_ast[1]);
  printf("\n");
  exit(1);
}

ast_node var(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  assert(ast.child_ast[1].lit_t == ident_t);
  char *variable_name = ast.child_ast[1].value.ident;
  // using 'hashmap_get' directly here so it doesn't throw
  // and error if the key DNE
  ast_node prev_value = hashmap_get(ctx, variable_name);
  // Look up if var exists and is const
  if (prev_value.type == const_t) {
    fprintf(stderr, "Cannot reassign to const ident %s\n", variable_name);
    exit(1);
  }
  ast_node value = auto_ast_walk(ast.child_ast[2], ctx);
  return bind_ident(ctx, variable_name, value);
}

ast_node _const(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  assert(ast.child_ast[1].lit_t == ident_t);
  char *variable_name = ast.child_ast[1].value.ident;
  ast_node prev_value = hashmap_get(ctx, variable_name);
  // Look up if var exists and is const
  if (prev_value.type == const_t) {
    fprintf(stderr, "Cannot reassign to const ident %s\n", variable_name);
    exit(1);
  }
  ast_node value = auto_ast_walk(ast.child_ast[2], ctx);
  value.type = const_t;
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
  printf("left is %f\n", to_double(left));
  printf("right is %f\n", to_double(right));
  ast_node n = {.type = literal_t, .lit_t = bool_t};
  struct ast_arr args = {.size = ast.size - 1, .child_ast = &ast.child_ast[1]};
  if (any_double(args, ctx)) {
    n.value.boolean = to_double(left) < to_double(right);
  } else {
    n.value.boolean = left.value.integer < right.value.integer;
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
  return bind_ident(ctx, ast.child_ast[1].value.ident, new_func);
}

ast_node average(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  double total = 0;
  int count = 0;
  for (; count < ast.size - 1; count++) {
    ast_node val = auto_ast_walk(ast.child_ast[count + 1], ctx);
    if (val.lit_t == integer_t) {
      total += val.value.integer;
    } else {
      total += val.value.floating;
    }
  }

  return (ast_node){
      .type = literal_t, .lit_t = floating_t, .value.floating = total / count};
}

ast_node my_abs(struct ast_arr ast, hashmap *ctx) {
  assert(ast.child_ast[0].lit_t == ident_t);
  ast_node operand = auto_ast_walk(ast.child_ast[1], ctx);
  if (operand.lit_t == integer_t) {
    return (ast_node){.type = literal_t,
                      .lit_t = integer_t,
                      .value.integer = labs(operand.value.integer)};
  } else if (operand.lit_t == floating_t) {
    return (ast_node){.type = literal_t,
                      .lit_t = floating_t,
                      .value.integer = fabs(operand.value.floating)};
  }

  printf("unsuported operation on ");
  ast_print(ast.child_ast[1]);
  printf("\n");
  exit(1);
}

builtin *builtin_arr[] = {plus,  minus, mul,     division, var,     _const,
                          begin, lt,    if_expr, func,     average, my_abs};

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
               auto_ast_walk(children.child_ast[i + 1], ctx));
  }

  ast_node func_to_execute = {.type = list_t, .child = user_func.child};
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
