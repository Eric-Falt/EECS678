#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#define NUM 100
  
int main()
{
  /* identifier for the shared memory segment */
  int segment_id;
  /* pointer to the shared memory segment */
  int *shared_array;
  /* size of the shared memory segment */
  int i, size;
  pid_t pid;
  int status;

  /* allocate a shared memory segment */
  size = sizeof(int) * NUM + 1;
  segment_id = shmget(IPC_PRIVATE, size, S_IRUSR|S_IWUSR);

  /* attach the shared memory segment */
  shared_array= (int *) shmat(segment_id, NULL, 0);

  /* initialize shared and unshared regions to INIT*/
  for(i=0 ; i<NUM ; i++){
    shared_array[i] = 0;
  }

  /*create a child process */
  if((pid = fork()) == -1){
    fprintf(stderr, "Error in fork\n");
    exit(0);
  }
  else if(pid == 0){
    /* child process */
    shared_array[0] = 100;
    exit(0);
  }
  else{
    /* parent process */
    /* wait for child process to finish */
    wait(&status);
    printf("shared memory: %d %d\n", shared_array[0], shared_array[1]);
    
    /* detach the shared memory segment */
    shmdt(shared_array);

    /* remove the shared memory segment */
    shmctl(segment_id, IPC_RMID, NULL);

    return 0;
  }
}
