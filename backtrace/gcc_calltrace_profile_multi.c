// refer to https://zhuanlan.zhihu.com/p/73621327, thanks.
// gcc -o test -g gcc_calltrace_profile_multi.c -finstrument-functions -lpthread && ./test
/*
enter 0x400bbe, caller 0x7f3ff01cc545
enter 0x400b90, caller 0x400bdd
enter 0x400b39, caller 0x400baa
in func get_tls_backtrace():
frame[1] addr:0x400ba6
frame[2] addr:0x400bd9
frame[3] addr:0x7f3ff01cc541
*/

/*
sBackTraceKey is used to store the pointer of thread_stack_t, contains callstack frames.
Frames are stored in heap.
__cyg_profile_func_enter() and __cyg_profile_func_exit() is generate by -finstrument-functions.
*/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define STACK_TRACE_DEPTH 4

typedef struct {
    void* frame[STACK_TRACE_DEPTH];
    int current;
} thread_stack_t;

static pthread_once_t sBackTraceOnce = PTHREAD_ONCE_INIT;
static pthread_key_t sBackTraceKey;

__attribute__((no_instrument_function)) static void destructor(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

__attribute__((no_instrument_function)) static void init_once(void) {
    pthread_key_create(&sBackTraceKey, destructor);
}

__attribute__((no_instrument_function))
static inline thread_stack_t* get_backtrace_info(void) {
    thread_stack_t* ptr = (thread_stack_t*)pthread_getspecific(sBackTraceKey);
    if (ptr) {
        return ptr;
    }
    ptr = (thread_stack_t*)malloc(sizeof(thread_stack_t));
    ptr->current = STACK_TRACE_DEPTH - 1;
    pthread_setspecific(sBackTraceKey, ptr);
    return ptr;
}

__attribute__((no_instrument_function)) void __cyg_profile_func_enter(void* this_func, void* call_site) {
    pthread_once(&sBackTraceOnce, init_once);
    thread_stack_t* ptr = get_backtrace_info();
    if (ptr->current > 0) {
        // why need -4 here?
        ptr->frame[ptr->current--] = (void*)((unsigned long)call_site - 4);
        // ptr->frame[ptr->current--] = call_site;
    }
    printf("enter %p, caller %p\n", this_func, call_site);
}

__attribute__((no_instrument_function)) void __cyg_profile_func_exit(void* this_func, void* call_site) {
    pthread_once(&sBackTraceOnce, init_once);
    thread_stack_t* ptr = get_backtrace_info();
    if (++ptr->current >= STACK_TRACE_DEPTH) {
        ptr->current = STACK_TRACE_DEPTH - 1;
    }
}

__attribute__((no_instrument_function))
static int get_tls_backtrace(void** backtrace, int max) {
    pthread_once(&sBackTraceOnce, init_once);
    int count = max;
    int i;
    thread_stack_t* ptr = get_backtrace_info();
    if (STACK_TRACE_DEPTH - 1 - ptr->current < count) {
        count = STACK_TRACE_DEPTH - 1 - ptr->current;
    }
    if (count > 0) {
        memcpy(backtrace, &ptr->frame[ptr->current + 1], sizeof(void*) * count);
    }
    printf("in func %s():\n", __func__);
    // here frame 0 addr is 0, so no need to print.
    for (i = 1; i <= count; i++) {
        printf("frame[%d] addr:0x%zx\n", ptr->current + i, ptr->frame[ptr->current + i]);
    }
    return count;
}

static void bar(void) {
    int i;
    unsigned long backtrace[STACK_TRACE_DEPTH];
    void* ptr = &backtrace;
    int count = get_tls_backtrace(ptr, STACK_TRACE_DEPTH);
}

static void foo(void) {
    bar();
}

int main(void) {
    foo();
    return 0;
}
