
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include "parser2.h"
#include "staticMapApi.h"
#include "recommend.h"


void printmaps(float[][2], float[][2], int[], int, int);
int elbowPoint(float[][2], float[][2], int[], int, float[]);
void getrecommendation(int[],int[],int[],float[][2], int[], int, int);
void swapFloat(float*, float*);
void swapInt(int*, int*);
void sortdata(int[], int[], int[], int);
void init_centroid(float[][2], float[][2], int[], int, int);
void printRandoms(int , int , int, int[]);
int get_corres_cluster(double dist[][10],int, int, int);
int assign_clusters(float[][2], float[][2], int[], int, int);
int get_new_cent( float[][2],int[], float[][2], int, int);
int outly(float[][2], float[][2], int[], int, int);

int main()
{
    int SIZE;
    int K;
    printf("Enter the size of the data\n");
    scanf("%d",&SIZE);
    printf("Enter the value of K\n");
    scanf("%d",&K);
    float cent[K][2];
    float data[SIZE][2];
    int ncluster[SIZE];
    int nclus[SIZE];
    int pos[SIZE];
    int likes[SIZE];
    char *color[10] = {"red", "yellow", "green", "black", "brown", "purple", "blue", "gray", "orange", "white"};
    //calc();  // to run parser.h
    char c[1000];
    FILE *fptr;
    //double x[SIZE],y[SIZE];
    //float cent[K][2];
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
    int loop=1;
    while(loop){
       int choice;
       printf("Enter your choice\n");
       printf("1- Calculate Elbow Point\n" );
       printf("2- Run K means Algorithm\n" );
       printf("3- Find the Recommended Cluster\n" );
       printf("4- Exit\n" );
       scanf("%d",&choice);
    
       switch(choice) {
          case 1 :
             printf("\nValues of SSE are\n");
             float Sse[10];
             elbowPoint(data, cent, ncluster, SIZE, Sse);
             for(int i=0; i<10;i++){
             printf("%f  ",Sse[i]);
             }
             break;
  
          case 2 :
             printf("\nStarting K-means Algorithm\n");
             init_centroid(data, cent, ncluster, SIZE, K);
             for(int i=0;i<1000;i++){
                 printf("\nIteration %d:\n\n",i+1);
    	         assign_clusters(data, cent, ncluster, SIZE, K);
             }
             printmaps(data, cent, ncluster, SIZE, K);  //to print google static map
             break;
             
          case 3 :
             for(int i=0;i<SIZE;i++){
             nclus[i] = ncluster[i];
             pos[i] = i;
             }
             getrecommendation(likes, nclus, pos, data, ncluster, SIZE, K);
             break;
             
          case 4 :
             loop=0;
             break;
             
          default :
             printf("Invalid Choice\n" );
       }
       
   }
    fclose(fptr);
    return 0;
}

void getrecommendation(int likes[],int nclus[],int pos[],float data[][2], int ncluster[], int SIZE,int K){  
    getLikes(likes);
     
    sortdata(likes, nclus, pos, SIZE);
    int counter[K];
    for(int i=0;i<15;i++){
        if(ncluster[pos[i]]!=-1)
            ncluster[pos[i]]=3;
        for(int j=0;j<K;j++){
            if(nclus[i]==j)
               counter[j] = counter[j] + 1;
               
        }
    }
    int min=nclus[0],p=0;
    for(int i=1;i<K;i++){
        if(min>nclus[i]){
           p=i;
        }
    }
    char *color[10] = {"red", "yellow", "green", "black", "brown", "purple", "blue", "gray", "orange", "white"};
    printf("\nThe recommended cluster to open your outlet is %s\n",color[p]);
    
    char outfilename1[] = "final.png";
    
    char url[10000] = "http://maps.googleapis.com/maps/api/staticmap?center=40.730610,-73.935242";
    for(int j=0;j<K+1;j++){
       char str1[5000];
       for(int i=0;i<SIZE;i++){
           char temp1[20], temp2[20];
           if(ncluster[i]==j){
              strcat(str1, "|");
              sprintf(temp1, "%f", data[i][0]);
              strcat(str1, temp1);
              strcat(str1,",");
              sprintf(temp2, "%f", data[i][1]);
              strcat(str1, temp2);
           }
       }
       strcat(str1, "|");
       strcat(url,"&markers=color:");
       strcat(url,color[j]);
       strcat(url,str1);
    }
    strcat(url,"&zoom=14&size=600x600&key=AIzaSyBNbX62JrJbubjzxaggsI0rKjqt-t9Bkoo");
    plotMap(url,outfilename1);    
}

