#include <stdio.h>
#include <math.h>
#include "instance.c"

// structure representing the class of the instance that has the distance 'distance'
struct neighboringClass
{
    double distance;
    char irisType;
};
typedef struct neighboringClass neighboringClass;

// structure to use to build a chained list of classes and their distance from the the instance to use after to calculate the mode
struct neighboringClassNode
{
    neighboringClass currentNeighbor;
    neighboringClassNode *furtherNeighborNode;
    neighboringClassNode *nearerNeighborNode;
};
typedef struct neighboringClassNode neighboringClassNode;

//chained list to keep track of k nearest neighbors
struct kNeighboringClasses
{
    int k;
    neighboringClassNode *furthestNeighboringClassNode;
    neighboringClassNode *nearesrNeighboringClassNode;
};

// structure used to build the kd tree
struct kdnode
{
    instance instance;
    struct knode *bigger;
    struct knode *smaller;
};
typedef struct knode knode;
