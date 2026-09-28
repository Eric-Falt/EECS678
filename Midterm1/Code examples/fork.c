#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  pid_t ret;
  int status;

  ret = fork();

  if(ret < 0){
    fprintf(stderr, "fork failed\n");
    exit(-1);
  }
  else if(ret == 0){
    printf("In Child: %d, Parent: %d\n", getpid(), getppid());
    execlp("/bin/whoami", "-l", NULL);
  }
  else {
    wait(&status);
    if(WIFEXITED(status))
      printf("Child complete: %d\n", WEXITSTATUS(status));
    exit(0);
  }

  printf("Final statement from Process: %d\n", getpid());
}

