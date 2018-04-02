#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "semaphore.h"
#include "pthread.h"


int n;
sem_t *sem;

typedef struct Arg{
	int nLevel;
	pthread_t* v;
	int index;
}Argument;

void* C(void* a){
	pthread_t t1, t2;
	Argument arg, *ar;
	ar = (Argument*) a;
	
	arg.index =  ar->index;
	arg.nLevel = ar->nLevel;
	arg.v = (pthread_t*)malloc((n+1)*sizeof(pthread_t));		
	memcpy(arg.v, ar->v, (n+1)*sizeof(pthread_t));
	
	arg.index++;
	arg.v[arg.index] = pthread_self();

	pthread_detach(pthread_self());
	
	sem_wait(sem);//The semaphore needs in order to print before 
				  //the left SubTree and then the right one.	
	if(arg.nLevel < n){
		arg.nLevel++;
		
		pthread_create(&t1, NULL, C, (void*) &arg);
		pthread_create(&t2, NULL, C, (void*) &arg);
		
		sleep(1);//It needs to give time to copy the memory 
				 //address of the vector before to free it.
	}else{
		int i = 0;
		printf("Thread Tree: ");
		for(i = 0; i < n+1; i++){
			printf("%ld ", arg.v[i]);		
		}
		printf("\n");
	}	
	free(arg.v);	
	sem_post(sem);
	pthread_exit(0);
}

int main(int argc, char** argv){

	if(argc != 2){
		fprintf(stderr, "Error! -> Usage:%s n\n", argv[0]);
		return 1;
	}
	
	pthread_t t1, t2;	
	Argument arg;
	
	n = atoi(argv[1]);
	arg.v = (pthread_t*)malloc((n+1)*sizeof(pthread_t));	
	sem = (sem_t*)malloc(sizeof(sem_t));
	
	sem_init(sem, 0, 1);
	
	arg.index = 0; //How many thread ids have been stored	
	arg.v[0] = pthread_self();

	arg.nLevel = 1;

	pthread_create(&t1, NULL, C, (void*) &arg);
	pthread_create(&t2, NULL, C, (void*) &arg);
	
	sleep(1);

	free(arg.v);
	pthread_exit(0);
}
