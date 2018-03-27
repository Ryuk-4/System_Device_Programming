#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>	

int main(int argc,char **argv){
	int c;
	int count, n;
	char *dir, command[80], line[10], path[10];
	FILE* fp;
		
	pid_t child;
	int status;

	if(argc != 3){
		fprintf(stderr, "Enter C and dir\n");
		return 1;
	}
	
	c = atoi(argv[1]);
	dir = argv[2];
	
	sprintf(command, "ls ./%s > list.txt", dir);
	system(command);

	count = 0;	

	fp = fopen("list.txt", "r");
	if(fp == NULL)
		fprintf(stderr, "Errore apertura file\n");

	while( fgets(line, sizeof(line), fp) != NULL){	
		n = strlen(line);
		line[n-1] = '\0';

		sprintf(path, "./%s/%s", dir, line);

		child = fork();
		if(child < 0){
			fprintf(stderr, "Errore fork\n");
			break;
		}
		else if(child == 0){
			printf("I'm sorting %s\n", path);
			execlp("sort" ,"mysort", "-n", "-o", path, path, (char *) NULL);	
		}else{
			count++;

            if(count >= c){
                 wait(&status);
                 count--;
            }

		}
	}
	fclose(fp);
	
	while(wait(&status) > 0);

	system("cat ./data/f* > all_sorted.txt");
	system("sort -n -o all_sorted.txt all_sorted.txt");	

	return 0;
}
