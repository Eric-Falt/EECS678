/* IPC example using pipes */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
  char *s, buf[1024];
  int ret, fds[2];
  s  = "EECS 678. Pipe program 2\n";

  /* create a pipe */
  pipe(fds);

  /* create a new process using fork */
  ret = fork();
  
  if (ret == 0) {

    /* child process. All file descriptors, including pipe are inherited, and
       copied in the child address space */
    write(fds[1], s, strlen(s));
    exit(0);
  }

  /* parent process */
  read(fds[0], buf, strlen(s));
  write(1, buf, strlen(s));
}
