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

