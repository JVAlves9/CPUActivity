#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void readUptime(float *used, float *idle){
    FILE *uptime = fopen("/proc/uptime","r");   //get the file containing the active and idle times of the cpu
    
    int i;
    char temp[15];

    char c=fgetc(uptime);   //get fisrt character of the file

    for(i = 0 ;c != ' '; i++) { //iterate until last digit of first number
        temp[i]=c;
        c = fgetc(uptime);
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
void stat(long *t,long *u,char pid[]){
    
    char pro[20] = "/proc/"; //proc directory
    
    strcat(pro,pid);    //concatenating proc with the id of the process
    strcat(pro,"/stat");    //stat pseudo -file
    
    FILE *sta = fopen(pro,"r");     //accessing the file

    int i,j;
    char temp [15];     //temp used to store the number in string, then converting to long
    char c = fgetc(sta);    //getting the file content
    long lgs[3];    //numbers storage

    for(i = 0;c != EOF ;i++){   //getting through the number
        for(j = 0 ; c != ' '; j++){     //getting through the digits
            if(i == 13 ||  i == 14 || i == 21){     //13(from 0) number - user time | 14(from 0) number - system time | 21 (from 0)  number - start time
                temp[j]=c;
            }
            c = fgetc(sta);
        }
        if( i == 13 || i ==14){
            lgs[i-13] = atol(temp);     //parsing char to long
            for ( j = 0; j < sizeof temp; j++ ){ temp[j] = '\0'; }  //cleaning the temp variable

        }else if (i ==21){
            lgs[2]= atol(temp);
            break;
        }
        c = fgetc(sta);
    }
    *t = lgs[0]+lgs[1]; // total time this process used the cpu
    *u = lgs[2];
    printf("t: %ld, u: %ld\n",*t,*u);
}

void calculate(){
    float used1 =0, used2=0, idle1=0, idle2=0;
    long t1,u1,t2,u2;
    float calc=0;
    double calc2 = 0.0;
    char pid[6]="";
    long hz = sysconf(_SC_CLK_TCK);
    
    scanf("%s",&pid);

    readUptime(&used1,&idle1);  //first values 
    stat(&t1,&u1,pid);
    sleep(1);
    //stat(&t2,&u2);
    readUptime(&used2,&idle2);  //second values
    printf("\nused1: %f,");
    double seconds = used1 - (u1 / hz);
    calc2 = 100*((t1 / hz) / seconds);

    calc = ((used2 - used1) / (idle2 - idle1 + used2 - used1)) * 100;   //perc of usage // wrong, will change
    printf("per used : %.2f %%\nper2: %.2f\n",calc,calc2);
}