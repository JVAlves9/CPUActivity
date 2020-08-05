
void calculate();

typedef struct process{
    char  * pid;
    char * comm;
    long ticks1;
    long ticks2;
    long startime;
} Process; 