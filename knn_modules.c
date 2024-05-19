#include "neighboring.c"
#include <time.h>
#include "mpi.h"
double knn(instance A, instance dataset[], int nbInstances, int nbClass, int k)
{
    clock_t execTime;
    execTime = clock();
    neighboringClass neighbors[1000];
    calculatingDistances(A, dataset, nbInstances, neighbors);
    printf("disntances:\n");
    printDistances(neighbors, nbInstances);
    sortNeighboringClasses(neighbors, nbInstances);
    printf("sorted disntances\n");
    printDistances(neighbors, nbInstances);
    classFrequency classFrequencyTable[3];
    frequencyOfNeighboringClass(neighbors, nbInstances, k, nbClass, classFrequencyTable);
    int class = modalClass(classFrequencyTable, nbClass);
    execTime = clock() - execTime;
    double duration = (double)execTime / CLOCKS_PER_SEC;
    printf("execuction time = %lf", duration);
    printf("class de l'instance = %d", class);
    return duration;
}

double knnThreads(int nbThreads, instance A, instance dataset[], int nbInstances, int nbClass, int k)
{
    clock_t execTime;
    execTime = clock();
    neighboringClass neighbors[1000];
    sem_t mutex;
    sem_init(&mutex, 0, 1);
    pthread_t threads[nbThreads];
    int i;
    int index = 0;
    for (i = 0; i < nbThreads; i++)
    {
        threadArg args;
        args.A = A;
        args.dataset = dataset;
        args.i = &index;
        args.mutex = &mutex;
        args.nbInstances = nbInstances;
        args.neighbors = neighbors;
        // printf("creating thread %d", i);
        threads[i] = pthread_create(&threads[i], NULL, calculatingDistancesThreads, (void *)&args);
    }
    for (i = 0; i < nbThreads; i++)
    {
        // printf("waitingfor a thread\n");
        pthread_join(threads[i], NULL);
        // printf("the thread finished\n");
    }
    sleep(1);
    // printf("disntances:\n");
    // printDistances(neighbors, nbInstances);
    sortNeighboringClasses(neighbors, nbInstances);
    // printf("sorted disntances\n");
    // printDistances(neighbors, nbInstances);
    classFrequency classFrequencyTable[3];
    frequencyOfNeighboringClass(neighbors, nbInstances, k, nbClass, classFrequencyTable);
    int class = modalClass(classFrequencyTable, nbClass);
    execTime = clock() - execTime;
    double duration = (double)execTime / CLOCKS_PER_SEC;
    printf("execuction time = %lf", duration);
    printf("class de l'instance = %d", class);
    return duration;
}

