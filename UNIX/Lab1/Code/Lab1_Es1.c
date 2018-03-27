#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char** argv){
	if(argc != 3){
		fprintf(stderr, "Enter n and k\n");
		return 1;
	}

	int n, k;

	system("mkdir ./data");

	n = atoi(argv[1]);
	k = atoi(argv[2]);
	
	/*Just for Debug*/
	printf("n = %d\nk = %d\n", n, k);
	
	srand(time(NULL));
	
	int i, j, x, y;
	FILE* fp;
	char *str;

	str = malloc(4*sizeof(char));

	for(i=0; i<n; i++){
		sprintf(str, "./data/f%02X", i);
		fp = fopen(str, "w+");
		x = (rand()%(k-1))+1;

		for(j=0; j<x; j++){
			y = (rand()%(k-1))+1;
			fprintf(fp, "%d\n", y);
		} 
		
		fclose(fp);
	}

	return 0;
}
