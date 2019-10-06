#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


/**
 *
 * The main method takes argument forom the comman line interface
 */
int main(int argc, char *argv[])
{
    pid_t pid; // defining the process ID
    
    if(argc < 2)
    {
        printf("Please provide the instruction in the format: %s [arg 1], arg[2]...",argv[1]);
    }
    pid = fork();
    if(pid < 0)
    {
        perror("Fork Failed");
        exit(0);
    }
    else if(pid == 0)
    {
        printf("Executing %s %s \n",argv[1],argv[2]);
        execl(argv[1],argv[2],argv[3],(char *) NULL);
        perror("execl");
        exit(0);
    }
    else // parent Block
    {
        wait(NULL);
    }
    
    return 1;
    
}