double knnMPI(int argc, char **argv, instance testInstance, instance dataset[], int nbInstances, int nbClass, int k)
{
    MPI_Init(&argc, &argv);

    // Get the number of processes and check only 2 processes are used
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Datatype MPI_Instance;
    int lengthsInstance[5] = {1, 1, 1, 1, 1};

    // Calculate displacements
    // In C, by default padding can be inserted between fields. MPI_Get_address will allow
    // to get the address of each struct field and calculate the corresponding displacement
    // relative to that struct base address. The displacements thus calculated will therefore
    // include padding if any.
    MPI_Aint displacementsI[5];
    instance Instance;
    MPI_Aint base_addressI;
    MPI_Get_address(&Instance, &base_addressI);
    MPI_Get_address(&Instance.sepalL, &displacementsI[0]);
    MPI_Get_address(&Instance.sepalW, &displacementsI[1]);
    MPI_Get_address(&Instance.petalL, &displacementsI[2]);
    MPI_Get_address(&Instance.petalW, &displacementsI[3]);
    MPI_Get_address(&Instance.irisType, &displacementsI[4]);
    displacementsI[0] = MPI_Aint_diff(displacementsI[0], base_addressI);
    displacementsI[1] = MPI_Aint_diff(displacementsI[1], base_addressI);
    displacementsI[2] = MPI_Aint_diff(displacementsI[2], base_addressI);
    displacementsI[3] = MPI_Aint_diff(displacementsI[3], base_addressI);
    displacementsI[4] = MPI_Aint_diff(displacementsI[4], base_addressI);

    MPI_Datatype typesInstance[5] = {MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_INT};
    MPI_Type_create_struct(5, lengthsInstance, displacementsI, typesInstance, &MPI_Instance);
    MPI_Type_commit(&MPI_Instance);

    MPI_Datatype MPI_NeighboringClass;
    int lengthsNC[2] = {1, 1};

    MPI_Aint displacementsNC[2];
    neighboringClass neighboring_Class;
    MPI_Aint base_addressNC;
    MPI_Get_address(&neighboring_Class, &base_addressNC);
    MPI_Get_address(&neighboring_Class.distance, &displacementsNC[0]);
    MPI_Get_address(&neighboring_Class.irisType, &displacementsNC[1]);
    displacementsNC[0] = MPI_Aint_diff(displacementsNC[0], base_addressNC);
    displacementsNC[1] = MPI_Aint_diff(displacementsNC[1], base_addressNC);

    MPI_Datatype typesNC[2] = {MPI_DOUBLE, MPI_INT};
    MPI_Type_create_struct(2, lengthsNC, displacementsNC, typesNC, &MPI_NeighboringClass);
    MPI_Type_commit(&MPI_NeighboringClass);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int length = nbInstances / size;
    int send_count[size];
    int dataset_displacements[size];
    int i;
    for (i = 0; i < size - 1; i++)
    {
        send_count[i] = length;
        dataset_displacements[i] = length * i;
    }
    if (nbInstances % size == 0)
    {
        send_count[i] = length;
        dataset_displacements[i] = length * i;
    }
    else
    {
        send_count[i] = length + nbInstances % size;
        dataset_displacements[i] = length * i;
    }
    switch (my_rank)
    {
    case 0:
    {

        // printf("scaterring the dataset\n");
        instance scatteredNeighbors[send_count[my_rank]];
        neighboringClass neighbors[nbInstances];
        double start = MPI_Wtime();
        MPI_Scatterv(dataset, send_count, dataset_displacements, MPI_Instance, scatteredNeighbors, send_count[my_rank], MPI_Instance, 0, MPI_COMM_WORLD);
        /*
        printf("\n\n number of instances in the scatterd dataset : %d from 0 \n\n", send_count[my_rank]);
        for (int i = 0; i < send_count[my_rank]; i++)
        {
            printf("%d :  %f %f %f %f %d\n", i + 1, scatteredNeighbors[i].sepalL, scatteredNeighbors[i].sepalW, scatteredNeighbors[i].petalL, scatteredNeighbors[i].petalW, scatteredNeighbors[i].irisType);
        }
        */
        neighboringClass gatheredNeighboringClass[send_count[my_rank]];
        calculatingDistances(testInstance, scatteredNeighbors, send_count[my_rank], gatheredNeighboringClass);
        sortNeighboringClasses(gatheredNeighboringClass, send_count[my_rank]);
        // printDistances(gatheredNeighboringClass, send_count[my_rank]);
        //  MPI_Gatherv(&my_value, 1, MPI_INT, buffer, counts, displacements, MPI_INT, root_rank, MPI_COMM_WORLD);
        MPI_Gatherv(gatheredNeighboringClass, send_count[my_rank], MPI_NeighboringClass, neighbors, send_count, dataset_displacements, MPI_NeighboringClass, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        // printDistances(neighbors , nbInstances);
        classFrequency classFrequencyTable[3];
        frequencyOfNeighboringClass(neighbors, nbInstances, k, nbClass, classFrequencyTable);
        int class = modalClass(classFrequencyTable, nbClass);
        double end = MPI_Wtime();
        double duration = end - start;
        printf("execuction time = %lf", duration);
        printf("class de l'instance = %d", class);
        break;
    }
    default:
    {
        // Receive the message
        instance scatteredNeighbors[send_count[my_rank]];
        MPI_Scatterv(NULL, NULL, NULL, MPI_Instance, scatteredNeighbors, send_count[my_rank], MPI_Instance, 0, MPI_COMM_WORLD);
        /*
        int i;
        printf("\n\n number of instances in the scatterd dataset : %d  from 0\n\n", send_count[my_rank]);
        for (int i = 0; i < send_count[my_rank]; i++)
        {
            printf("%d :  %f %f %f %f %d\n", i + 1, scatteredNeighbors[i].sepalL, scatteredNeighbors[i].sepalW, scatteredNeighbors[i].petalL, scatteredNeighbors[i].petalW, scatteredNeighbors[i].irisType);
        }
        */
        neighboringClass gatheredNeighboringClass[send_count[my_rank]];
        calculatingDistances(testInstance, scatteredNeighbors, send_count[my_rank], gatheredNeighboringClass);
        sortNeighboringClasses(gatheredNeighboringClass, send_count[my_rank]);
        // printDistances(gatheredNeighboringClass , send_count[my_rank]);
        MPI_Gatherv(gatheredNeighboringClass, send_count[my_rank], MPI_NeighboringClass, NULL, NULL, NULL, MPI_NeighboringClass, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        break;
    }
    }

    MPI_Finalize();
    int done_already;

    return 0;
}
double knnMPIFforPlot(int argc, char **argv, instance testInstance, instance dataset[], int nbInstances, int nbClass, int k, int nbProc, double duration[])
{
    MPI_Init(&argc, &argv);

    {
        // Get the number of processes and check only 2 processes are used
        int size;
        MPI_Comm_size(MPI_COMM_WORLD, &size);

        MPI_Datatype MPI_Instance;
        int lengthsInstance[5] = {1, 1, 1, 1, 1};

        // Calculate displacements
        // In C, by default padding can be inserted between fields. MPI_Get_address will allow
        // to get the address of each struct field and calculate the corresponding displacement
        // relative to that struct base address. The displacements thus calculated will therefore
        // include padding if any.
        MPI_Aint displacementsI[5];
        instance Instance;
        MPI_Aint base_addressI;
        MPI_Get_address(&Instance, &base_addressI);
        MPI_Get_address(&Instance.sepalL, &displacementsI[0]);
        MPI_Get_address(&Instance.sepalW, &displacementsI[1]);
        MPI_Get_address(&Instance.petalL, &displacementsI[2]);
        MPI_Get_address(&Instance.petalW, &displacementsI[3]);
        MPI_Get_address(&Instance.irisType, &displacementsI[4]);
        displacementsI[0] = MPI_Aint_diff(displacementsI[0], base_addressI);
        displacementsI[1] = MPI_Aint_diff(displacementsI[1], base_addressI);
        displacementsI[2] = MPI_Aint_diff(displacementsI[2], base_addressI);
        displacementsI[3] = MPI_Aint_diff(displacementsI[3], base_addressI);
        displacementsI[4] = MPI_Aint_diff(displacementsI[4], base_addressI);

        MPI_Datatype typesInstance[5] = {MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_INT};
        MPI_Type_create_struct(5, lengthsInstance, displacementsI, typesInstance, &MPI_Instance);
        MPI_Type_commit(&MPI_Instance);

        MPI_Datatype MPI_NeighboringClass;
        int lengthsNC[2] = {1, 1};

        MPI_Aint displacementsNC[2];
        neighboringClass neighboring_Class;
        MPI_Aint base_addressNC;
        MPI_Get_address(&neighboring_Class, &base_addressNC);
        MPI_Get_address(&neighboring_Class.distance, &displacementsNC[0]);
        MPI_Get_address(&neighboring_Class.irisType, &displacementsNC[1]);
        displacementsNC[0] = MPI_Aint_diff(displacementsNC[0], base_addressNC);
        displacementsNC[1] = MPI_Aint_diff(displacementsNC[1], base_addressNC);

        MPI_Datatype typesNC[2] = {MPI_DOUBLE, MPI_INT};
        MPI_Type_create_struct(2, lengthsNC, displacementsNC, typesNC, &MPI_NeighboringClass);
        MPI_Type_commit(&MPI_NeighboringClass);

        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
        int length = nbInstances / size;
        int send_count[size];
        int dataset_displacements[size];
        int i;
        for (i = 0; i < size - 1; i++)
        {
            send_count[i] = length;
            dataset_displacements[i] = length * i;
        }
        if (nbInstances % size == 0)
        {
            send_count[i] = length;
            dataset_displacements[i] = length * i;
        }
        else
        {
            send_count[i] = length + nbInstances % size;
            dataset_displacements[i] = length * i;
        }
        switch (my_rank)
        {
        case 0:
        {

            // printf("scaterring the dataset\n");
            instance scatteredNeighbors[send_count[my_rank]];
            neighboringClass neighbors[nbInstances];
            double start = MPI_Wtime();
            MPI_Scatterv(dataset, send_count, dataset_displacements, MPI_Instance, scatteredNeighbors, send_count[my_rank], MPI_Instance, 0, MPI_COMM_WORLD);
            /*
            printf("\n\n number of instances in the scatterd dataset : %d from 0 \n\n", send_count[my_rank]);
            for (int i = 0; i < send_count[my_rank]; i++)
            {
                printf("%d :  %f %f %f %f %d\n", i + 1, scatteredNeighbors[i].sepalL, scatteredNeighbors[i].sepalW, scatteredNeighbors[i].petalL, scatteredNeighbors[i].petalW, scatteredNeighbors[i].irisType);
            }
            */
            neighboringClass gatheredNeighboringClass[send_count[my_rank]];
            calculatingDistances(testInstance, scatteredNeighbors, send_count[my_rank], gatheredNeighboringClass);
            sortNeighboringClasses(gatheredNeighboringClass, send_count[my_rank]);
            // printDistances(gatheredNeighboringClass, send_count[my_rank]);
            //  MPI_Gatherv(&my_value, 1, MPI_INT, buffer, counts, displacements, MPI_INT, root_rank, MPI_COMM_WORLD);
            MPI_Gatherv(gatheredNeighboringClass, send_count[my_rank], MPI_NeighboringClass, neighbors, send_count, dataset_displacements, MPI_NeighboringClass, 0, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
            // printDistances(neighbors , nbInstances);
            classFrequency classFrequencyTable[3];
            frequencyOfNeighboringClass(neighbors, nbInstances, k, nbClass, classFrequencyTable);
            int class = modalClass(classFrequencyTable, nbClass);
            double end = MPI_Wtime();
            duration[i] = end - start;
            printf("execuction time = %lf", *duration);
            printf("class de l'instance = %d", class);
            break;
        }
        default:
        {
            // Receive the message
            instance scatteredNeighbors[send_count[my_rank]];
            MPI_Scatterv(NULL, NULL, NULL, MPI_Instance, scatteredNeighbors, send_count[my_rank], MPI_Instance, 0, MPI_COMM_WORLD);
            /*
            int i;
            printf("\n\n number of instances in the scatterd dataset : %d  from 0\n\n", send_count[my_rank]);
            for (int i = 0; i < send_count[my_rank]; i++)
            {
                printf("%d :  %f %f %f %f %d\n", i + 1, scatteredNeighbors[i].sepalL, scatteredNeighbors[i].sepalW, scatteredNeighbors[i].petalL, scatteredNeighbors[i].petalW, scatteredNeighbors[i].irisType);
            }
            */
            neighboringClass gatheredNeighboringClass[send_count[my_rank]];
            calculatingDistances(testInstance, scatteredNeighbors, send_count[my_rank], gatheredNeighboringClass);
            sortNeighboringClasses(gatheredNeighboringClass, send_count[my_rank]);
            // printDistances(gatheredNeighboringClass , send_count[my_rank]);
            MPI_Gatherv(gatheredNeighboringClass, send_count[my_rank], MPI_NeighboringClass, NULL, NULL, NULL, MPI_NeighboringClass, 0, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
            break;
        }
        }
    }
    MPI_Finalize();

    return 0;
}