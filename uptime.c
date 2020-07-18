#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void readUptime(float *used, float *idle){
    FILE *uptime = fopen("/proc/uptime","r");   //get the file containing the active and idle times of the cpu

    int i;
    char temp[15];

    char c=fgetc(uptime);   //get fisrt character of the file

    for(i = 0 ;c != ' '; i++) { //iterate until last digit of first number
        c = fgetc(uptime);
        temp[i]=c;
    }
    *used = atof(temp);  //convert string into float
    for(i = 0 ; i < 15 ; i++ ){ //same iteration, but for second number
        if (c != EOF){  //just get chars until end of file not reached
            c = getc(uptime);
            temp[i] = c;
        }else{  //complete the rest with zeros in case the first number was longer than the second
            temp[i]=0;
        }
    }
    *idle = atof(temp);
    printf("first: %f\nsecond: %f\n",*used,*idle);
}

void calculate(){
    float used1 =0, used2=0, idle1=0, idle2=0;
    float calc=0;

    readUptime(&used1,&idle1);  //first values 
    sleep(1);
    readUptime(&used2,&idle2);  //second values

    calc = ((used2 - used1) / (idle2 - idle1)) * 100;   //perc of usage
    printf("per used : %.2f %%",calc);
}