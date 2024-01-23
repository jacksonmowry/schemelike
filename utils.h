#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>
#include <stdint.h>
typedef struct string {
  char *str;
} string;

char *str_val(string *);
uint16_t str_len(string *);
void str_set_len(string *, uint16_t);
void str_free(string *);
string str_new(char *, int16_t);
string str_auto(const char *);

bool str_whitespace(string *, uint16_t);

#endif // UTILS_H_
