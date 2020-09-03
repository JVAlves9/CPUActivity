
void calculate(float calc[]);

typedef struct process{
    char  * pid;
    char * comm;
    long ticks1;
    long ticks2;
    long startime;
    float perc;
} Process; 

long * procStat(long idle[], int cpus);

void readUptime(float *used);