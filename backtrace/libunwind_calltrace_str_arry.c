// gcc -o test libunwind_test.c -lunwind && ./test

#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <stdio.h>
#include <string.h>

#define STACK_TRACE_DEPTH 4
#define STACK_TRACE_MAX_SYMBOL_LEN 30
#define STACK_TRACE_MAX_OUTPUT_LEN (STACK_TRACE_DEPTH * STACK_TRACE_MAX_SYMBOL_LEN)

typedef struct {
    char frame[STACK_TRACE_DEPTH][STACK_TRACE_MAX_SYMBOL_LEN];
} unwind_calltrace_t;

static inline void get_backtrace_by_unwind(unwind_calltrace_t* trace) {
    unw_cursor_t cursor;
    unw_context_t context;

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    int depth = 0;
    while (unw_step(&cursor) > 0) {
        unw_word_t offset;
        char symbols[STACK_TRACE_MAX_OUTPUT_LEN] = {0};
        int len = 0;

        unw_get_proc_name(&cursor, symbols, sizeof(symbols), &offset);
        len = strlen(symbols) > STACK_TRACE_MAX_SYMBOL_LEN ? STACK_TRACE_MAX_SYMBOL_LEN : strlen(symbols);
        strncpy(trace->frame[depth], symbols, len);
        trace->frame[depth][STACK_TRACE_MAX_SYMBOL_LEN - 1] = '\0';
        depth++;
        if (depth >= STACK_TRACE_DEPTH)
            break;
    }
}

static void bar(void) {
    unwind_calltrace_t trace;
    memset(&trace, 0, sizeof(trace));
    get_backtrace_by_unwind(&trace);
    printf("call trace:%s->%s->%s\n",
           trace.frame[2], trace.frame[1], trace.frame[0]);
}

static void foo(void) {
    bar();
}

int main(void) {
    foo();
    return 0;
}

/**
 * output:
 * call trace:main->foo->bar
 */