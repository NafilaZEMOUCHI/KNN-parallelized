#include <stdio.h>
#include <math.h>
#include "knn_modules.c"

#define k 5
#define nbClass 3
void main()

{   // retrieveData(filename);
    char* filename = "Iris_flower_dataset.csv";
    instance dataset[144];
    retrieveData(filename , 144 , dataset);
    //checking
    int nbRecords = sizeof(dataset)/sizeof(instance);
    /*
    printf("\n\n number of instances in the dataset : %d \n\n",nbRecords );
    for(int i =0 ; i<nbRecords ; i++){
        printf("%d :  %f %f %f %f %d\n", i+1 ,dataset[i].sepalL , dataset[i].sepalW , dataset[i].petalL , dataset[i].petalW , dataset[i].irisType );
    }
    */
    
    //testing
    instance testInstance;
    initInstance(&testInstance,6.3,2.7,4.9,1.8,3 );

    int classOfA = knnThreads(testInstance , dataset , nbRecords , nbClass , k , 3);
    
    printf("the resulting class by knn : %d\n",classOfA);

}

