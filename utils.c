#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *str_val(string *s) {
  return (char *)((uint64_t)s->str & (0x0000FFFFFFFFFFFF));
}

uint16_t str_len(string *s) { return (uint16_t)((uint64_t)s->str >> 48); }

void str_set_len(string *s, uint16_t new_size) {
  s->str = (char *)((uint64_t)str_val(s) | ((uint64_t)new_size << 48));
}

void str_free(string *s) {
  free(str_val(s));
  s->str = NULL;
}

// use with heap memory
string str_new(char *ptr, int16_t len) {
  return (string){.str = (char *)((uint64_t)ptr | ((uint64_t)len << 48))};
}

// Use with stack memory
string str_auto(const char *ptr) {
  int len = strlen(ptr);
  char *str = strdup(ptr);
  return (string){.str = (char *)((uint64_t)str | ((uint64_t)len << 48))};
}

bool str_whitespace(string *s, uint16_t pos) {
  char c = str_val(s)[pos];
  switch (c) {
  case ' ':
  case '\t':
  case '\n':
    return true;
  default:
    return false;
  }
}

/* int main() { */
/*   char *str = malloc(30); */
/*   strcpy(str, "hi mom"); */
/*   string s = str_new(str, 30); */

/*   printf("%s\n", str_val(&s)); */
/*   printf("%d\n", str_len(&s)); */
/*   str_free(&s); */
/* } */
