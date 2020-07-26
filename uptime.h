
void readUptime(float *used); //get the system uptime in /proc/uptime

void calculate();

void stat(long *t,long *u,char pid[]); //get process ticks in /proc/pid/stat

long * procStat(long idle[], int cpus);//return the sum of CPUs times whitout idle times, putting these on idle[]