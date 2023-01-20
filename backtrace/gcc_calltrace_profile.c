// gcc -o test test.c -finstrument-functions && test
#include <stdio.h>

__attribute__((no_instrument_function)) void __cyg_profile_func_enter(void* this_fn, void* call_site) {
    printf("entering %p, called by %p\n", this_fn, call_site);
}

__attribute__((no_instrument_function)) void __cyg_profile_func_exit(void* this_fn, void* call_site) {
    printf("exiting %p, called by %p\n", this_fn, call_site);
}

static void bar(void) {
    printf("Hello\n");
}

static void foo(void) {
    bar();
}

int main(void) {
    foo();
    return 0;
}

/**
output:
entering 0x4006a4, called by 0x7f723c590545
entering 0x400677, called by 0x4006c3
entering 0x400645, called by 0x400691
Hello
exiting 0x400645, called by 0x400691
exiting 0x400677, called by 0x4006c3
exiting 0x4006a4, called by 0x7f723c590545
// use addr2line to get the function name related with addr.
addr2line -e ./test -f 0x400677
foo
test.c:18
*/