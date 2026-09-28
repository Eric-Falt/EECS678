/* Pipe example for Process Synchronization */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
  char *s, buf[1024];
  int fds[2], ret;
  s  = "EECS 678. Pipe program 3\n";

  /* create a pipe */
  pipe(fds);

  ret = fork();
  if (ret == 0) {

    /* child process. */
    printf("Child line 1\n");
    printf("Child line 2\n");
    write(fds[1], buf, strlen(s));
  } else {

    /* parent process */
    read(fds[0], s, strlen(s));
    printf("Parent line 1\n");
    printf("Parent line 2\n");
    wait(NULL);
  }
}
