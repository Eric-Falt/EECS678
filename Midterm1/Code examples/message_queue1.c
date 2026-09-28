/* Simple example using message queues within the same process */

#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define MSG_LENGTH 1000
#define BIG_LENGTH 10000

struct msg_buf{
  long mtype;
  char buffer[MSG_LENGTH];
};

int main()
{
  /* identifier for the message queue */
  int queue_id;
  /* send and receive message buffers */
  struct msg_buf send_buf, recv_buf;
  char str[BIG_LENGTH];

  /* create a message queue */
  queue_id = msgget(1, S_IRUSR|S_IWUSR|IPC_CREAT);

  /* send a message to the queue */
  send_buf.mtype = 1;
  strcpy(send_buf.buffer, "EECS 678 Class");
  msgsnd(queue_id, (struct msg_buf*)&send_buf, sizeof(send_buf), 0);

  /* get the message from the queue */
  msgrcv(queue_id, (struct msg_buf *)&recv_buf, sizeof(recv_buf), 1, 0);
  printf("%ld\n", recv_buf.mtype);
  printf("%s\n", recv_buf.buffer);

  /* delete the message queue, and deallocate resources */
  msgctl(queue_id, IPC_RMID, NULL);

  return 0;
}