void swapInt(int *xp, int *yp) 
{ 
    int temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 

void swapFloat(float *xp, float *yp) 
{ 
    float temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 
  
 
void sortdata(int likes[], int nclus[], int pos[], int n) 
{ 
   int i, j; 
   for (i = 0; i < n-1; i++)   
       for (j = 0; j < n-i-1; j++)  
           if (likes[j] < likes[j+1]) {
              swapInt(&likes[j], &likes[j+1]); 
              swapInt(&nclus[j], &nclus[j+1]); 
              swapInt(&pos[j], &pos[j+1]); 
            }
}

int elbowPoint(float data[][2], float cent[][2], int ncluster[],int SIZE, float Sse[])
{
    float sse[11]={0};
    for(int i=1;i<=10;i++)
    {	
    	float cent1[i][2];
    	
        init_centroid(data, cent1, ncluster, SIZE, i);
        for(int l=0;l<100;l++)
        {
    	   assign_clusters(data, cent1, ncluster, SIZE, i);
        }
        for(int j=0;j<SIZE;j++)
        {
            for(int k=0;k<i;k++)
                {
                     if(ncluster[j]==k)
                      {
                        sse[i]=sse[i]+(sqrt( (data[j][0]-cent1[k][0])*(data[j][0]-cent1[k][0]) + (data[j][1]-cent1[k][1])*(data[j][1]-cent1[k][1])));
                      }
                }
        }
        Sse[i-1] = sse[i];
    }

   return 0;
}

void printmaps(float data[][2], float cent[][2], int ncluster[],int SIZE, int K){
    char outfilename1[] = "raw.png";
    char outfilename2[] = "refined.png";
    char *color[10] = {"red", "yellow", "green", "black", "brown", "purple", "blue", "gray", "orange", "white"};
    char url[10000] = "http://maps.googleapis.com/maps/api/staticmap?center=40.730610,-73.935242";
    for(int j=0;j<K;j++){
       char str1[5000];
       for(int i=0;i<SIZE;i++){
           char temp1[20], temp2[20];
           if(ncluster[i]==j){
              strcat(str1, "|");
              sprintf(temp1, "%f", data[i][0]);
              strcat(str1, temp1);
              strcat(str1,",");
              sprintf(temp2, "%f", data[i][1]);
              strcat(str1, temp2);
           }
       }
       strcat(str1, "|");
       strcat(url,"&markers=color:");
       strcat(url,color[j]);
       strcat(url,str1);
    }
    strcat(url,"&zoom=14&size=600x600&key=AIzaSyBNbX62JrJbubjzxaggsI0rKjqt-t9Bkoo");
    plotMap(url,outfilename1);
    
    outly(data, cent, ncluster, SIZE, K);


    char url1[10000] = "http://maps.googleapis.com/maps/api/staticmap?center=40.730610,-73.935242";
    for(int j=0;j<K;j++){
       char str1[5000]="";
       for(int i=0;i<SIZE;i++){

           char tem1[20], tem2[20];
           if(ncluster[i]==j){

              strcat(str1, "|");
              sprintf(tem1, "%f", data[i][0]);
              strcat(str1, tem1);
              strcat(str1,",");
              sprintf(tem2, "%f", data[i][1]);
              strcat(str1, tem2);
           }
       }
       strcat(str1, "|");
       strcat(url1,"&markers=color:");
       strcat(url1,color[j]);
       strcat(url1,str1);
    }
    strcat(url1,"&zoom=14&size=600x600&key=AIzaSyBNbX62JrJbubjzxaggsI0rKjqt-t9Bkoo");
    plotMap(url1,outfilename2);
}

void printRandoms(int lower, int upper, int count, int arr[])
{
    int i;
    for (i = 0; i < count; i++) {
        int num = (rand()%(upper-lower+1))+lower;
        arr[i] = num;

    }
}

void init_centroid(float data[][2], float centroids[][2], int ncluster[],int SIZE, int K){
    srand(time(0));
    int arr[K];
    printRandoms(0, SIZE-1, K, arr);


    for(int i=0;i<K;i++){
    	centroids[i][0] = data[arr[i]][0];
    	centroids[i][1] = data[arr[i]][1];
    }
    assign_clusters(data, centroids, ncluster, SIZE, K);
}

//iterating through every datapoint and assigning the clusters using the eucledian distance

// function to find out the corresponding cluster for a datapoint
int get_corres_cluster(double dist[][10],int i, int SIZE, int K)
{
   int j;
   double m;
   m=dist[i][0];
   for(j=0;j<K;j++)
    {
        if(m>dist[i][j])
        {
            m=dist[i][j];
        }
   }
    for(j=0;j<K;j++)
    {
        if(m==dist[i][j])
        {
            break;
        }
    }
    return j;
}

int assign_clusters(float data[][2], float centroids[][2], int ncluster[], int SIZE, int K){
   int i,j;
   double dist[SIZE][10];	//an array to store the cluster of each datapoint

   for(i=0;i<SIZE;i++)
   {
      for(j=0;j<K;j++)
      {
         dist[i][j]=sqrt( (data[i][0]-centroids[j][0])*(data[i][0]-centroids[j][0]) + (data[i][1]-centroids[j][1])*(data[i][1]-centroids[j][1]));
      }

   }
   printf("Cluster Array:\n");
   for(i=0;i<SIZE;i++)
   {
      ncluster[i]=get_corres_cluster(dist,i, SIZE, K);
      printf("%d ", ncluster[i]);	// calling the get_corres_cluster function to get the cluster of the corresponding data point
   }
   printf("\n");
   get_new_cent(data,ncluster,centroids, SIZE, K);
}

int get_new_cent(float data[][2], int cluster_n[], float centroids[][2], int SIZE, int K)
{
    float cent[K][2];
    float arr[K];
    for(int j=0;j<K;j++){
        arr[j] = 0.0;
        cent[j][0]=0;
        cent[j][1]=0;
    }
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<K;j++){
           if(cluster_n[i]==j) {
              cent[j][0]+=data[i][0];
              cent[j][1]+=data[i][1];
              arr[j] = arr[j]+1.0;
              break;
           }
        }
    }
    for(int j=0;j<K;j++){
        if(arr[j]==0){
           cent[j][0] = centroids[j][0];
           cent[j][1] = centroids[j][1];
        }else{
           centroids[j][0] = cent[j][0]/=arr[j];
           centroids[j][1] = cent[j][1]/=arr[j];
        }
    }
    printf("\nUpdated Centroids:");
    for(int i=0;i<K;i++){
    	printf("\n%f %f",centroids[i][0], centroids[i][1]);
    }
    printf("\n");
}
int outly(float data[][2], float cent[][2], int n_cluster[], int SIZE, int K) //function to detect and remove the outlier
{
    int i,j;
    double outf; //outlyginess factor
    double T=0.80; //threshold value
    double distance;
    double d_max[K];
    for(i=0;i<SIZE;i++)
    {
       for(j=0;j<K;j++)
         {
            if(n_cluster[i]==j)
            {
                 distance=sqrt( (data[i][0]-cent[j][0])*(data[i][0]-cent[j][0]) + (data[i][1]-cent[j][1])*(data[i][1]-cent[j][1]));
              if(distance>d_max[j])
                d_max[j]=distance;
               distance=0.0;
            }

         }
    }
    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<K;j++)
         {
          if(n_cluster[i]==j)
            {
                 outf=(sqrt( (data[i][0]-cent[j][0])*(data[i][0]-cent[j][0]) + (data[i][1]-cent[j][1])*(data[i][1]-cent[j][1])))/d_max[j];

              if(outf>T) //if the value of outf is greater than threshold that means the datapoint is an outlier
                n_cluster[i]=-1; // initializing the outlier -1
            }
          }
    }
    printf("\nCluster array after removing Outlier\n");

    for(int i=0;i<SIZE;i++)
    {
        printf("%d ",n_cluster[i] );
    }
    printf("\n");

}
