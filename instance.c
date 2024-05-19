#include <math.h>
#include <stdio.h>
typedef struct instance
{
    float sepalL;
    float sepalW;
    float petalL;
    float petalW;
    int irisType;
} instance;

void initInstance(instance* instance , float sepalL , float sepalW , float petalL , float petalW , int irisType){
    (*instance).sepalL = sepalL;
    (*instance).sepalW = sepalL;
    (*instance).petalL = petalL;
    (*instance).petalW= petalW;
    (*instance).irisType = irisType;

}
double euclideanDistance(instance sourceInstance, instance neighboringInstance)
{
    double distance;
    distance = (sourceInstance.sepalL - neighboringInstance.sepalL)*(sourceInstance.sepalL - neighboringInstance.sepalL);
    distance += (sourceInstance.sepalW - neighboringInstance.sepalW)*(sourceInstance.sepalW - neighboringInstance.sepalW);
    distance += (sourceInstance.petalL - neighboringInstance.petalL)*(sourceInstance.petalL - neighboringInstance.petalL);
    distance += (sourceInstance.petalW - neighboringInstance.petalW)*(sourceInstance.petalW - neighboringInstance.petalW);
    
    return distance;
};

int retrieveData(char fileName[], int nbInstances , instance dataset[] )
{
    printf("%s", fileName);
    FILE *file;
    //instance dataset[144];

    file = fopen(fileName, "r");
    if (file == NULL)
    {
        printf("error opening file");
        return 1;
    };

    int nbReadRecords = 0;
    int nbAttributs;
    do
    {
        nbAttributs = fscanf(file,
                       "%f,%f,%f,%f,%d\n",
                       &dataset[nbReadRecords].sepalL,
                       &dataset[nbReadRecords].sepalW,
                       &dataset[nbReadRecords].petalL,
                       &dataset[nbReadRecords].petalW,
                       &dataset[nbReadRecords].irisType);
        if (nbAttributs == 5)
        {
            nbReadRecords++;
        }
        else
        {
            if (!feof(file))
            {
                printf("Error : incorrect formating \n");
                return 1;
            }
        }
        if (ferror(file))
            printf("error reading file \n");

    } while (!feof(file));

    fclose(file);
    
    return 0;
};
