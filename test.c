#include <stdio.h>
#include <math.h>
#include "knn_modules.c"
#include "pbPlots.h"
#include "supportLib.h"

#define nbProcessTrial 10
#define k 3
#define nbClass 3
void main(int argc, char **argv)
{ // retrieveData(filename);
    char *filename = "Iris_flower_dataset.csv";
    instance dataset[144];
    retrieveData(filename, 144, dataset);
    // checking
    int nbRecords = sizeof(dataset) / sizeof(instance);

    // testing
    instance testInstance;
    initInstance(&testInstance, 6.3, 2.7, 4.9, 1.8, 3);

    double execSeq = knn(testInstance, dataset, nbRecords, nbClass, k);

    double nbProc[nbProcessTrial];
    double speedUpThreads[nbProcessTrial];
    double speedUpMPI []= { 0.000353,0.000281,0.000331,0.000406,0.000482 , 0.000532,0.000569,0.000486,0.000505,0.000536};

    int i;
    for (i = 0; i < nbProcessTrial; i++)
    {
        nbProc[i] = i + 1;
    }

    
    //knnMPI(argc , argv , testInstance, dataset, nbRecords, nbClass, k);
    
    for (i = 0; i < nbProcessTrial; i++)
    {
        speedUpMPI[i]=execSeq/speedUpMPI[i];
    }
    for (i = 0; i < nbProcessTrial; i++)
    {
        speedUpThreads[i]=execSeq/knnThreads(i ,testInstance, dataset, nbRecords, nbClass, k );
    }

    RGBABitmapImageReference *imageRefMPI = CreateRGBABitmapImageReference();
    StringReference *errormessageMPI;

    DrawScatterPlot(imageRefMPI, 600, 400, nbProc, nbProcessTrial, speedUpMPI, nbProcessTrial, errormessageMPI);
    size_t lengthMPI;
    double *pngDataMPI = ConvertToPNG(&lengthMPI, imageRefMPI->image);
    WriteToFile(pngDataMPI, lengthMPI, "speedUpMPI.png");

    RGBABitmapImageReference *imageRefThreads = CreateRGBABitmapImageReference();
    StringReference *errormessageThreads;

    DrawScatterPlot(imageRefThreads, 600, 400, nbProc, nbProcessTrial, speedUpThreads, nbProcessTrial, errormessageThreads);
    size_t lengthThreads;
    double *pngDataThreads = ConvertToPNG(&lengthThreads, imageRefThreads->image);
    WriteToFile(pngDataThreads, lengthThreads, "speedUpThreads.png");
    
}
