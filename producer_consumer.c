#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

struct thread_info {    /* Used as argument to thread_start() */
  pthread_t thread_id;        /* ID returned by pthread_create() */
  int       thread_num;       /* Application-defined thread # */
};

pthread_cond_t empty = PTHREAD_COND_INITIALIZER;     // semphore producer requires
pthread_cond_t full = PTHREAD_COND_INITIALIZER;      // semphore consumer requires
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;  // mutex1 was used to control mutual access to "in" between producer
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;  // mutex2 was used to control mutual access to "out" between consumer

#define BUFFLEN  10                                  // total 10 buffer;
#define PROD_NUM  3                                  // number of producer
#define CONS_NUM  2                                  // number of consumer
int CONTINUE = 1;
int in = 0;                                          // in point to the next empty buffer
int out = 0;                                         // out point to the next full buffer

int buf[BUFFLEN];
int product = 20;

int empty_count = BUFFLEN;                          // initializer value of empty buffer
int full_count = 0;

void* producer(void* arg) { // arg: producer id
  // produce
  int* tid = arg;
  while (CONTINUE) {
    pthread_mutex_lock(&mutex1);
    while (empty_count <= 0)
      pthread_cond_wait(&empty, &mutex1);            // producer wait on condition empty
    // to this point the consumer was awake from consumer, and hold the mutex1
    buf[in] = ++product;
    printf("Producer: thread %d put product into buffer, buf[%d] = %d\n", *tid, in, product);
    in = (in + 1) % BUFFLEN;
    empty_count--;
    full_count++;
    usleep(30000);
    pthread_mutex_unlock(&mutex1);
    pthread_cond_signal(&full);
  }
  pthread_exit(NULL);
}

void* consumer(void* arg) {  // arg: consumer id
  int* tid = arg;
  while (CONTINUE) {
    pthread_mutex_lock(&mutex2);
    while (full_count == 0)                             // how to know if there is at least a full buffer?
      pthread_cond_wait(&full, &mutex2);                // consumer wait on condition full
    // to this point the consumer was awke from producer, and hold the mutex2
    int result = buf[out];                              // get out the result from the buffer
    printf("Consumer: thread %d get prodct from buffer, buf[%d] = %d\n", *tid, out, result);
    out = (out + 1) % BUFFLEN;
    usleep(30000);
    empty_count++;
    full_count--;
    pthread_mutex_unlock(&mutex2);
    pthread_cond_signal(&empty);
  }
  pthread_exit(NULL);
}

int main() {
  struct thread_info* prod_info = calloc(PROD_NUM, sizeof(struct thread_info));
  struct thread_info* cons_info = calloc(CONS_NUM, sizeof(struct thread_info));
  
  for (int tnum = 0; tnum < PROD_NUM; tnum++) {
    prod_info[tnum].thread_num = tnum + 1;
    int s = pthread_create(&prod_info[tnum].thread_id, NULL, producer, &prod_info[tnum].thread_num);
    if (s != 0) {
      printf("Create producer error\n");
      exit(1);
    }
    //    pthread_join(prod_info[tnum].thread_id, NULL);
  }

  for (int tnum = 0; tnum < CONS_NUM; tnum++) {
    cons_info[tnum].thread_num = tnum + 1;
    int s = pthread_create(&cons_info[tnum].thread_id, NULL, consumer, &cons_info[tnum].thread_num);
    if (s != 0) {
      printf("Create consumer error\n");
      exit(1);
    }
    //    pthread_join(cons_info[tnum].thread_id, NULL);
  }

  while (getchar() == 'q') 
    CONTINUE = 0;
}
  
