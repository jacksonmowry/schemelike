#ifndef LEX_H_
#define LEX_H_
#include "utils.h"
typedef enum token_type {
  syntax_type,
  integer_type,
  floating_type,
  bool_type,
  string_type,
  ident_type,
} token_type;

typedef struct token {
  token_type type;
  char *value;
  int location;
} token;

token *token_new(int, token_type, char *);
void token_debug(token);

void eat_whitespace(string *, int *);

token *lex_syntax(string *, int *);
token *lex_number(string *, int *);
token *lex_bool(string *, int *);
token *lex_string(string *, int *);
token *lex_ident(string *, int *);

token *(lexer)(string *, int *);

typedef struct token_arr {
  token *tokens;
  int size;
  int cap;
} token_arr;

void ta_pb(token_arr *, token *);
token_arr ta_int();
void ta_free(token_arr *);

token_arr lex(string *);

#endif // LEX_H_
