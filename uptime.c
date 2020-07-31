#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <regex.h>
#include <dirent.h>
#include <time.h>
typedef struct 
{
    char ** directories;    //array of strins containing the processess' PIDs 
    size_t size;    //array size
    size_t bytes;   //number of bytes used
}dirs;

typedef struct
{
    char  * pid;
    char * comm;
    long ticks1;
    long ticks2;
    long startime;
}process;


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
            if( *u != 0){
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
        exit(EXIT_FAILURE);
    }else if(r ==  NULL){
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

dirs PIDsInProc(){
    dirs dirs;  //struct with an array of PIDs
    int i = 0;
    DIR * dir = opendir("/proc");   //opening the process folder 
    struct dirent *dir_access; 
    regex_t regex;

    if(dir == NULL){
        printf("Couldn't open directory");
        exit(-1);
    }

    if( regcomp(&regex,"[0-9]+",REG_EXTENDED) != 0){    //regular expression will make sure that tthe name of the folder is a number, thus it is a PID
        printf("regcomp problem");
        exit(-1);
    }
    
    dirs.size = 0;
    dirs.bytes = 0;
    dirs.directories = malloc(1);

    while( (dir_access = readdir(dir)) != NULL ){//go though the files and folders in this directory   // DT_DIR confirms that it is in fact a folder
        if( dir_access->d_type == DT_DIR && regexec(&regex,dir_access->d_name,0,NULL,REG_EXTENDED)==0 ){//regexec checks if it matches with the regex var
            dirs.bytes+= (strlen(dir_access->d_name)+1)*4;      //bytes needed - pointers and chars
            dirs.directories = realloc(dirs.directories,dirs.bytes);    //increase the allocatedd memory for the upcoming string
            dirs.directories[dirs.size] = dir_access->d_name;
            dirs.size++;
        }
    }

    closedir(dir);
    return dirs;
}

char * command(char pid[]){     //get the name of the process command
    int i = 0;
    char * cmd;
    char pro[20] = "/proc/";
    char c, temp[20];
    strcat(pro,pid);
    strcat(pro,"/comm");
    FILE * comm = fopen(pro,"r");

    if( comm == NULL){
        exit(EXIT_FAILURE);
    }

    c = getc(comm);

    for (int i = 0; c!=EOF; i++)
    {
        temp[i] = c;
        c = getc(comm);
    }
    cmd = malloc(strlen(temp)+1);
    strcpy(cmd,temp);
    
    return cmd;
}

void calculate(){
    float up1 =0, calcp = 0.0;
    int cpus = get_nprocs(), i;
    long  idles1[cpus+1], idles2[cpus+1], *used1, *used2;
    float calc=0,calc0=0, total, used/*,calc1=0,calc2=0, calc3=0*/;
    char pid[6]="";
    long hz = sysconf(_SC_CLK_TCK);

    dirs dirs = PIDsInProc();
    process pro[dirs.size];
    
    for( i = 0; i < dirs.size; i++ ){
        pro[i].pid = dirs.directories[i];
        pro[i].ticks1 = 0;
        pro[i].ticks2 = 0;
        pro[i].startime = 0;
        pro[i].comm = command(pro[i].pid);
    }

    readUptime(&up1);   
    for(i=0; i < dirs.size; i++){
        stat(&pro[i].ticks1,&pro[i].startime,pro[i].pid);   //get all processes ticks
    }
    used1 = procStat(idles1,cpus);//first values
    sleep(1);
    for(i=0; i < dirs.size; i++){
        stat(&pro[i].ticks2,&pro[i].startime,pro[i].pid);
    }
    used2 = procStat(idles2,cpus);//second values
    
    for(i =0 ; i < dirs.size; i++){
        calcp =(float) ( pro[i].ticks2 - pro[i].ticks1 ) / hz ; //usage of the processes | time passed is 1s, so no need to divide

        printf("per used : %.2f %%\n",100 * calcp);
    }

    for(i = 0; i < cpus+1; i++ ){   //per usage of each cpu, the first is from all cpus
        total = (idles2[i] - idles1[i] + used2[i] - used1[i]);
        used = (used2[i] - used1[i]);
        calc0 = (used / total) * 100;
        printf("cpu%d:  %.2f%%\n",i,calc0);
    }
}