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
#define REQUEST "REQUEST"
#define RELEASE "RELEASE"
#define LIST "LIST"
#define FIND "FIND"
#define AVAILABLE "AVAILABLE"
#define ASSIGNED "ASSIGNED"


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
    FILE* file; //instruction file
    node* nHead; //the dllist of nodes
    struct process* next; //the next process in the RR
    int pNum; //ID to keep track of processes
    int line; //line of execution
    int isLast; //flag for element that connects behind the pHead
    unsigned long pos; //position in the instruction file
    int deadlock; //tracker for contiguous deadlocks in a process
} process;

/*
* get the name for the process
* @param:line = instruction line
* @param:pname = place to store name
* @param:i = index of line
*/
void getName(byte* line, byte* pname, int* i)
{
    int j = 0;
    byte name[17] = {0};
    // get the process name
    while (line[*i] != '\n' && line[*i] != '\000' && line[*i] != ' ')
    {
        name[j] = line[*i];
        ++*i;
        ++j;
    }
    strcpy(pname, name); //set the name
}

/*
 * release the requested node
 * @param:head = head of nodes
 * @param:pname = name of process
 * @param:actualSize = amount allocated
 */
void release(node** head, byte pname[17], long unsigned int *actualSize)
{
    // nothing allocated
    if (*actualSize == 0)
    {
        printf("FAIL %s %s\n", RELEASE, pname);
        return;
    }

    node* temp;
    temp = *head;
    // loop through the nodes
    while (temp->next != NULL)
    {
        // found node to release
        if(strcmp(pname, temp->pname) == 0)
        {
            // free head node and reassign
            if (temp == *head)
            {
                temp->next->prev = NULL;
                *head = temp->next;
                *actualSize -= temp->size; //decrement actualSize
                printf("FREE %s %ld %ld\n", temp->pname, temp->size, temp->location);
                free(temp);
                temp = NULL;
                return; //done with function
            }
            // node besides the head
            else
            {
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
                *actualSize -= temp->size; //decrement actualSize
                printf("FREE %s %ld %ld\n", temp->pname, temp->size, temp->location);
                free(temp);
                temp = NULL;
                return; //done with function
            }
        }
        temp = temp->next;
    }
    // check last value and head
    if(strcmp(pname, temp->pname) == 0)
    {
        // head of nodes, only node in list
        if (temp->prev == NULL)
        {
            *actualSize -= temp->size;
            printf("FREE %s %ld %ld\n", temp->pname, temp->size, temp->location);
            free(temp);
            temp = NULL;
            *head = NULL;
            return;
        }
        // last node
        else
        {
            temp->prev->next = temp->next;
            *actualSize -= temp->size;
            printf("FREE %s %ld %ld\n", temp->pname, temp->size, temp->location);
            free(temp);
            temp = NULL;
            return;
        }

    }
        // name not found
    else
    {
        printf("FAIL %s %s\n", RELEASE, pname);
        return;
    }
}


/*
 * list available and assigned spots in memory
 * @param:head = head node
 * @param:c2 = second command
 * @param:actualSize = total amount allocated
 * @param:space = total space we are using
 */
void list(node** head, byte c2[17], long unsigned int actualSize, long unsigned int space)
{
    // display free spots
    if (strcmp(c2, AVAILABLE) == 0)
    {
        // empty
        if (actualSize == 0)
        {
            printf("(%ld, %ld)\n", space, actualSize);
            return;
        }
        else if (actualSize == space)
        {
            printf("FULL\n");
            return;
        }
        else
        {
            // check before the head
            if ((**head).location != 0)
            {
                printf("(%ld, %ld) ", (**head).location, (long unsigned int)0);
            }
            node* temp = *head;
            long unsigned int distance;
            // loop through the list
            while (temp->next != NULL)
            {
                distance = (temp->next->location) - (temp->location + temp->size);
                // display free room
                if (distance > 0)
                {
                    printf("(%ld, %ld) ", distance, (temp->location + temp->size));
                }
                temp = temp->next;
            }
            // check last node
            if ((temp->location + temp->size) < space)
            {
                distance = space - (temp->location + temp->size);
                printf("(%ld, %ld)", distance, (temp->location + temp->size));
            }
            printf("\n");
            return;
        }
        printf("FULL\n");
    }
        // list assigned spots
    else if (strcmp(c2, ASSIGNED) == 0)
    {
        node* temp = *head;
        if (actualSize == 0)
        {
            printf("NONE\n");
        }
        else
        {
            //loop through list
            while (temp->next != NULL)
            {
                printf("(%s, %ld, %ld) ", temp->pname, temp->size, temp->location);
                temp = temp->next;
            }
            printf("(%s, %ld, %ld)\n", temp->pname, temp->size, temp->location);
        }
    }
}

