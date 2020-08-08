#include "cpuUsage.h"

typedef struct node
{
    Process value;
    struct node * next;
}Node;

void addPro(char * pid);

void remPro(char * pid);

Process * findPro(char * pid);

Node * getHead();

void freeAll();

int size();

void initiateProcValues();