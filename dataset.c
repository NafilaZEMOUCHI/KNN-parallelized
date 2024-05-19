#include "instance.c"
#include <stdio.h>
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

