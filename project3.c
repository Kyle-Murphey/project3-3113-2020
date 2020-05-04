#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *  Project 3, final
 * Kyle Murphey
 * 05/06/20
 */

#define TRUE 1
#define FALSE 0
#define FIRSTFIT "FIRSTFIT"
#define BUDDY "BUDDY"

/* unsigned char */
typedef unsigned char byte;

/* node struct for holding the data, taken from my Project 2 */
typedef struct node {
    byte pname[17]; //name
    long unsigned int size; //size of alloc
    long unsigned int location; //starting point of data
    struct node* next; //next in line
    struct node* prev; //previous in line
} node;

/* process struct for each process to run */
typedef struct process {
    FILE* file;
    node* nHead;
    struct process* next;
    int pNum;
    int line;
    int isLast;
} process;

/*
 * creates the llist for the processes, formatting them in a round-robin style
 * @param:pHead = the head process of the llist to start with
 * @param:pNum = the amount of processes we want to make
 */
void createProcs(process** pHead, int pNum)
{
    char fileNum[6];
    *pHead = (process*)malloc(sizeof(process));
    process* pTemp = *pHead;

    // creates the correct number of processes
    for (int i = 0; i < pNum; ++i)
    {
        fileNum[0] = i + '0'; //holds name of file to open
        strcat(fileNum, ".ins"); //adds '.ins' to the pNum for file format

        // check if file is valid
        if ((pTemp->file = fopen(fileNum, "r")) == NULL)
        {
            fprintf(stderr, "error opening file: %s\n", fileNum);
            exit(-1);
        }
        // valid file found for process
        else
        {
            // initialize the process
            pTemp->nHead = NULL;
            pTemp->line = 0;
            pTemp->pNum = i;
            // all but last process
            if (pNum - i != 1)
            {
                pTemp->isLast = FALSE;
                pTemp->next = (process*)malloc(sizeof(process));
                pTemp = pTemp->next;
            }
            // last process in list, connect to nHead to complete round-robin
            else
            {
                pTemp->isLast = TRUE;
                pTemp->next = *pHead;
            }
        }
        memset(fileNum, 0, sizeof(fileNum)); //clear the file name
    }
}

/*
 * delete the linked list of nodes, taken from my Project 2
 * @param:nHead = nHead node
 */
void deleteNodes(node** head)
{
    node* curNode = *head;
    node* temp;
    while(curNode->next != NULL)
    {
        temp = curNode->next;
        free(curNode);
        curNode = temp;
    }
    free(curNode);
    *head = NULL;
}

/*
 * delete the list of processes
 * @param:pHead = the first process
 */
void closeProcs(process** pHead)
{
    process* cur = *pHead;
    process* temp;

    if (cur == NULL)
    {
        return; //nothing to free
    }
    else
    {
        while (cur->isLast != TRUE)
        {
            // delete the dllist of nodes in the process if it exists
            if (cur->nHead != NULL)
            {
                deleteNodes(&cur->nHead);
            }
            fclose(cur->file); //close the process file
            temp = cur->next;
            free(cur);
            cur = temp;
        }
        // close last process in the round-robin
        if (cur->nHead != NULL)
        {
            deleteNodes(&cur->nHead);
        }
        fclose(cur->file);
        free(cur);
        *pHead = NULL;
    }
}

/*
 * the main function, runs initial checks on the CL input and then calls a function for the appropriate style of allocation
 */
int main(int argc, char** argv)
{
    // check to make sure enough arguments are typed in the CL
   /* if (argc < 5)
    {
        fprintf(stderr, "not enough arguments\n");
        exit(-1);
    }*/

    int quantum = /*atoi(argv[1])*/ 8;
    int pNum = /*atoi(argv[2])*/ 8;
    int totalSpace = /*atoi(argv[3])*/ 64;

    // check for valid quantum size
    if (quantum < 1 || quantum > 8)
    {
        fprintf(stderr, "invalid quantum size\n");
        exit(-1);
    }
    // check for valid number of processes
    if (pNum < 1 || pNum > 8)
    {
        fprintf(stderr, "invalid number of processes\n");
        exit(-1);
    }
    // check for valid allocation size
    if (totalSpace < 16 || totalSpace > 1073741824)
    {
        fprintf(stderr, "invalid allocation size\n");
        exit(-1);
    }

    process* pHead = NULL;
    createProcs(&pHead, pNum);

    closeProcs(&pHead);
    printf("YUHHHHHHHHHH");
    exit(-69);

    if (strcmp(argv[5]/*"FIRSTFIT"*/, FIRSTFIT) == 0)
    {
        //bestfit(file, size, &head);
    }
    else if (strcmp(argv[5]/*"BUDDY"*/, BUDDY) == 0)
    {
        // TODO: extra credit for implementing the buddy system
    }
    else
    {
        fprintf(stderr, "invalid method of allocation\n");
        closeProcs(&pHead);
        exit(-1);
    }
}
