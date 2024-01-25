#include "ast_walking.h"
#include "hashmap.h"
#include "lex.h"
#include "parse.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const char *HEADER = "\033[1;95m";
const char *OKBLUE = "\033[1;94m";
const char *OKCYAN = "\033[1;96m";
const char *OKGREEN = "\e[1;32m";
const char *WARNING = "\033[1;93m";
const char *FAIL = "\033[1;91m";
const char *ENDC = "\033[0m";
const char *UNDERLINE = "\033[4m";

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: ./%s filename\n", argv[0]);
    exit(1);
  }
  printf("%sSchemelike interpreter!%s\n\n", OKGREEN, ENDC);
  hashmap ctx = hashmap_init(fnv_string_hash, str_equals, 0.5, 0);

  FILE *fp = fopen(argv[1], "r");
  char raw[1000];
  size_t read = fread(raw, sizeof(char), 1000, fp);
  raw[read - 1] = 0;
  printf("%sSource code: %s\n", OKBLUE, ENDC);
  puts(raw);
  fclose(fp);

  string program = str_auto(raw);

  token_arr ta = lex(&program);
  /* for (int i = 0; i < ta.size; i++) { */
  /*   token_debug(ta.tokens[i]); */
  /* } */
  str_free(&program);

  int cursor = 0;
  ast_node ast = parse(ta, &cursor);
  printf("%sAST Representation: %s\n", OKBLUE, ENDC);
  ast_print(ast);
  puts("");

  ast_node result = ast_walk(ast, &ctx);
  printf("%sResult:%s \n", FAIL, ENDC);
  ast_print(result);
  puts("");

  for (int i = 0; i < ctx.capacity; i++) {
    ast_node a = ctx.array[i].value;
    if (a.type == function_t) {
      free(a.value.params);
    }
  }

  ast_node_free(&ast);
  ta_free(&ta);
  hashmap_free(&ctx);
}
