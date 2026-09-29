#include <stdio.h>
#include <time.h>

#define N 10000000

__attribute__((noinline))
long user_function(long x)
{
    return x + 1;
}

int main(void)
{
    struct timespec start, end;
    long value = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);

    //This is the part of the code that does work
    for (long i = 0; i < N; i++) {
        value = user_function(value);
    }
    //This is the end of the part of the code that does work

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Result: %ld\n", value);
    printf("Time: %.6f seconds\n", elapsed);

    return 0;
}
