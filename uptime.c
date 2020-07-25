#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>

void readUptime(float *used){
    FILE *uptime = fopen("/proc/uptime","r");   //get the file containing the active and idle times of the cpu
    
    if( uptime == NULL){
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

void stat(long *t,long *u,char pid[]){
    
    char pro[20] = "/proc/"; //proc directory
    
    strcat(pro,pid);    //concatenating proc with the id of the process
    strcat(pro,"/stat");    //stat pseudo -file
    
    FILE *sta = fopen(pro,"r");     //accessing the file

    if( sta == NULL){
        exit(EXIT_FAILURE);
    }

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
            for ( j = 0; j < (sizeof temp)/sizeof (char) ; j++ ){ temp[j] = '\0'; }  //cleaning the temp variable

        }else if (i ==21){
            lgs[2]= atol(temp);
            break;
        }
        c = fgetc(sta);
    }
    *t = lgs[0]+lgs[1]; // total time this process used the cpu
    *u = lgs[2];
    fclose(sta);
}

long * procStat(long idle[], int cpus){
    long * r = malloc( (cpus+1) * sizeof (long)); // allocate the memory for the returning array containing the cpus times
    FILE * stat = fopen("/proc/stat","r");
    long lgs[10];   //cpu times temporary variable
    int i, j, k ;
    char temp[15];      //temp variable to parse char to long
    char c =  fgetc(stat);

    if( stat == NULL){
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
    float up1 =0, up2=0;
    int cpus = get_nprocs(),i;
    long t1, u1, t2, u2, idles1[cpus+1], idles2[cpus+1], *used1, *used2;
    float calc=0,calc0=0, total, used/*,calc1=0,calc2=0, calc3=0*/;
    double calcp = 0.0;
    char pid[6]="";
    long hz = sysconf(_SC_CLK_TCK);
    
    scanf("%s",&pid);

    readUptime(&up1);  //first values 
    stat(&t1,&u1,pid);
    used1 = procStat(idles1,cpus);
    sleep(1);
    //stat(&t2,&u2,pid);
    used2 = procStat(idles2,cpus);
    readUptime(&up2);  //second values

    double seconds = up1 - (u1/hz);
    used = (used2[0] - used1[0]);

    calcp = ((( t1 ) / hz )/seconds /*/ used*/ ) * 100; //usage of the process // working on this

    printf("per used : %.2f %%\nper2: %.2f\n",calc,calcp);

    for(i = 0; i < cpus+1; i++ ){   //per usage of each cpu, the first is from all cpus
        total = (idles2[i] - idles1[i] + used2[i] - used1[i]);
        used = (used2[i] - used1[i]);
        calc0 = (used / total) * 100;
        printf("cpu%d:  %.2f%%\n",i,calc0);
    }
}