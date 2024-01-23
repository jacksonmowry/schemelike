#include "lex.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *token_str[] = {"Syntax", "Integer", "Floating Point", "String",
                           "Identifier"};

token *token_new(int pos, token_type type, char *value) {
  token *t = malloc(sizeof(token));
  *t = (token){.location = pos, .type = type, .value = value};
  return t;
}

void token_debug(token t) {
  if (t.type == string_type) {
    printf("{\n\ttype:      %s,\n\tvalue:     \"%s\",\n\tlocation:  %d\n}\n",
           token_str[t.type], t.value, t.location);
  } else if (t.type == syntax_type) {
    printf("{\n\ttype:      %s,\n\tvalue:     `%s`,\n\tlocation:  %d\n}\n",
           token_str[t.type], t.value, t.location);
  } else {
    printf("{\n\ttype:      %s,\n\tvalue:     %s,\n\tlocation:  %d\n}\n",
           token_str[t.type], t.value, t.location);
  }
}

void eat_whitespace(string *source, int *cursor) {
  while (*cursor < str_len(source)) {
    if (str_whitespace(source, *cursor)) {
      (*cursor)++;
      continue;
    }
    break;
  }
  return;
}

token *lex_syntax(string *source, int *cursor) {
  char c = str_val(source)[*cursor];
  if (c == '(' || c == ')') {
    int orig_cursor = *cursor;
    (*cursor)++; // skip past syntax
    return token_new(orig_cursor, syntax_type, strndup(&c, 1));
  }

  return NULL;
}

token *lex_number(string *source, int *cursor) {
  int orig_cursor = *cursor;
  while (*cursor < str_len(source)) {
    char c = str_val(source)[*cursor];
    if (c >= '0' && c <= '9' || c == '.') {
      (*cursor)++;
      continue;
    }
    break;
  }

  if (*cursor > orig_cursor) {
    char *num = strndup(&str_val(source)[orig_cursor], *cursor - orig_cursor);
    if (strchr(num, '.')) {
      return token_new(orig_cursor, floating_type, num);
    } else {
      return token_new(orig_cursor, integer_type, num);
    }
  } else {
    return NULL;
  }
}

token *lex_string(string *source, int *cursor) {
  char c = str_val(source)[*cursor];
  if (c != '"') {
    return NULL;
  }
  int orig_cursor = *cursor;
  (*cursor)++;

  while (*cursor < str_len(source)) {
    c = str_val(source)[*cursor];
    if (c == '"') {
      (*cursor)++; // skip past `"`
      return token_new(orig_cursor, string_type,
                       strndup(&str_val(source)[orig_cursor + 1],
                               *cursor - orig_cursor - 2));
    }
    (*cursor)++;
  }

  fprintf(stderr, "Unclosed string literal around %.*s", 20,
          &str_val(source)[orig_cursor]);
  exit(EXIT_FAILURE);
}

token *lex_ident(string *source, int *cursor) {
  int orig_cursor = *cursor;
  while (*cursor < str_len(source)) {
    char c = str_val(source)[*cursor];
    if (!(c == ' ' || c == ')')) {
      (*cursor)++;
      continue;
    }
    break;
  }

  if (*cursor > orig_cursor) {
    return token_new(
        orig_cursor, ident_type,
        strndup(&str_val(source)[orig_cursor], *cursor - orig_cursor));
  }

  return NULL;
}

token *(*lexer_array[])(string *, int *) = {lex_syntax, lex_number, lex_string,
                                            lex_ident};

int lexer_count = sizeof(lexer_array) / sizeof(uintptr_t);

void ta_pb(token_arr *ta, token *t) {
  if (ta->size + 1 >= ta->cap) {
    ta->cap *= 2;
    ta->tokens = reallocarray(ta->tokens, ta->cap, sizeof(token));
  }
  ta->tokens[ta->size++] = *t;
  free(t);
}

token_arr ta_init() {
  return (token_arr){.tokens = calloc(10, sizeof(token)), .size = 0, .cap = 10};
}

