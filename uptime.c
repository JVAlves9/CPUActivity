#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <regex.h>
#include <dirent.h>
#include <time.h>
#include "listOfProcess.h"

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

void stat(long *t,long *u,char pid[]){
    
    char pro[20] = "/proc/"; //proc directory
    
    strcat(pro,pid);    //concatenating proc with the id of the process
    strcat(pro,"/stat");    //stat pseudo -file
    
    FILE *sta = fopen(pro,"r");     //accessing the file

    if( sta == NULL){
        printf("file couldn't open in stat");
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
            if( *u != 0 && i==4){
                lgs[2] = *u;
                break; 
            }

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

void PIDsInProc(){
    char * temp;
    DIR * dir = opendir("/proc");   //opening the process folder 
    struct dirent *dir_access; 
    regex_t regex;

    if(dir == NULL){
        printf("Couldn't open directory");
        exit(EXIT_FAILURE);
    }

    if( regcomp(&regex,"[0-9]+",REG_EXTENDED) != 0){    //regular expression will make sure that tthe name of the folder is a number, thus it is a PID
        printf("regcomp problem");
        exit(EXIT_FAILURE);
    }

    while( (dir_access = readdir(dir)) != NULL ){//go though the files and folders in this directory   // DT_DIR confirms that it is in fact a folder
        if( dir_access->d_type == DT_DIR && regexec(&regex,dir_access->d_name,0,NULL,REG_EXTENDED)==0 ){//regexec checks if it matches with the regex var
            
            temp = (char *) malloc( strlen(dir_access->d_name) * sizeof (char *) );
            strcpy(temp,dir_access->d_name);
            addPro(temp);

        }
    }

    closedir(dir);
}//it may appear thaat DT_DIR is an error, but it compiles fine

char * command(char pid[]){     //get the name of the process command
    int i = 0;
    char * cmd = (char *) malloc( sizeof (char *) );
    char pro[20] = "/proc/";
    char c;
    strcat(pro,pid);
    strcat(pro,"/comm");
    FILE * comm = fopen(pro,"r");

    if( comm == NULL){
        printf("empty command");
        exit(EXIT_FAILURE);
    }

    c = getc(comm);

    for (int i = 0; c!=EOF; i++)
    {
        cmd = (char *) realloc( cmd, ( i+1 ) * sizeof ( char *));
        cmd[i] = c;
        c = getc(comm);
    }
    fclose(comm);

    return cmd;
}

void calculate(){
    float up1 =0, calcp = 0.0;
    int cpus = get_nprocs(), i;
    long  idles1[cpus+1], idles2[cpus+1], *used1, *used2;
    float calc=0,calc0=0, total, used/*,calc1=0,calc2=0, calc3=0*/;
    char pid[6]="";
    long hz = sysconf(_SC_CLK_TCK);
    Node * h, * temp;

    PIDsInProc();
    h = getHead();
    initiateProcValues();

    temp = h;   
    while(temp!=NULL){
        temp->value.comm = command(temp->value.pid);
        temp = temp->next;
    }
    
    readUptime(&up1);
    temp = h;   
    while(temp!=NULL){
        stat(&temp->value.ticks1,&temp->value.startime,temp->value.pid);   //get all processes ticks
        temp = temp->next;
    }
    used1 = procStat(idles1,cpus);//first values

    sleep(1);

    temp = h;
    while(temp!=NULL){
        stat(&temp->value.ticks2,&temp->value.startime,temp->value.pid);
        temp = temp->next;
    }
    used2 = procStat(idles2,cpus);//second values

    temp = h;
    while(temp!=NULL){
        calcp =(float) ( temp->value.ticks2 - temp->value.ticks1 ) / hz ; //usage of the processes | time passed is 1s, so no need to divide

        printf("%s -- per used : %.2f %%\n",temp->value.comm,100 * calcp);
        temp = temp->next;
    }

    for(i = 0; i < cpus+1; i++ ){   //per usage of each cpu, the first is from all cpus
        total = (idles2[i] - idles1[i] + used2[i] - used1[i]);
        used = (used2[i] - used1[i]);
        calc0 = (used / total) * 100;
        printf("cpu%d:  %.2f%%\n",i,calc0);
    }
}