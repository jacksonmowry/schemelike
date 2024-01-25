#include <stdint.h>
#include <stdio.h>

typedef enum INST {
  ADD = 0,
  SUB,
  DIV,
  MUL,
  PUSH,
  POP,
  BEQ,
} INST;

int64_t STACK[1024];
uint64_t SP = 0; // Points to the first free space
uint64_t IP = 0;

int main() {
  int64_t program[] = {PUSH, 3, PUSH, 3, BEQ, 7};
  while (IP < sizeof(program) / sizeof(int64_t)) {
    int64_t op_code = program[IP];
    switch (op_code) {
    case ADD: {
      int64_t reg_a = STACK[--SP];
      int64_t reg_b = STACK[--SP];
      STACK[SP++] = reg_a + reg_b;
      IP += 1;
      break;
    }
    case SUB: {
      int64_t reg_a = STACK[--SP];
      int64_t reg_b = STACK[--SP];
      STACK[SP++] = reg_a - reg_b;
      IP += 1;
      break;
    }
    case DIV: {
      int64_t reg_a = STACK[--SP];
      int64_t reg_b = STACK[--SP];
      STACK[SP++] = reg_a / reg_b;
      IP += 1;
      break;
    }
    case MUL: {
      int64_t reg_a = STACK[--SP];
      int64_t reg_b = STACK[--SP];
      STACK[SP++] = reg_a * reg_b;
      IP += 1;
      break;
    }
    case PUSH: {
      STACK[SP++] = program[IP + 1];
      IP += 2;
      break;
    }
    case POP: {
      --SP;
      IP += 1;
      break;
    }
    case BEQ: {
      int64_t reg_a = STACK[--SP];
      int64_t reg_b = STACK[--SP];
      if (reg_a == reg_b) {
        IP = program[IP + 1];
      } else {
        IP += 2;
      }
      break;
    }
    }
  }

  printf("%ld\n", (SP > 0) ? STACK[SP - 1] : 69);
  printf("%lu\n", IP);
}
