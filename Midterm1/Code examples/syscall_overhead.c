/*
 * use 'gcc -O3 -fno-inline -DGETPID syscall_overhead.c -o testgetpid' to make getpid system call
 * use 'gcc -O3 -fno-inline syscall_overhead.c -o testmyid' to call the getid() routine
 * 
 * Make sure you use "objdump" to verify that the binaries are generated as
 * expected, without optimizations getting rid of loops.
 * use 'time testgetpid' to get the total time when making 10M getpid calls
 * use 'time testmyid' to get the total time when calling 10M getid routines
 */ 

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

unsigned long long getid()
{
  static unsigned long long myid = 1001;

  myid += 1;
  return myid;
}
  
int main()
{
  unsigned long long i, j=0;
  for (i=0; i<9999999; i++) {
#ifndef GETPID
    j += getid();
#else
    j += getpid();
#endif
  }
  
  printf("%lld\n", j);
  return 0;
}
