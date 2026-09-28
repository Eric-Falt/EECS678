/* Simple example using message queues within the same process */

#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_LENGTH 1000
#define PATH_NAME "/users/kulkarni/courses/EECS678/examples"

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
  /*message queue identified by unique key*/
  key_t key;

  /* create a message queue */
  key = ftok(PATH_NAME, 0);
  queue_id = msgget(key, S_IRUSR|S_IWUSR|IPC_CREAT);

  /* send messages to the queue */
  send_buf.mtype = 1;
  strcpy(send_buf.buffer, "Message type 1: First Message");
  msgsnd(queue_id, (struct msg_buf*)&send_buf, sizeof(send_buf), 0);

  send_buf.mtype = 2;
  strcpy(send_buf.buffer, "Message type 2: First Message");
  msgsnd(queue_id, (struct msg_buf*)&send_buf, sizeof(send_buf), 0);

  send_buf.mtype = 1;
  strcpy(send_buf.buffer, "Message type 1: Second Message");
  msgsnd(queue_id, (struct msg_buf*)&send_buf, sizeof(send_buf), 0);

  send_buf.mtype = 2;
  strcpy(send_buf.buffer, "Message type 2: Second Message");
  msgsnd(queue_id, (struct msg_buf*)&send_buf, sizeof(send_buf), 0);
  
  /* get the message from the queue in different order*/
  msgrcv(queue_id, (struct msg_buf *)&recv_buf, sizeof(recv_buf), 2, 0);
  printf("%ld %s\n", recv_buf.mtype, recv_buf.buffer);

  msgrcv(queue_id, (struct msg_buf *)&recv_buf, sizeof(recv_buf), 2, 0);
  printf("%ld %s\n", recv_buf.mtype, recv_buf.buffer);

  msgrcv(queue_id, (struct msg_buf *)&recv_buf, sizeof(recv_buf), 1, 0);
  printf("%ld %s\n", recv_buf.mtype, recv_buf.buffer);

  msgrcv(queue_id, (struct msg_buf *)&recv_buf, sizeof(recv_buf), 1, 0);
  printf("%ld %s\n", recv_buf.mtype, recv_buf.buffer);
  
  /* delete the message queue, and deallocate resources */
  msgctl(queue_id, IPC_RMID, NULL);

  return 0;
}
