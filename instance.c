struct instance
{
    int sepalL;
    int sepalW;
    int petalL;
    int petalW;
    char irisType;
};
typedef struct instance instance;
double euclideanDistance(instance sourceInstance, instance neighboringInstance)
{
    double distance;
    distance = (sourceInstance.sepalL - neighboringInstance.sepalL) ^ 2;
    distance += (sourceInstance.sepalW - neighboringInstance.sepalW) ^ 2;
    distance += (sourceInstance.petalL - neighboringInstance.petalL) ^ 2;
    distance += (sourceInstance.petalW - neighboringInstance.petalW) ^ 2;
    distance = sqrt(distance);
    return distance;
};