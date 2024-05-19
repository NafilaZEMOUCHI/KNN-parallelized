#include "neighboring.c"

int knn(instance A , instance dataset[] , int nbInstances , int nbClass , int k){
    neighboringClass neighbors[1000];
    calculatingDistances(A , dataset , nbInstances ,neighbors);
    printf("disntances:\n");
    printDistances(neighbors , nbInstances);
    sortNeighboringClasses(neighbors , nbInstances);
    printf("sorted disntances\n");
    printDistances(neighbors , nbInstances);
    classFrequency classFrequencyTable[3];
    frequencyOfNeighboringClass(neighbors,nbInstances,k,nbClass,classFrequencyTable);
    int class = modalClass(classFrequencyTable,nbClass);
    return class;
}
