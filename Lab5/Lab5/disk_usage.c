#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define BSIZE 256

#define DU_EXEC "/usr/bin/du"
#define SORT_EXEC "/usr/bin/sort"
#define HEAD_EXEC "/usr/bin/head"
#define CUT_EXEC "/usr/bin/cut"

int main(int argc, char *argv[])
{

  int status;
  pid_t pid_1, pid_2, pid_3, pid_4;

  int p1[2], p2[2], p3[2];

  // Checking if input parameters DIR and NUM are correct
  if (argc != 3)
  {
    printf(stderr, "usage: %s DIRECTORY NUM_RESULTS\n", argv[0]);
    exit(0);
  }

  // STEP 1
  // Initialize pipes p1, p2, p3
  pipe(1);
  pipe(2);
  pipe(3);

  pid_1 = fork();
  if (pid_1 == 0)
  {
    /*First Child */

    // STEP 2
    // In this first child process, we want to send everything that is printed on the standard output, to the next child process through pipe p1
    // So, redirect standard output of this child process to p1's write end - written data will be automatically available at pipe p1's read end
    // And, close all other pipe ends except the ones used to redirect the above OUTPUT (very important)
    dup2(p1[1], STDOUT_FILENO);
    close(p1[0]);
    close(p2[1]);
    close(p2[0]);
    close(p3[1]);
    close(p3[0]);

    // STEP 3
    // Invoke execl for du (use DU_EXEC as the path)

    if ((execl(DU_EXEC, "du", argv[1], (char *)NULL)) < 0)
    {
      printf(stderr, "\n Error execing DU ERROR #%d\n", errno);
      return EXIT_FAILURE
    }
    exit(0);
  }

  pid_2 = fork();
  if (pid_2 == 0)
  {

    /* Second Child */

    // STEP 4
    // In this second child process, we want to receive everything that is available at pipe p1's read end, and use the received information as standard input for this child process
    // In this second child process, we want to send everything that is printed on the standard output, to the next child process through pipe p2
    // So, redirect standard output of this child process to p2's write end - written data will be automatically available at pipe p2's read end
    // And, close all other pipe ends except the ones used to redirect the above two INPUT/OUTPUT (very important)

    // STEP 5
    // Invoke execl for sort  (use SORT_EXEC as the path)

    exit(0);

    dup2(p1[0], STDIN_FILENO);
    dup2(p2[1], STDOUT_FILENO);
    close(p1[1]);
    close(p2[0]);
    close(p3[1]);
    close(p3[0]);

    if ((execl(SORT_EXEC, "nr", (char *)NULL)) < 0)
    {
      printf(stderr, "\n Error execing SORT ERROR #%d\n", errno);
      return EXIT_FAILURE;
    }
    exit(0);
  }

  pid_3 = fork();
  if (pid_3 == 0)
  {

    /* Third Child */

    // STEP 6
    // In this third child process, we want to receive everything that is available at pipe p2's read end, and use the received information as standard input for this child process
    // In this third child process, we want to send everything that is printed on the standard output, to the next child process through pipe p3
    // So, redirect standard output of this child process to p3's write end - written data will be automatically available at pipe p3's read end
    // And, close all other pipe ends except the ones used to redirect the above two INPUT/OUTPUT (very important)

    // STEP 7
    // Invoke execl for head (use HEAD_EXEC as path)

    dup2(p2[0], STDIN_FILENO);
    dup2(p3[1], STDOUT_FILENO);
    close(p1[0]);
    close(p1[1]);
    close(p2[1]);
    close(p3[0]);

    if ((execl(HEAD_EXEC, "n", arg[2], (char *)NULL)) < 0)
    {
      printf(stderr, "\n Error execing HEAD ERROR #%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }

  pid_4 = fork();
  if (pid_4 == 0)
  {
    // STEP 8
    // In this fourth child process, we want to receive everything that is available at pipe p3's read end, and use the received information as standard input for this child process
    // Output of this child process should directly be to the standard output and NOT to any pipe
    // And, close all other pipe ends except the ones used to redirect the above INPUT (very important)

    // STEP 9
    // Invoke execl for cut (use CUT_EXEC as path).

    dup2(p3[0], STDIN_FILENO);
    close(p1[0]);
    close(p1[1]);
    close(p2[1]);
    close(p2[0]);
    close(p3[1]);

    if ((execl(CUT_EXEC, "f2", (char *)NULL)) < 0)
    {
      printf(stderr, "\n Error execing CUT ERROR #%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }
  if ((waitpid(pid_1, &status, 0)) == -1)
  {
    fprintf(stderr, "Process 1 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_2, &status, 0)) == -1)
  {
    fprintf(stderr, "Process 2 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_3, &status, 0)) == -1)
  {
    fprintf(stderr, "Process 3 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_4, &status, 0)) == -1)
  {
    fprintf(stderr, "Process 4 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }

  return 0;
}
