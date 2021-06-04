#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <string.h>

#define NUM_OF_COMMENTATORS 5
pthread_mutex_t hello_msg_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t msg_created_cond = PTHREAD_COND_INITIALIZER; 
pthread_cond_t all_done_cond = PTHREAD_COND_INITIALIZER;
char * HELLO_MESSAGE; 
int msg_initialized = 0;
int q_num;
int current_q;
float prob;
int num_commentator;
int t_speak;
int all_done;
int visited;


void *moderator_function(void *arg){

//-n 5 -q 2 -p 1 -t 1
  visited = 0;
  int i=0;
  
  
  while (i<q_num) {
  	pthread_mutex_lock(&hello_msg_lock);
  	while(all_done<num_commentator){
	   pthread_cond_wait(&all_done_cond,&hello_msg_lock);   
  	}
  	printf("here\n");
  	all_done=0;
  	if (visited==1) {current_q++;}
	msg_initialized = 1;
	printf("Moderator asks question %d\n", current_q);
 	if (visited==0) {visited=1;}
	pthread_cond_signal(&msg_created_cond); 
	pthread_mutex_unlock(&hello_msg_lock);
 	i++;
  }

  
  
}

void *commentator_function (void *arg) {
//	printf("this is commentator %d\n", current_q);
    int threadNum = (int)arg;
    pid_t tid = syscall(SYS_gettid);
    int q_left[q_num];
    
    for (int i=0; i<q_num; i++) {
    	q_left[i]=1;
    }
    while(1==1) {
	if (q_left[current_q-1]==1) {
	    pthread_mutex_lock(&hello_msg_lock);
//	    printf("%d", msg_initialized); 
	    while(msg_initialized == 0){
		pthread_cond_wait(&msg_created_cond,&hello_msg_lock);   
	    }
	    printf("I am in thread no : %d answering question %d\n",threadNum, current_q);
	    all_done++;
	    printf("all done: %d\n", all_done);
	    if(all_done==num_commentator) {
    	    	pthread_cond_signal(&all_done_cond);
//    	    	msg_initialized
    	    }
	    pthread_mutex_unlock(&hello_msg_lock); 
	    pthread_cond_signal(&msg_created_cond);
    	    q_left[current_q-1]=0;
    	    if(current_q==q_num){break;}//printf("about to break\n");
	}
    }

}

int main(int argc, char *argv[]){

//    char* input[256];
    char* dummy1;
    char* dummy2;
    char* dummy3;
    char* dummy4;
    printf("Please enter: -n [commentators] -q [questions] -p [probability] -t [time]\n");
    scanf("%s %d %s %d %s %f %s %d", &dummy1, &num_commentator, &dummy2, &q_num, &dummy3, &prob, &dummy4, &t_speak);
    
    current_q=1;
    all_done=num_commentator;
    //visited=0;

    pthread_t commentators[num_commentator];    
    pthread_t moderator[1];

    int creation_status = 0;
    for (int i=0; i<num_commentator; i++){
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
printf("%s", input);
    if (input==NULL) {
    	return 0;
    } else {
    	char *token;
    	token = strtok(input, " \t");
    	printf("%s", token);
    	while (token!=NULL) {
    		if (strcmp(token, "-q")==0) {
    			token = strtok(NULL, input);
    			if (token!=NULL) { //check if integer
    				q_num = atoi(token);
    			} else {
    				printf("Wrong input format\n");
    				return 0;
    			}
    		} else if (strcmp(token, "-p")==0) {
    			token = strtok(NULL, input);
    			if (token!=NULL) { //some other exceptions we can check
    				prob = atoi(token);
    			} else {
    				printf("Wrong input format\n");
    				return 0;
    			}
    		} else if (strcmp(token, "-n")==0) {
    			token = strtok(NULL, input);
    			printf("%s", token);
    			if (token!=NULL) { //some other exceptions we can check
    				num_commentator = atoi(token);
    			} else {
    				printf("Wrong input format\n");
    				return 0;
    			}
    		} else if (strcmp(token, "-t")==0) {
    			token = strtok(NULL, input);
    			if (token!=NULL) { //some other exceptions we can check
    				t_speak = atoi(token);
    			} else {
    				printf("Wrong input format\n");
    				return 0;
    			}
    		} else {
    			printf("Wrong input format\n");
    			return 0;
    		}
    		token = strtok(NULL, input);
    	}
    }
*/
