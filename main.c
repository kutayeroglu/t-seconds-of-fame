#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <string.h>

#define NUM_OF_COMMENTATORS 5
pthread_mutex_t hello_msg_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t msg_created_cond = PTHREAD_COND_INITIALIZER; 
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
  //visited = 1;
  int i=0;
  while (all_done==num_commentator && i<q_num) {
  	all_done=0;
  	pthread_mutex_lock(&hello_msg_lock);
	msg_initialized = 1;
	printf("Moderator asks question %d\n", current_q);
	pthread_cond_signal(&msg_created_cond); 
	pthread_mutex_unlock(&hello_msg_lock);
 	i++;
 	if (all_done==num_commentator) {
 		current_q++;
 	}
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
    
	while (q_left[current_q-1]==1) {
	    pthread_mutex_lock(&hello_msg_lock);
//	    printf("%d", msg_initialized); 
	    while(msg_initialized == 0){
		pthread_cond_wait(&msg_created_cond,&hello_msg_lock);   
	    }
	    printf("I am in thread no : %d answering question %d\n",threadNum, current_q);
	    all_done++;
	    pthread_mutex_unlock(&hello_msg_lock); 
	    pthread_cond_signal(&msg_created_cond);
    	    q_left[current_q-1]=0;
    	    
	}

}

int main(int argc, char *argv[]){

//    char* input[256];
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

	printf("%d\n", num_commentator);
	printf("%f\n", prob);
	printf("%d\n", q_num);
	printf("%d\n", t_speak);

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
