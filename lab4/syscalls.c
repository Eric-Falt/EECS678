#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>

#define N 10000000

int main(void)
{
    struct timespec start, end;
    long value = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);

    //This is the part of the code that does work
    for (long i = 0; i < N; i++) {
        value += syscall(SYS_getpid);
    }
    //This ends the part of the code that does work

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Result: %ld\n", value);
    printf("Time: %.6f seconds\n", elapsed);

    return 0;
}
