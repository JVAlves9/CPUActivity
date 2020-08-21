#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include "listOfProcess.h"
#include "processes.h"

void readUptime(float *used){
    FILE *uptime = fopen("/proc/uptime","r");   //get the file containing the active and idle times of the cpu
    
    if( uptime == NULL){
        printf("couldn't open file in uptime");
        exit(EXIT_FAILURE);
    }

    int i;
    char temp[15];

    char c=fgetc(uptime);   //get fisrt character of the file

    for(i = 0 ;c != ' '; i++) { //iterate until last digit of first number
        temp[i]=c;
        c = fgetc(uptime);
    }
    *used = atof(temp);  //convert string into float
    
    fclose(uptime);
}

long * procStat(long idle[], int cpus){
    long * r = malloc( (cpus+1) * sizeof (long)); // allocate the memory for the returning array containing the cpus times
    FILE * stat = fopen("/proc/stat","r");
    long lgs[10];   //cpu times temporary variable
    int i, j, k ;
    char temp[15];      //temp variable to parse char to long
    char c =  fgetc(stat);

    if( stat == NULL){
        printf("stat file was not open");
        exit(EXIT_FAILURE);
    }else if(r ==  NULL){
        printf("allocation couldn't occur in proc stat");
        exit(EXIT_FAILURE);
    }

    for (k = 0 ; k<cpus+1; k++){    // get through the lines
        for( i = 0; i< 11;i++){   //get through the columns
            for(j = 0; c != ' ' && c!='\n'; j ++){     //get through the digits
                if(i != 0 ){       //exclude 'cpu' text
                    temp[j] = c;
                    
                }
                c= getc(stat);
            }
            if( c == '\n'){ //break in new line
                lgs[i - 1] = atol(temp);
                break;
            }
            else if( i != 0 ){
                lgs[i - 1] = atol(temp);
                for ( j = 0; j < (sizeof temp)/sizeof (char) ; j++ ){ temp[j] = '\0'; }
            }
            c=getc(stat);
            if( c == ' '){ i--;}    //there's an extra space after the first cpu
        }
        r[k]= 0;    //initialize
        idle[k] = lgs[3];   //save the idle numbers
        for( j = 0 ; j < (sizeof lgs)/sizeof (long); j++){
            if( j != 3){
                r[k] += lgs[j];     //sum all times, except idle
            }
        }
        c=getc(stat);
    }
    fclose(stat);
    return r;
}

void calculate(){
    float up1 =0, calcp = 0.0;
    int cpus = get_nprocs(), i;
    long  idles1[cpus+1], idles2[cpus+1], *used1, *used2;
    float calc=0,calc0=0, total, used/*,calc1=0,calc2=0, calc3=0*/;
    Node ** h, * temp;

    PIDsInProc();   //fill the list with valid processes
    h = getHead();  //get the first item of the list
    initializeProcValues();   //initialize variables of the process struct in the list
    getCommands();    

    readUptime(&up1);
    gettingStats(1);
    used1 = procStat(idles1,cpus);//first values

    sleep(1);

    gettingStats(2);
    used2 = procStat(idles2,cpus);//second values

    calcPercsNStore();
    i=0;

    for(i = 0; i < cpus+1; i++ ){   //per usage of each cpu, the first is from all cpus
        total = (idles2[i] - idles1[i] + used2[i] - used1[i]);
        used = (used2[i] - used1[i]);
        calc0 = (used / total) * 100;
        printf("cpu%d:  %.2f%%\n",i,calc0);
    }
    freeAll();
}