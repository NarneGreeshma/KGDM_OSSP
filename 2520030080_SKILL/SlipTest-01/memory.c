#include <stdio.h>
#include <stdlib.h>

int main() {
    char *ptr = malloc(1000000);

    if (ptr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    printf("Memory allocated successfully\n");

    free(ptr);
    return 0;
}