/*
 * find a specific node and display size and location
 * @param:head = head node
 * @param:pname = name to search for
 * @param:actualSize = amount alloc'ed
 */
void find(node **head, byte pname[17], long unsigned int actualSize)
{
    if (actualSize == 0)
    {
        printf("FAULT\n");
        return;
    }
    node* temp = *head;
    // loop through nodes
    while (temp->next != NULL)
    {
        // node found
        if (strcmp(temp->pname, pname) == 0)
        {
            printf("(%s, %ld, %ld)\n", pname, temp->size, temp->location);
            return; //found node
        }
        temp = temp->next;
    }
    // last node
    if (strcmp(temp->pname, pname) == 0)
    {
        printf("(%s, %ld, %ld)\n", pname, temp->size, temp->location);
        return; //found node
    }
    printf("FAULT\n");
}

/*
 * creates the llist for the processes, formatting them in a round-robin style
 * @param:pHead = the head process of the llist to start with
 * @param:pNum = the amount of processes we want to make
 */
void createProcs(process** pHead, int pNum, node** nHead)
{
    char fileNum[6] = {0}; //used to hold the file name
    *pHead = (process*)malloc(sizeof(process)); //allocates room for the first process
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
            pTemp->nHead = *nHead;
            pTemp->line = 0;
            pTemp->pos = 0;
            pTemp->pNum = i;
            pTemp->deadlock = 0;
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
 * @param:head = head node
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
 * @param:pHead = the first (head) process
 */
void closeProcs(process** pHead, node** nHead)
{
    process* cur = *pHead;
    process* temp;

    if (cur == NULL)
    {
        return; //nothing to free
    }
    else
    {
        // delete the dllist of nodes in the process if it exists
        if (*nHead != NULL)
        {
            deleteNodes(nHead);
        }
        // delete the processes
        while (cur->isLast != TRUE)
        {
            fclose(cur->file); //close the process file
            temp = cur->next;
            free(cur);
            cur = temp;
        }
        // close last process in the round-robin
        fclose(cur->file);
        free(cur);
        *pHead = NULL;
    }
}

/*
 * set the node heads on all the processes (for when a new head is created in a process)
 * @param:curProc = the head process with the new head
 */
void setHeads(process** curProc, node** nHead)
{
    process* temp = *curProc;
    temp = temp->next;
    // loop through the procs and set each nHead to point to the same address
    while (temp != *curProc)
    {
        temp->nHead = *nHead;
        temp = temp->next;
    }
    temp->nHead = *nHead; //last proc
}

void firstfit(process **curProc, int quantum, int space, long unsigned int* actualSize, node** nHead)
{
    byte line[50] = {'\n'}; //execution line
    byte command[10] = {0}; //command

    int j = 0; //index of instruction line
    //fflush((*curProc)->file);
    // executing the number of instructions (quantum) specified for each process
    for (int i = 0; i < quantum; ++i)
    {
        if ((*curProc)->line < 0)
            break;

        (*curProc)->pos = ftell((*curProc)->file);
        fseek((*curProc)->file, (*curProc)->pos, SEEK_SET);
        // get instruction line
        if ((fgets(line, sizeof(line), (*curProc)->file)) != NULL)
        {
            // line is a comment
            if (line[0] == '#')
            {
                --i;
                continue;
            }
            // line is not a comment, get the command
            while (line[j] != ' ')
            {
                command[j] = line[j];
                ++j;
            }
            ++j; //skip the space

            // request allocation
            if (strcmp(command, REQUEST) == 0)
            {
                //************************************************************************************************** GET ALL THE DATA FOR THE REQUEST
                byte pname[17] = {0}; //process name
                byte bpsize[11] = {0}; //process size stored as bytes
                long unsigned int lpsize; //process size
                int nameLength = 0; //name length

                getName(line, pname, &j);
                nameLength = strlen(pname);
                ++j;

                int k = 0;
                // get the size of allocation
                while (line[j] != '\n' && line[j] != '\000')
                {
                    bpsize[k] = line[j];
                    ++k;
                    ++j;
                }
                lpsize = atol(bpsize); //size wanting to alloc
                //--------------------------------------------------------------------------------------------------------------------------

                // check if allocation size is valid
                if (lpsize > space || lpsize < 1 || (lpsize > space - *actualSize)) //not valid
                {
                    printf("FAIL %s %s %ld\n", REQUEST, pname, lpsize);
                    ++(*curProc)->deadlock;
                    fseek((*curProc)->file, (*curProc)->pos, SEEK_SET);
                    // 2 failed alloc's in a row
                    if ((*curProc)->deadlock > 1)
                    {
                        printf("DEADLOCK DETECTED\n");
                        exit(0);
                    }
                    return;
                }

                //*************************************************************************************************************** ACTUAL ALGORITHM
                else
                {
                    // first REQUEST, set up the node list
                    if (*nHead == NULL)
                    {
                        // set up the head ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SET UP HEAD NODE
                        *nHead = (node*)malloc(sizeof(node));
                        (*curProc)->nHead = *nHead;
                        (*curProc)->nHead->location = 0;
                        (*curProc)->nHead->next = NULL;
                        (*curProc)->nHead->prev = NULL;
                        memset((*curProc)->nHead->pname, 0, sizeof((*curProc)->nHead->pname));
                        // set the name
                        for (int letter = 0; letter < nameLength; ++letter)
                        {
                            (*curProc)->nHead->pname[letter] = pname[letter];
                        }
                        (*curProc)->nHead->size = lpsize;
                        *nHead = (*curProc)->nHead;
                        *actualSize += lpsize; // increase total size
                        printf("ALLOCATED %s %ld\n", pname, (*curProc)->nHead->location);
                        ++(*curProc)->line;

                        setHeads(curProc, nHead); //set the same head node for the pointers
                    }
                    // check the spot before the head for availability ~~~~~~~~~~~~~~~~~~~~ CHECK BEFORE THE HEAD NODE
                    else if ((*nHead)->location != 0 && (*nHead)->location >= lpsize)
                    {
                        node* newNode = (node*)malloc(sizeof(node));
                        memset(newNode->pname, 0, sizeof(newNode->pname));
                        /// set up node
                        newNode->next = *nHead;
                        newNode->prev = NULL;
                        (*nHead)->prev = newNode;
                        newNode->location = 0;
                        newNode->size = lpsize;
                        for (int letter = 0; letter < nameLength; ++letter)
                        {
                            newNode->pname[letter] = pname[letter];
                        }
                        ///
                        *actualSize += lpsize;
                        *nHead = newNode;
                        (*curProc)->nHead = *nHead;
                        printf("ALLOCATED %s %ld\n", pname, newNode->location);
                        ++(*curProc)->line;

                        setHeads(curProc, nHead);

                        (*curProc)->deadlock = 0; //reset the dl counter for the process on successful alloc
                    }
                    // check the rest of the dllist for room to alloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MAIN PART OF ALGO
                    else
                    {
                        int spotFound = FALSE;
                        node* newNode;
                        node* curNode = *nHead;

                        // loop through the nodes looking for room
                        while (curNode->next != NULL)
                        {
                            if ((curNode->next->location - (curNode->location + curNode->size)) > lpsize)
                            {
                                newNode = (node*)malloc(sizeof(node));
                                memset(newNode->pname, 0, sizeof(newNode->pname));

                                /// set up node
                                newNode->prev = curNode;
                                newNode->next = curNode->next;
                                newNode->next->prev = newNode;
                                curNode->next = newNode;
                                newNode->location = curNode->location + curNode->size;
                                newNode->size = lpsize;
                                for (int letter = 0; letter < nameLength; ++letter)
                                {
                                    newNode->pname[letter] = pname[letter];
                                }
                                ///
                                *actualSize += lpsize;
                                printf("ALLOCATED %s %ld\n", pname, newNode->location);
                                ++(*curProc)->line;

                                (*curProc)->deadlock = 0;
                                spotFound = TRUE;
                                break;
                            }
                            curNode = curNode->next;
                        }
                        // check the end of the allocated space (from last node to the end of total amount alloc'ed)
                        if (spotFound == FALSE && (space - (curNode->size + curNode->location)) >= lpsize)
                        {
                            newNode = (node*)malloc(sizeof(node));
                            memset(newNode->pname, 0, sizeof(newNode->pname));

                            /// set up node
                            newNode->prev = curNode;
                            newNode->next = NULL;
                            curNode->next = newNode;
                            newNode->location = curNode->location + curNode->size;
                            newNode->size = lpsize;
                            for (int letter = 0; letter < nameLength; ++letter)
                            {
                                newNode->pname[letter] = pname[letter];
                            }
                            ///
                            *actualSize += lpsize;
                            printf("ALLOCATED %s %ld\n", pname, newNode->location);
                            ++(*curProc)->line;

                            (*curProc)->deadlock = 0;
                        }
                        else if (spotFound == FALSE)
                        {
                            printf("FAIL %s %s %ld\n", REQUEST, pname, lpsize);
                            ++(*curProc)->deadlock;
                            fseek((*curProc)->file, (*curProc)->pos, SEEK_SET);
                            // 2 failed alloc's in a row
                            if ((*curProc)->deadlock > 1)
                            {
                                printf("DEADLOCK DETECTED\n");
                                exit(0);
                            }
                            return;
                        }
                    }
                }
                //-------------------------------------------------------------------------------------------------------------------------------
            }
            // release allocation
            else if (strcmp(command, RELEASE) == 0)
            {
                byte pname[17] = {0};
                getName(line, pname, &j);

                if (*nHead == NULL)
                {
                    printf("FAIL %s %s\n", RELEASE, pname);
                }
                else
                {
                    release(nHead, pname, actualSize);
                }
                ++(*curProc)->line;
            }
            // list available or assigned
            else if (strcmp(command, LIST) == 0)
            {
                byte c2[17] = {0};
                getName(line, (byte *)&c2, &j);
                list(nHead, c2, *actualSize, space);
                ++(*curProc)->line;
            }
            // find node
            else if (strcmp(command, FIND) == 0)
            {
                byte pname[17] = {0};
                getName(line, (byte *)&pname, &j);
                find(nHead, pname, *actualSize);
                ++(*curProc)->line;
            }
            // bad command
            else
            {
                fprintf(stderr, "invalid command\n");
            }
        }
        // set line number to negative to signify completion
        else
        {
            if ((*curProc)->line > 0)
                (*curProc)->line *= -1;
            else if ((*curProc)->line == 0)
                (*curProc)->line = -1;
        }
        j = 0;
        memset(command, 0, sizeof(command));
    }
}

int isComplete(process** pHead)
{
    process* temp = *pHead;

    while (temp->isLast == FALSE)
    {
        if (temp->line > 0)
            return FALSE;

        temp = temp->next;
    }
    if (temp->line > 0)
        return FALSE;

    return TRUE;
}

void firstfitRR(int space, int pNum, int quantum, process **pHead, long unsigned int* actualSize, node** nHead)
{
    process* curProc = *pHead;

    // main loop, goes through all the processes in a round-robin fashion until complete
    do
    {
        // loops through each process, from 0-[7]
        for (int i = 0; i < pNum; ++i)
        {
            // the instruction file (and in turn, the process) is done executing, so skip the process
            if (curProc->line < 0)
            {
                curProc = curProc->next;
                continue;
            }
            firstfit(&curProc, quantum, space, actualSize, nHead);
            curProc = curProc->next;
        }
    } while (isComplete(pHead) == FALSE);
}


/*
 * the main function, runs initial checks on the CL input and then calls a function for the appropriate style of allocation
 */
int main(int argc, char** argv)
{
    // check to make sure enough arguments are typed in the CL
    /*if (argc < 5)
    {
        fprintf(stderr, "not enough arguments\n");
        exit(-1);
    }*/

    int quantum = /*atoi(argv[1])*/8;
    int pNum = /*atoi(argv[2])*/8;
    int totalSpace = /*atoi(argv[3])*/653556;
    long unsigned int actualSize = 0; //amount of memory already allocated

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
    node* nHead = NULL;
    createProcs(&pHead, pNum, &nHead);


    if (strcmp(/*argv[4]*/"FIRSTFIT", FIRSTFIT) == 0)
    {
        firstfitRR(totalSpace, pNum, quantum, &pHead, &actualSize, &nHead);
    }
    else if (strcmp(argv[4]/*"BUDDY"*/, BUDDY) == 0)
    {
        // TODO: extra credit for implementing the buddy system
        return 0;
    }
    else
    {
        fprintf(stderr, "invalid method of allocation\n");
        closeProcs(&pHead, &nHead);
        exit(-1);
    }
    closeProcs(&pHead, &nHead); //free the processes and the nodes
}
