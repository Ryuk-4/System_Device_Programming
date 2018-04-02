#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "pthread.h"
#include "semaphore.h"

sem_t *mutex, *sem0, *ready;

struct arg{  //arguments of threads
     int g;
     int n1;  // #values
     int n2;  // #values
};

void randEven(int* ptr, int len);//Random even number
void randOdd(int*ptr, int len);//Random odd numbe
int writeBin(int* ptr, int len, char* c);//System call write()
void* C1(void*);//Client1
void* C2(void*);//Client2

int main(int argc, char** argv){//Server
	
/*****************************************************************************/
/*                         Lab 2.3 (First Part)                              */
/*****************************************************************************/
    struct arg t;//Arguments to give to threads

	if(argc < 3){
		fprintf(stderr,"ERROR! -> Usage: %s n1 n2\n", argv[0]);
        exit(1);//Exit with failure
	}
	
	t.n1 = atoi(argv[1]);
	t.n2 = atoi(argv[2]);

    int v1[t.n1];//vector where I store even numbers
    int v2[t.n2];//vector where I store odd numbers
    
    randEven(v1, t.n1); //fill v1 with random even number [10-100]
    randOdd(v2, t.n2);  //fill v2 with random odd number [21-101]
    
    if( (writeBin(v1, t.n1, "fv1.b")) == 1) return 1;//check if everything is OK
    if( (writeBin(v2, t.n2, "fv2.b")) == 1) return 1;//check if everything is OK
    
/*****************************************************************************/
/*                  		Lab 2.3 (Second Part) 			                 */
/*****************************************************************************/
    int counter;//count how many request...

    pthread_t th_b, th_c;
    
    mutex = (sem_t*)malloc(sizeof(sem_t));//create semaphores
    sem0 = (sem_t*)malloc(sizeof(sem_t));
    ready = (sem_t*)malloc(sizeof(sem_t));
    
	sem_init(mutex, 0, 1);//initialize the semaphores    
	sem_init(sem0, 0, 0);	
    sem_init(ready, 0, 0);

    t.g = 0;
    counter = 0;

    pthread_create(&th_b, NULL, C1, (void*) &t);//create threads
    sleep(1);
    pthread_create(&th_c, NULL, C2, (void*) &t);
    
    while(counter<(t.n1+t.n2)){
        sem_wait(sem0);

		counter++;        
        t.g = 3 * t.g;

		sem_post(ready);
    }
    
	pthread_join(th_b, NULL);
	pthread_join(th_c, NULL);
    printf("The server \"main\" has served %d request from the clients;\n", counter);
    return 0;
}

void randEven(int* ptr, int len){
    //It generates random even number[10-100]
    int i, p;
    srand(time(NULL));
    for(i=0; i<len; i++){
        p=(rand()%90)+10;
            if(((p%2)== 0))
            {ptr[i]=p;}
            else
            {ptr[i]=p+1;}
    }
}

void randOdd(int*ptr, int len){
    //It generates random odd number[21-101]
    int i, p;
    srand(time(NULL));
    for(i=0; i<len; i++){
        p=(rand()%80)+21;
            if(((p%2)!= 0))
            {ptr[i]=p;}
            else
            {ptr[i]=p+1;}
    }
}

int writeBin(int* ptr, int len, char* c){
    int fdo;// file descriptor
    if ((fdo = open(c, O_CREAT | O_WRONLY, 0777)) < 0){
        fprintf(stderr," error open %s\n", c);
        return 1;//Exit with failure
    }
    
    write(fdo, ptr, len*sizeof(int));
    
    if ( close(fdo) < 0){
        fprintf(stderr," error close %s\n", c);
        return 1;//Exit with failure
    }
    return 0;//Exit with success
}

void* C1(void* a){ //Client1
    int fdo;// file descriptor
    int i, *v;

    struct arg* th;
    th = (struct arg*) a;
	//open the file
    if ((fdo = open("fv1.b", O_RDONLY, 0777)) < 0){
        fprintf(stderr," error open %s\n", "fv1.b");
        exit(1);//Exit with failure
    }
	//read the sequence of bits and store them into an array 
    v=(int*)malloc((th->n1)*sizeof(int));
    read(fdo, v, (th->n1)*sizeof(int));
	//colse the file
    if ( close(fdo) < 0){
        fprintf(stderr," error close %s\n", "fv1.b");
        exit(1);//Exit with failure
    }
	//loop the call to the server
    for(i=0; i<(th->n1); i++){
		sem_wait(mutex);
	//store into g the read value
        (th->g) = v[i];
	//call the server and wait before to print		   
        sem_post(sem0);
		sem_wait(ready);	  

		printf("[C1]: tID = %ld --> g = %d\n", pthread_self(), th->g); 
	
		sem_post(mutex);			
	}
    free(v);//free the memory

    pthread_exit(0);
}

void* C2(void* a){ //Client2
    int fdo;// file descriptor
    int i, *v;
	
    struct arg* th;
    th = (struct arg*) a;
	//open the file
    if ((fdo = open("fv2.b", O_RDONLY, 0777)) < 0){
        fprintf(stderr," error open %s\n", "fv2.b");
        exit(1);//Exit with failure
    }
	//read the sequence of bits and store them into an array
    v=(int*)malloc((th->n2)*sizeof(int));
    read(fdo, v, (th->n2)*sizeof(int));
	//close the file
    if ( close(fdo) < 0){
        fprintf(stderr," error close %s\n", "fv2.b");
        exit(1);//Exit with failure
    }
	//loop the call to the server
    for(i=0; i<(th->n2); i++){     
		sem_wait(mutex);
	//store into g the read value
        (th->g) = v[i];
	//call the server and wait before to print
		sem_post(sem0);
		sem_wait(ready);       

		printf("[C2]: tID = %ld --> g = %d\n", pthread_self(), th->g); 
		
		sem_post(mutex);
    }
	free(v);//free the memory

    pthread_exit(0);
}
