#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>

int sig1 = 0;
int sig2 = 0;

void catcherSig1(){
	if(sig1 == 0){
		printf("[Father]: I cannot print because of my child!!!\n");
		sig1 = 1;
	}else{ 
		printf("[Father]: At last, I can print again!!!\n");
		sig1 = 0;
	}
}

void catcherSig2(){
	printf("[Father]: Received SIGUSR2 from the child\n");
	sig2 = 1;
}

void catcherSig3(){
	printf("[Child]: Received SIGUSR2 from the grandchild\n");
	kill(getppid(), SIGUSR2);
	exit(0);
}

int main(int argc, char** argv){
	if(argc < 2){
		fprintf(stderr, "Usage: ./%s Name_File\n", argv[0]);
		return 1;
	}
	
	FILE* fp;
	int c, x, i;
	char line[80];
	
	i = 0;
	srand(time(NULL)); 
	signal(SIGUSR1, catcherSig1);
	signal(SIGUSR2, catcherSig2);
	
	c = fork();
	if(c == 0){ //child
		
		signal(SIGUSR2, catcherSig3);
		
		if(fork() != 0){ //still child
			while(1){
				x = (rand() % 10) + 1;
				sleep(x);
				kill(getppid() ,SIGUSR1);
			}
		}else{ //grandchild
			sleep(60);
			kill(getppid() ,SIGUSR2);
			exit(0);
			}
	}
	else if(c > 0){ //father
		fp = fopen(argv[1], "r");
		while(1){
			if(sig2 == 1){
				fclose(fp);
				exit(0);
			}else{
				while(fgets(line, sizeof(line), fp) != NULL){	
					if(sig1 == 0){
						line[strlen(line)-1] = '\0';
						i++;
						printf("Row %d: %s\n", i, line);
						fflush(stdout);
					}			
				}
				rewind(fp);
				i = 0;
			}	
		}	
	}
	else{ //error
		fprintf(stderr, "Error fork()!!!\n");
		return 1;
	}
		
	return 0;
}
