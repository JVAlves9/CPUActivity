#include <stdio.h>
#include <stdlib.h>
#include <cpuid.h>
#include <sys/sysinfo.h>
#include "cpuUsage.h"

int main (){
    while(1){
        calculate();
    }
    return 0;
}