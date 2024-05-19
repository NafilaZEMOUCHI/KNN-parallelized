#include <stdio.h>
#include <math.h>
#include "instance.c"

// structure used to build the kd tree
typedef struct kdnode
{
    instance instance;
    struct knode *bigger;
    struct knode *smaller;
}kdnode;

typedef struct kdtree{
    kdnode root;    
}kdtree;

