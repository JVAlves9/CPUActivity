#include <stdio.h>
#include <stdlib.h>

int readUptime(){
    FILE *uptime = fopen("/proc/uptime","r");
    char c=fgetc(uptime);

    while(c != EOF){
        printf("%c",c);
        c = fgetc(uptime);
    }
    return 0;
}