// gcc -o test -O0 -g k.c -rdynamic
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>

#define STACK_TRACE_DEPTH 4
#define STACK_TRACE_MAX_SYMBOL_LEN 20
#define STACK_TRACE_MAX_OUTPUT_LEN (STACK_TRACE_DEPTH * STACK_TRACE_MAX_SYMBOL_LEN)

static void get_backtrace(char* str) {
    void *buffer[STACK_TRACE_MAX_OUTPUT_LEN];
    int depth = 0;
    int len = 0;

    int buffer_size = backtrace(buffer, STACK_TRACE_MAX_OUTPUT_LEN);
    char **symbols = backtrace_symbols(buffer, buffer_size);
    for (int i = 0; i < buffer_size; ++i) {
        printf("%p:%s\n", buffer[i], symbols[i]);
        len = strlen(symbols[i]) > STACK_TRACE_MAX_SYMBOL_LEN ? STACK_TRACE_MAX_SYMBOL_LEN : strlen(symbols[i]);
        strncat(str, symbols[i], len);
        depth++;
        if (depth >= STACK_TRACE_DEPTH) break;
        strncat(str, "<-", 3);
    }
    free(symbols);
}

static void bar(void) {
  char call_trace[STACK_TRACE_MAX_OUTPUT_LEN] = {0};
  get_backtrace(call_trace);
  printf("call trace:%s\n", call_trace);
}

static void foo(void) {
  bar();
}

int main(void)
{
  foo();
  return 0;
}