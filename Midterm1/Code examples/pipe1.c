/* Simple example using pipes. Single process */

#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main()
{
  char *s, buf[1024];
  int fds[2];
  s  = "EECS 678\n";

  /* open a pipe. fd[0] is opened for reading, and fd[1] for writing.*/
  pipe(fds);

  /* write to the write-end of the pipe */
  write(fds[1], s, strlen(s));

  /* This can be read from the other end of the pipe */
  read(fds[0], buf, strlen(s));

  printf("fds[0]=%d, fds[1]=%d\n", fds[0], fds[1]);
  printf("%s", buf);
}
