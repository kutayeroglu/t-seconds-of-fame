#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define NUM_OF_COMMENTATORS 5
pthread_mutex_t hello_msg_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t msg_created_cond = PTHREAD_COND_INITIALIZER; 
pthread_cond_t all_done_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t breaking_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t breaking_cond = PTHREAD_COND_INITIALIZER;
char * HELLO_MESSAGE; 
int msg_initialized = 0;
int q_num;
int current_q;
float prob;
int num_commentator;
int t_speak;
int all_done;
int visited;
int current_c;


void *moderator_function(void *arg){

//-n 5 -p 0.75 -q 3 -t 1
  visited = 0;
  int i=0;
  
  
  while (i<q_num) {
  	pthread_mutex_lock(&hello_msg_lock);
  	while(all_done<num_commentator){
	   pthread_cond_wait(&all_done_cond,&hello_msg_lock);   
  	}
//  	printf("here\n");
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
	    if(rand() <  prob * RAND_MAX) {
	    	current_c= threadNum;
	    	printf("I am in thread no : %d answering question %d\n",threadNum, current_q);
	    }
	    all_done++;
//	    printf("all done: %d\n", all_done);
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

void *breaking_function (void *arg) {
	
	
	struct timespec waitFor;
	struct timeval now;

//  printf("all done: %d current_q: %d\n", all_done, current_q);
    
    while(all_done!=num_commentator&&current_q!=q_num) {
        printf("here 1\n");
        gettimeofday(&now, NULL);
        waitFor.tv_sec = now.tv_sec;
        waitFor.tv_nsec = now.tv_usec*1000;
        waitFor.tv_sec += 1;
        pthread_mutex_lock(&breaking_lock);
        int r = pthread_cond_timedwait(&breaking_cond, &breaking_lock, &waitFor);
        pthread_mutex_unlock(&breaking_lock);
        printf("here 2\n");
    	if(rand() <  0.5 * RAND_MAX) {
    		printf("Breaking news!\n");
    		printf("Commentator #%d is cut short due to breaking news\n", current_c);
    		sleep(5);
    		printf("Breaking news ends\n");
    		
    	}
    	
    }
    printf("all done: %d current_q: %d\n", all_done, current_q);
}

int main(int argc, char *argv[]){

    char dummy1[50];
	int param_counter = 1; 

    printf("Please enter: -n [commentators] -p [probability] -q [questions]  -t [time]\n");
	scanf("%[^\n]", dummy1);
	char* token = strtok(dummy1, " ");

	do{
		if((strcmp("-n", token)==0) || (strcmp("-p", token)==0) ||  (strcmp("-q", token)==0) || (strcmp("-t", token)==0)) {
			token = strtok(NULL, " ");
			continue;
		}

		if(param_counter == 1){
			num_commentator = atoi(token);
		} 
		else if (param_counter == 2){
			prob = atof(token);
		}
		else if (param_counter == 3){
			q_num = atoi(token);
		}
		else if (param_counter == 4){
			t_speak = atoi(token);
		}

		token = strtok(NULL, " ");
		param_counter++;
	}
	while(token != NULL && param_counter < 5);

    current_q=1;
    all_done=num_commentator;
    current_c=-1;
    srand(time(0));

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
    
    pthread_t news;
    creation_status = pthread_create(&news, NULL, breaking_function, (void *)0);
     
    pthread_exit(NULL);
    return 0;
}