#include <stdio.h>
#include <stdlib.h>
#include <cpuid.h>
#include <sys/sysinfo.h>
#include "uptime.h"

int main (){
    const double megabyte = 1024*1024;

    struct sysinfo si;

    calculate();
    /*system("free");
    double totalram = si.totalram/megabyte;
    double free_ram = (si.bufferram/megabyte+si.freeram/megabyte+si.sharedram/megabyte);
    double perc_free_ram = (free_ram/totalram)*100;

    printf("free: %5.1f\ntotal: %5.1f\n%f%%",free_ram,totalram,perc_free_ram);
    printf("\n");
    int a,b,c,d;
    __cpuid(0,a,b,c,d);
    printf("EAX: %x\nEBX: %x\nECX: %x\nEDX: %x\n",a,b,c,d);*/

    return 0;
}