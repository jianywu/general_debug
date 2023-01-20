// gcc -o test test_libunwind.c -lunwind && ./test

#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <stdio.h>
#include <string.h>

#define STACK_TRACE_DEPTH 4
#define STACK_TRACE_MAX_SYMBOL_LEN 20
#define STACK_TRACE_MAX_OUTPUT_LEN (STACK_TRACE_DEPTH * STACK_TRACE_MAX_SYMBOL_LEN)

static inline void get_backtrace_by_unwind(char* str) {
    unw_cursor_t    cursor;
    unw_context_t   context;

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    int depth = 0;
    while (unw_step(&cursor) > 0) {
        unw_word_t  offset, pc;
        char symbols[STACK_TRACE_MAX_OUTPUT_LEN] = {0};
        int len = 0;

        unw_get_proc_name(&cursor, symbols, sizeof(symbols), &offset);
        len = strlen(symbols) > STACK_TRACE_MAX_SYMBOL_LEN ? STACK_TRACE_MAX_SYMBOL_LEN : strlen(symbols);
        strncat(str, symbols, len);
        depth++;
        if (depth >= STACK_TRACE_DEPTH) break;
        strncat(str, "<-", 3);
    }
    strncat(str, "\0", 3);
}

static void bar(void) {
  char call_trace[STACK_TRACE_MAX_OUTPUT_LEN] = {0};
  get_backtrace_by_unwind(call_trace);
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