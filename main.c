#include <stdio.h>
#include <stdlib.h>
#include <cpuid.h>


int main (){
    printf("\n");
    int a,b,c,d;
    __cpuid(0,a,b,c,d);
    printf("EAX: %x\nEBX: %x\nECX: %x\nEDX: %x\n",a,b,c,d);

    return 0;
}