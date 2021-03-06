#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "listOfProcess.h"

Node * head = NULL;

void addPro(char * pid){
    Process * newp = (Process *) malloc( sizeof(Process *) );
    newp->pid = pid;
    if(head == NULL){
        head = (Node *) malloc(sizeof(Node));
        head->value = *newp;
        head->next = NULL;
    }else{
        Node * temp = head;

        while(temp->next!= NULL){
            temp = temp->next;
        }

        temp->next = (Node *)malloc(sizeof(Node));
        temp->next->value = *newp;
        temp->next->next = NULL;
    }
}

void remPro(char * pid){
    if(head != NULL){
        if(strcmp(head->value.pid,pid)==0){
            Node * temp = head->next;
            free(head);
            head = temp;
        }else{
            Node * temp=head;
            while( temp->next!=NULL && strcmp(temp->next->value.pid,pid)!=0){
                temp = temp->next;
            }
            if(temp->next!=NULL){
                Node * temp2 = temp->next;
                temp->next = temp2->next;
                free(temp2);
            }
        }
    }
}

Process * findPro(char * pid){
    if(head!=NULL){
        Node * temp = head;
        while(temp!=NULL && strcmp(temp->value.pid,pid)!=0){
            temp = temp->next;
        }
        if(temp!=NULL){
            return &temp->value;
        }
    }
    return NULL;
}

Node ** getHead(){
    return &head;
}

void initializeProcValues(){
    if(head != NULL){
        Node * temp = head;
        while(temp != NULL){
            temp->value.startime = 0;
            temp->value.ticks1 = 0;
            temp->value.ticks2 =0;
            temp = temp->next;
        }
    }
}

void recfreeAll(Node * r){
    if(r == NULL){
        return;
    }else{
        recfreeAll(r->next);
    }
    free(r);
}

void freeAll(){
    recfreeAll(head);
    head = NULL;
}

int recsize(Node * r){
    if(r == NULL){
        return 0;
    }
    return 1 + recsize(r->next);
}

int size(){
    return recsize(head);
}

void printLista(){
    Node *temp = head;
    while(temp != NULL) {
        printf("%s",temp->value.comm);
        temp = temp->next;
    }
}

//using merge sort for now, even though knowing the high usage

Node * sortedMerge(Node *a, Node *b){
    Node * r = NULL;
    if(a == NULL)
        return b;
    else if(b == NULL)
        return a;
    
    if(a->value.perc > b->value.perc){
        r = a;
        r->next = sortedMerge(a->next,b);
    }else{
        r = b;
        r->next = sortedMerge(a,b->next);
    }
    return r;
}

void split(Node *src, Node ** front, Node **back){
    Node* fast; 
    Node* slow; 
    slow = src; 
    fast = src->next; 

    while (fast != NULL) { 
        fast = fast->next; 
        if (fast != NULL) { 
            slow = slow->next; 
            fast = fast->next; 
        } 
    } 
  
    *front = src; 
    *back = slow->next; 
    slow->next = NULL;  
}

void mergeSortRec(Node ** headRef){
    Node *h = *headRef;
    Node *a, *b;
    if(h != NULL && h->next != NULL){
        split(h, &a, &b);

        mergeSortRec(&a);
        mergeSortRec(&b);

        *headRef = sortedMerge(a,b);
    }
}

void mergeSort(){
    mergeSortRec(&head);
}