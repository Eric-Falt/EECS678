/* Program to show the use of shared memory across
   unrelated processes*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define PATH_NAME "/users/kulkarni/courses/EECS678/examples"

int main()
{
  /* identifier for the shared memory segment */
  int segment_id;
  /* pointer to the shared memory segment */
  char *shared_memory;
  /* size of the shared memory segment */
  const int size = 4096;
  key_t key;

  /* create a message queue */
  key = ftok(PATH_NAME, 0);

  /* allocate a shared memory segment */
  segment_id = shmget(key, size, IPC_CREAT|S_IRUSR|S_IWUSR);

  /* attach the shared memory segment */
  shared_memory = (char *) shmat(segment_id, NULL, 0);

  /* write a new message to the shared memory segment */
  sprintf(shared_memory, "EECS 678");

  /* Pre-existing contents of shared memory */
  printf("Original Process 1: %s\n", shared_memory);

  while((strncmp(shared_memory, "EECS 678", 8)) == 0);

  /* New contents of shared memory */
  printf("Original Process 2: %s\n", shared_memory);

  /* detach the shared memory segment */
  shmdt(shared_memory);

  /* remove the shared memory segment. Segment will stay if not deleted here. */
  shmctl(segment_id, IPC_RMID, NULL);

  return 0;
}
