// gcc -o test test_libunwind.c -lunwind && ./test
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <stdio.h>

static void get_backtrace_by_unwind() {
  unw_cursor_t cursor;
  unw_context_t context;

  unw_getcontext(&context);
  unw_init_local(&cursor, &context);

  while (unw_step(&cursor) > 0) {
    unw_word_t offset, pc;
    char symbols[64] = {0};

    unw_get_reg(&cursor, UNW_REG_IP, &pc);
    unw_get_proc_name(&cursor, symbols, sizeof(symbols), &offset);
    printf("%lu : (%s+0x%lu) [%lu]\n", pc, symbols, offset, pc);
  }
}

static void show_backtrace(void) {
  unw_cursor_t cursor;
  unw_context_t uc;
  unw_word_t ip, sp;

  unw_getcontext(&uc);
  unw_init_local(&cursor, &uc);
  while (unw_step(&cursor) > 0) {
    unw_get_reg(&cursor, UNW_REG_IP, &ip);
    unw_get_reg(&cursor, UNW_REG_SP, &sp);
    printf("ip = %lx, sp = %lx\n", (long)ip, (long)sp);
  }
  get_backtrace_by_unwind();
}

static void bar(void) { show_backtrace(); }

static void foo(void) { bar(); }

int main(void) {
  foo();
  return 0;
}
