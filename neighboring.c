#include <stdio.h>
#include "instance.c"
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
// structure representing the class of the instance that has the distance 'distance'
typedef struct
{
    double distance;
    int irisType;
} neighboringClass;

void printNeighboringClass(neighboringClass neighboringClass)
{
    printf("(%d , %f)\n", neighboringClass.irisType, neighboringClass.distance);
}
// structure to use to build a chained list of classes and their distance from the the instance to use after to calculate the mode

typedef struct neighboringClassNode
{
    neighboringClass currentNeighboringClass;
    struct neighboringClassNode *furtherNeighboringClass;
    struct neighboringClassNode *nearerNeighboringClass;
} neighboringClassNode;

// chained list to keep track of k nearest neighbors
typedef struct kNeighboringClasses
{
    int k;
    int count;
    neighboringClassNode *furthestNeighboringClass;
    neighboringClassNode *nearestNeighboringClass;
} kNeighboringClasses;

void insertNeighboringClass(neighboringClassNode *newNeighboringClass, kNeighboringClasses *kNeighboringClasses)
{
    (*newNeighboringClass).furtherNeighboringClass = NULL;
    (*newNeighboringClass).nearerNeighboringClass = NULL;
    printf("checking if there are any elements in the list\n");
    if ((*kNeighboringClasses).nearestNeighboringClass == NULL && (*kNeighboringClasses).furthestNeighboringClass == NULL)
    {
        printf("no the list is empty\n");
        (*kNeighboringClasses).furthestNeighboringClass = newNeighboringClass;
        (*kNeighboringClasses).nearestNeighboringClass = newNeighboringClass;
    }
    else
    {
        printf("yes there are\n");
        neighboringClassNode *currentNeighboringClass;
        currentNeighboringClass = (*kNeighboringClasses).nearestNeighboringClass;
        while (currentNeighboringClass != NULL)
        {
            printf("checking ...\n");
            if ((*newNeighboringClass).currentNeighboringClass.distance <= (*currentNeighboringClass).currentNeighboringClass.distance)
            {
                printf("it's less \n");
                if ((*currentNeighboringClass).nearerNeighboringClass = NULL)
                {
                    printf("there is ony one element in the list\n");
                    (*kNeighboringClasses).nearestNeighboringClass = newNeighboringClass;
                }
                printf("adding the new node\n");
                (*currentNeighboringClass).nearerNeighboringClass = newNeighboringClass;
                (*newNeighboringClass).furtherNeighboringClass = currentNeighboringClass;
                break;
            }
            else
            {
                printf("its bigger\n");
                if ((*currentNeighboringClass).furtherNeighboringClass == NULL)
                {
                    printf("reached the end of the list\n");
                    (*currentNeighboringClass).furtherNeighboringClass = newNeighboringClass;
                    (*newNeighboringClass).nearerNeighboringClass = currentNeighboringClass;
                }
                else
                {
                    printf("moving to the next node\n");
                    currentNeighboringClass = (*currentNeighboringClass).furtherNeighboringClass;
                }
            }
        }
    }
    kNeighboringClasses->count++;
};

void printKNeighboringClass(kNeighboringClasses kNeighboringClasses)

{

    neighboringClassNode *currentNeighboringClass;
    currentNeighboringClass = kNeighboringClasses.nearestNeighboringClass;
    neighboringClass current;
    while (currentNeighboringClass != NULL)
    {
        current = (*currentNeighboringClass).currentNeighboringClass;
        printNeighboringClass(current);
        currentNeighboringClass = (*currentNeighboringClass).furtherNeighboringClass;
        if (currentNeighboringClass == NULL)
            printf("true\n");
    }
};

void calculatingDistances(instance A, instance dataset[], int nbInstances, neighboringClass neighbors[])
{
    int i;
    neighboringClass neighboringClass;
    for (i = 0; i < nbInstances; i++)
    {
        neighboringClass.irisType = dataset[i].irisType;
        neighboringClass.distance = euclideanDistance(A, dataset[i]);
        neighbors[i] = neighboringClass;
    }
}
typedef struct threadArg
{
    instance A;
    instance *dataset;
    int nbInstances;
    neighboringClass *neighbors;
    sem_t *mutex;
    int *i;
} threadArg;

void *calculatingDistancesThreads(void *args)
{
    threadArg *Args = (threadArg *)args;
    instance A = Args->A;
    instance *dataset = Args->dataset;
    int nbInstances = Args->nbInstances;
    neighboringClass *neighbors = Args->neighbors;
    sem_t *mutex = Args->mutex;
    int *i = Args->i;

    while (1)
    {
        sem_wait(mutex);
        //printf("nbinstances = %d\n",nbInstances);
        if ((*i) < nbInstances)
        {
            //printf("i = %d \n", *i);
            
            neighboringClass neighboringClass;
            neighboringClass.irisType = dataset[*i].irisType;
            neighboringClass.distance = euclideanDistance(A, dataset[*i]);
            neighbors[*i] = neighboringClass;
            (*i)++;
            
            sem_post(mutex);
        }
        else
        {
            sem_post(mutex);            
            break;
        }
    }
    //printf("out of loop\n");
    pthread_exit(0);
}

void printDistances(neighboringClass neighbors[], int nbInstances)
{
    int i;
    for (i = 0; i < nbInstances; i++)
    {
        printf("(%d ; %lf) \n", neighbors[i].irisType, neighbors[i].distance);
    }
}
void swap(neighboringClass *xp, neighboringClass *yp)
{
    neighboringClass temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void sortNeighboringClasses(neighboringClass neighbors[], int n)
{
    int i, j, min_idx;

    // One by one move boundary of unsorted subneighboay
    for (i = 0; i < n - 1; i++)
    {
        // Find the minimum element in unsorted neighboray
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (neighbors[j].distance < neighbors[min_idx].distance)
                min_idx = j;

        // Swap the found minimum element with the first element
        if (min_idx != i)
            swap(&neighbors[min_idx], &neighbors[i]);
    }
}

void mergeNeighboringClasses(neighboringClass neighborsA[], int nA, neighboringClass neighborsB[], int nB, neighboringClass neighborsC[], int nC)
{
    int i = 0;
    int j = 0;
    int k = 0;
    while (k < nA + nB)
    {
        if (neighborsA[i].distance <= neighborsB[j].distance)
        {
            neighborsC[k] = neighborsA[i];
            k++;
            i++;
        }
        else
        {
            neighborsC[k] = neighborsB[j];
            k++;
            j++;
        }
    }
}

typedef struct
{
    int irisType;
    int frequency;
} classFrequency;

void frequencyOfNeighboringClass(neighboringClass sortedNeighbors[], int nbNeighbors, int k, int nbClass, classFrequency classFrequencyTable[])
{
    int i, j;
    for (i = 0; i < nbClass; i++)
    {
        classFrequencyTable[i].irisType = i + 1;
        for (j = 0; j < k; j++)
        {
            if (sortedNeighbors[j].irisType == i)
            {
                classFrequencyTable[i].frequency++;
            }
        }
    }
}

int modalClass(classFrequency classFrequencyTable[], int nbClass)
{
    int i = 0;
    int maxFrequency = classFrequencyTable[i].frequency;
    int modalClass = classFrequencyTable[i].irisType;
    for (i = 0; i < nbClass; i++)
    {
        if (maxFrequency < classFrequencyTable[i].frequency)
        {
            maxFrequency = classFrequencyTable[i].frequency;
            modalClass = classFrequencyTable[i].irisType;
        }
    }
    return modalClass;
}
