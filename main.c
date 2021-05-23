#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

#define NUM_OF_COMMENTATORS 5
pthread_mutex_t hello_msg_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t msg_created_cond = PTHREAD_COND_INITIALIZER; 
char * HELLO_MESSAGE; 
int msg_initialized = 0; 


void *moderator_function(void *arg){
  
  
  pthread_mutex_lock(&hello_msg_lock);
  msg_initialized = 1;
  printf("Questionnn\n");

  pthread_cond_signal(&msg_created_cond); 
  pthread_mutex_unlock(&hello_msg_lock); 

}

void *commentator_function (void *arg) {
    int threadNum = (int)arg;
    pid_t tid = syscall(SYS_gettid);
    

    pthread_mutex_lock(&hello_msg_lock);
    printf("%d", msg_initialized); 
    while(msg_initialized == 0){
        pthread_cond_wait(&msg_created_cond,&hello_msg_lock);   
    }
    printf("I am in thread no : %d with Thread ID : %d\n",threadNum,(int)tid);
    pthread_mutex_unlock(&hello_msg_lock); 
    pthread_cond_signal(&msg_created_cond);
    

}

int main(int argc, char *argv[]){

    pthread_t commentators[NUM_OF_COMMENTATORS];    
    pthread_t moderator[1];

    int creation_status = 0;
    for (int i=0; i<NUM_OF_COMMENTATORS; i++){
        pthread_t thread;
        commentators[i] = thread; 
        creation_status = pthread_create(&commentators[i], NULL, commentator_function, (void *)i);
    }
    
    pthread_t thread; 
    moderator[0] = thread;
    creation_status = pthread_create(&moderator[0], NULL, moderator_function, (void *)0);
     
    pthread_exit(NULL);
    return 0;
}

/*
int mails = 0;
pthread_mutex_t mutex;

void* routine() {
    for (int i = 0; i < 10000000; i++) {
        pthread_mutex_lock(&mutex);
        mails++;
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char* argv[]) {
    pthread_t th[8];
    int i;
    pthread_mutex_init(&mutex, NULL);
    for (i = 0; i < 8; i++) {
        if (pthread_create(th + i, NULL, &routine, NULL) != 0) {
            perror("Failed to create thread");
            return 1;
        }
        printf("Thread %d has started\n", i);
    }
    for (i = 0; i < 8; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            return 2;
        }
        printf("Thread %d has finished execution\n", i);
    }
    pthread_mutex_destroy(&mutex);
    printf("Number of mails: %d\n", mails);
    return 0;
    }
    */
