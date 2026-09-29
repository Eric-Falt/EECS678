/*
  pipe-sync.c: Use Pipe for Process Synchronization
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// Question: Update this program to synchronize execution of the parent and
// child processes using pipes, so that we are guaranteed the following
// sequence of print messages:
// Child line 1
// Parent line 1
// Child line 2
// Parent line 2

int main()
{
  char *s, buf[1024];
  int ret, stat;
  int pipe1[2];
  int pipe2[2];

  s = "Use Pipe for Process Synchronization\n";

  /* create pipe */
  pipe(pipe1);
  pipe(pipe2);

  ret = fork();

  if (ret == 0) {

    /* child process. */
    printf("Child line 1\n");

    /* Tell parent that Child line 1 has finished */
    write(pipe1[1], "x", 1);

    /* Wait for parent to finish Parent line 1 */
    read(pipe2[0], buf, 1);

    printf("Child line 2\n");

    /* Tell parent that Child line 2 has finished */
    write(pipe1[1], "x", 1);

  } else {

    /* parent process */

    /* Wait for child line 1 */
    read(pipe1[0], buf, 1);

    printf("Parent line 1\n");

    /* Tell child that Parent line 1 has finished */
    write(pipe2[1], "x", 1);

    /* Wait for child line 2 */
    read(pipe1[0], buf, 1);

    printf("Parent line 2\n");

    wait(&stat);
  }
}
