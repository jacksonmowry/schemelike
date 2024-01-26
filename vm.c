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
  J,
  CALL,
  RET,
  LDX,
  LDXI,
  LDY,
  LDYI,
  STX,
  STY,
  PRINT,
} INST;

int64_t STACK[1024];
uint64_t SP = 0; // Points to the first free space
uint64_t IP = 18;
uint64_t RA = 0;

int64_t X = 0;
int64_t Y = 0;

int main() {
  int64_t program[] = {LDYI,  122, LDX,  STY, STX,  BEQ, 16,   STX,
                       PRINT, STX, PUSH, 1,   ADD,  LDX, J,    3,
                       STX,   RET, PUSH, 11,  CALL, 0,   PRINT};
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
    case J: {
      IP = program[IP + 1];
      break;
    }
    case CALL: {
      RA = IP + 2;
      IP = program[IP + 1];
      break;
    }
    case RET: {
      IP = RA;
      break;
    }
    case LDX: {
      X = STACK[--SP];
      IP += 1;
      break;
    }
    case LDXI: {
      X = program[IP + 1];
      IP += 2;
      break;
    }
    case LDY: {
      Y = STACK[--SP];
      IP += 1;
      break;
    }
    case LDYI: {
      Y = program[IP + 1];
      IP += 2;
      break;
    }
    case STX: {
      STACK[SP++] = X;
      IP += 1;
      break;
    }
    case STY: {
      STACK[SP++] = Y;
      IP += 1;
      break;
    }
    case PRINT: {
      int64_t a = STACK[--SP];
      printf("%ld\n", a);
      IP += 1;
      break;
    }
    }
  }
  for (int i = 0; i < SP; i++) {
    printf("%ld ", STACK[i]);
  }
  puts("");
  printf("SP: %ld\n", SP);
  printf("IP: %lu\n", IP);
  printf("RA: %lu\n", RA);
  printf("X: %lu\n", X);
  printf("Y: %lu\n", Y);
}
