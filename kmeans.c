
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#define SIZE 10
#define K 3

void init_centroid(float[SIZE][2]);
void printRandoms(int , int , int, int[]);
void assign_clusters(float[SIZE][2], float[K][2]);

int main()
{
    char c[1000];
    FILE *fptr;
    float data[SIZE][2];
    if ((fptr = fopen("data.txt", "r")) == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    char delim[] = ",";
    int i1=0,j1=0;
    while((fgets (c, 1000, fptr))!= NULL) {
        char *ptr = strtok(c, delim);
	while(ptr != NULL)
	{
		data[i1][j1]= atof(ptr);
		ptr = strtok(NULL, delim);
		j1++;
	}
	i1++;
	j1=0;
	
    }
    
    
    init_centroid(&data);
    
    fclose(fptr);
    return 0;
}

void printRandoms(int lower, int upper, int count, int arr[]) 
{ 
    int i; 
    for (i = 0; i < count; i++) { 
        int num = (rand()%(upper-lower+1))+lower; 
        arr[i] = num;
        
    } 
}

void init_centroid(float data[SIZE][2]){
    srand(time(0));
    int arr[K];
    printRandoms(0, SIZE-1, K, arr);
    float centroids[K][2];
    
    for(int i=0;i<K;i++){
    	centroids[i][0] = data[arr[i]][0];
    	centroids[i][1] = data[arr[i]][1];
    }
    assign_clusters(data, centroids);
}

void assign_clusters(float data[SIZE][2], float centroids[K][2]){	// SIZE = size of dataset, K = cluster size both are defined above
 //TODO
}

