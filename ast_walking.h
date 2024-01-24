#ifndef AST_WALKING_H_
#define AST_WALKING_H_
#include "hashmap.h"
#include "lex.h"
#include "parse.h"

ast_node ast_walk(ast_node, hashmap *);
ast_node auto_ast_walk(ast_node, hashmap *);

typedef ast_node(builtin)(struct ast_arr, hashmap *);
builtin *is_builtin(char *);

ast_node plus(struct ast_arr ast, hashmap *ctx);
ast_node var(struct ast_arr ast, hashmap *ctx);
ast_node begin(struct ast_arr ast, hashmap *ctx);

ast_node bind_ident(hashmap *ctx, char *key, ast_node value);
ast_node get_ident(hashmap *ctx, char *key);
#endif // AST_WALKING_H_
