#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_var = 10;

void code_func()
{
    printf("Code   : %p\n", (void *)code_func);
}

int main()
{
    static int static_var = 20;
    int stack_var = 30;
    int *heap_var = malloc(sizeof(int));

    *heap_var = 40;

    code_func();
    printf("Global : %p\n", (void *)&global_var);
    printf("Static : %p\n", (void *)&static_var);
    printf("Heap   : %p\n", (void *)heap_var);
    printf("Stack  : %p\n", (void *)&stack_var);

    printf("PID: %d\n", getpid());
    sleep(30);

    free(heap_var);
    return 0;
}