void ta_free(token_arr *ta) {
  for (int i = 0; i < ta->size; i++) {
    free(ta->tokens[i].value);
  }
  free(ta->tokens);
}

token_arr lex(string *source) {
  token_arr ta = ta_init();
  token *t = NULL;
  int cursor = 0;

outer:
  while (cursor < str_len(source)) {
    eat_whitespace(source, &cursor);
    for (int i = 0; i < lexer_count; i++) {
      t = lexer_array[i](source, &cursor);
      if (t) {
        ta_pb(&ta, t);
        goto outer;
      }
    }
    fprintf(stderr, "Unable to lex token at pos: %d\n", cursor);
    exit(EXIT_FAILURE);
  }

  return ta;
}

/* int main() { */
  /* assert(!strcmp(token_str[syntax_type], "Syntax")); */
  /* token t = {.type = syntax_type, .value = "(", .location = 0}; */
  /* token_debug(t); */
  /* token t2 = {.type = string_type, .value = "hi mom", .location = 1}; */
  /* token_debug(t2); */
  /* token t3 = {.type = integer_type, .value = "5", .location = 10}; */
  /* token_debug(t3); */

  /* char *str = malloc(30); */
  /* strcpy(str, "    (5 5.5 \"hi mom\" ident)"); */
  /* string s = str_new(str, 30); */

  /* int pos = 0; */
  /* eat_whitespace(&s, &pos); */
  /* assert(pos == 4); */

  /* token *syntax = lex_syntax(&s, &pos); */
  /* token_debug(*syntax); */
  /* assert(pos == 5); */
  /* assert(syntax->location == 4); */
  /* assert(syntax->type == syntax_type); */
  /* assert(!strcmp(syntax->value, "(")); */

  /* token *num = lex_number(&s, &pos); */
  /* token_debug(*num); */
  /* assert(pos == 6); */
  /* assert(num->location == 5); */
  /* assert(num->type == integer_type); */
  /* assert(!strcmp(num->value, "5")); */

  /* eat_whitespace(&s, &pos); */
  /* assert(pos == 7); */

  /* token *floating = lex_number(&s, &pos); */
  /* token_debug(*floating); */
  /* assert(pos == 10); */
  /* assert(floating->location == 7); */
  /* assert(floating->type == floating_type); */
  /* assert(!strcmp(floating->value, "5.5")); */

  /* eat_whitespace(&s, &pos); */
  /* assert(pos == 11); */

  /* token *str_tok = lex_string(&s, &pos); */
  /* token_debug(*str_tok); */
  /* assert(pos == 19); */
  /* assert(str_tok->location == 11); */
  /* assert(str_tok->type == string_type); */
  /* assert(!strcmp(str_tok->value, "hi mom")); */

  /* eat_whitespace(&s, &pos); */
  /* assert(pos == 20); */

  /* token *ident = lex_ident(&s, &pos); */
  /* token_debug(*ident); */
  /* assert(pos == 25); */
  /* assert(ident->location == 20); */
  /* assert(ident->type == ident_type); */
  /* assert(!strcmp(ident->value, "ident")); */

  /* str_free(&s); */

  /* free(syntax->value); */
  /* free(num->value); */
  /* free(floating->value); */
  /* free(str_tok->value); */
  /* free(ident->value); */
  /* free(syntax); */
  /* free(num); */
  /* free(floating); */
  /* free(str_tok); */
  /* free(ident); */

/*   string program = str_auto("(+ 5 5.5 (var a 5) \"hi mom\" ident)"); */
/*   assert(str_len(&program) == 34); */
/*   assert(!strcmp(str_val(&program), "(+ 5 5.5 (var a 5) \"hi mom\" ident)")); */

/*   token_arr ta = lex(&program); */
/*   assert(ta.size == 12); */
/*   for (int i = 0; i < ta.size; i++) { */
/*     token_debug(ta.tokens[i]); */
/*   } */

/*   ta_free(&ta); */
/*   str_free(&program); */
/* } */
