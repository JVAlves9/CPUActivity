#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include "listOfProcess.h"


void stat(long *ticks,long *up,char pid[]){
    
    char pro[20] = "/proc/"; //proc directory
    
    strcat(pro,pid);    //concatenating proc with the id of the process
    strcat(pro,"/stat");    //stat pseudo -file
    
    FILE *sta = fopen(pro,"r");     //accessing the file

    if( sta == NULL){
        printf("file couldn't open in stat");
        remPro(pid);    //remove the pid that's problably not attached to a running process
        return;
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
            if( *up != 0 && i==14){
                lgs[2] = *up;
                break; 
            }

        }else if (i ==21){
            lgs[2]= atol(temp);
            for ( j = 0; j < (sizeof temp)/sizeof (char) ; j++ ){ temp[j] = '\0'; } //the buffer was getting in the next process
            break;
        }
        c = fgetc(sta);
    }
    *ticks = lgs[0]+lgs[1]; // total time this process used the cpu
    *up = lgs[2];
    fclose(sta);
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
    strcat(pro,"/comm");    //concatenate the string to get the right /proc/{PID}/comm file
    FILE * comm = fopen(pro,"r");

    if( comm == NULL){
        printf("empty command");
        remPro(pid);    //remove the pid that's problably not attached to a running process
        return NULL;
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

void gettingStats(int ncall){    //using the stat function for all item in the list
    Node * temp = *getHead();
    while(temp!=NULL){
        if(ncall == 1)
            stat(&temp->value.ticks1,&temp->value.startime,temp->value.pid);    //get all processes ticks
        else if( ncall == 2)
            stat(&temp->value.ticks2,&temp->value.startime,temp->value.pid);
        temp = temp->next;
    }
}

void calcPercsNStore(){
    Node * temp = *getHead();
    long hz = sysconf(_SC_CLK_TCK); //used to transform ticks into seconds
    while(temp != NULL){
        temp->value.perc = (float) ( temp->value.ticks2 - temp->value.ticks1 ) / hz ;
        temp = temp->next;
    }
}

void getCommands(){ //get the name of the command to all processes
    Node * temp = *getHead();
    while(temp!=NULL){
        temp->value.comm = command(temp->value.pid);    //getting the names of the commands running the processes
        temp = temp->next;
    }
}