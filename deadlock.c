#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t resd = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rese = PTHREAD_MUTEX_INITIALIZER;
int CONTINUE = 1;

void* maybedead1(void* arg) {
  while (CONTINUE) {
    printf("before lock in maybeded1 thread\n");
    pthread_mutex_lock(&resd);
    //    usleep(3000);
    pthread_mutex_lock(&rese);
    printf("Acquires two locks in maybeded1 thread\n");
    pthread_mutex_unlock(&rese);
    pthread_mutex_unlock(&resd);
  }
  printf("maybeded1 thread about to exit...\n");
  pthread_exit(NULL);
}

void* maybedead2(void* arg) {
  while (CONTINUE) {
    printf("before lock in maybeded2 thread\n");
    pthread_mutex_lock(&rese);
    //    usleep(3000);
    pthread_mutex_lock(&resd);
    printf("Acquires two locks in maybeded2 thread\n");
    pthread_mutex_unlock(&rese);
    pthread_mutex_unlock(&resd);
  }
  printf("maybeded2 thread about to exit...\n");
  pthread_exit(NULL);
}

int main() {
  pthread_t thread1, thread2;
  int s;
  if ((s = pthread_create(&thread1, NULL, maybedead1, NULL)) != 0) {
    printf("create thread1 error\n");
    exit(1);
  }

  if ((s = pthread_create(&thread1, NULL, maybedead2, NULL)) != 0) {
    printf("create thread2 error\n");
    exit(2);
  }

  while (getchar() == 'q')
    CONTINUE = 0;
  
}
