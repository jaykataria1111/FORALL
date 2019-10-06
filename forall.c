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
#include <fcntl.h> // open function
#include <signal.h> // Signal Action.


pid_t child;                 // Is the current child that we are going to handle if problems come by.
pid_t main_process;          // main_process is the main process that we are going to handle.



/**
 * The following function handles the signals that come through during the execution.
 *  The following piece of code was written by jay katariya, The professor helped jay with the execution.
 *  He guided jay by using the yes program and strace.
 *  @param sig which is the signal
 *  @param *siginfo is the signal information struct which can be used to access the information of the sending process
 *  @param *ucontext is the old context which we generally ignore.
 *  Note : The variable name may be the same of the linux man pages since that was what was used for the methods.
 */
static void handler(int sig, siginfo_t *siginfo, void *ucontext)
{
    
    if(sig == SIGINT)     // If the signal is SIGINT
    {
        dprintf(STDOUT_FILENO,"Signaling %d\n", child);   //Terminal
        kill(child,sig);                                    // Killing the child process
    }
    
    if(sig == SIGQUIT)                                  // IF the Signal is sigQUIT
    {
        kill(child,sig);
        kill(main_process,sig);
        dprintf(STDOUT_FILENO,"Signaling %d\n", main_process);
        dprintf(STDOUT_FILENO,"Exiting due to quit signal\n");
        exit(0);
    }
    
}


/**
 *
 * The main method takes argument forom the comman line interface
 * All of the following code was written by jay Katariya, Professor Ben Reed helped him with the code
 * @param argc is the command line arguments that are used.
 * @param argv is the argument array.
 * Have used online recources such as linux man pages and  https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.1.0/com.ibm.zos.v2r1.bpxbd00/rtsigac.htm to learn how
 *  Sigaction works.
 *  All the code was written by Jay.
 */
int main(int argc, char *argv[])
{
   
    
    main_process = getpid();      // Sets the global variable for the the main process of the pid.
    struct sigaction act;         // Definging the struct for sigaction.
    act.sa_flags = SA_SIGINFO;    // If this is set to SIGINFO, it will go to the function, i.e. sigaction instead of sa_handler.
    sigemptyset(&act.sa_mask);    //mask
    act.sa_sigaction = handler;   // passing the handler so that it can call.
    
    pid_t pid; // defining the process ID
    
    
    if(sigaction(SIGQUIT,&act,NULL) == -1)
    {
        perror("Sigaction");            // Error cheking for SIGINT   i.e. control C
        exit(errno);                    // Code was inspired from Professor Bens Code for what he wrote on the class for signals on 3/4/19
    }
    
    
    if(sigaction(SIGINT,&act,NULL) == -1)
    {
        perror("Sigaction");           // Error checking for SIGQUIT i.e. control
        exit(errno);                   // Code was inspired from Professor Bens Code for what he wrote on the class for signals on 3/4/19
    }
    
    if(argc < 2)
    {
        printf("Please provide the instruction in the format: %s [arg 1], arg[2]...",argv[1]);  // Argumental Checking
    }
    for(int i=2; i<argc ; i++)
    {
        
        
        char filename[50];                                  // To create a file
        char *str = ".out";                                 // TO use .out
        sprintf(filename,"%d%s",i-1,str);                   // Creating the file name.
        
    
        pid = fork();                                       // Fork Called
        child = pid;                                        // Assigning global child variable.
        if(pid < 0)
        {
            perror("Fork Failed");                          // Error Handling for Fork.
            exit(errno);
        }
        else if(pid == 0)
        {
        
            close(STDOUT_FILENO);           //Stdout Close
            close(STDERR_FILENO);           //STDERR Close
            int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, S_IWUSR | S_IRUSR );  // opening a file using the following flags. Professor Ben Reed has shown the code using iClicker.
            //sleep(10);
            
            
                dprintf(1,"Executing %s %s \n",argv[1],argv[i]);            // Writing in the file.
            
                if(execlp(argv[1],"Cool",argv[i],(char *) NULL) == -1)            // Executing the file with the name cool in as an argument.
                {
                    perror("execl");                                            // Exel error handling.
                    exit(errno);
                }
                exit(0);                            // Not suppposed to execute this!

        }

//        else // parent Block
//        {
            int stat;                   // Status of the child process will be stored here!
            int wpid;                   // wpid is the wait return
        
            do {
                wpid = wait(&stat);
                //perror("wait");                   for debugging not in the desired output as shown in the assignment
            } while (wpid == -1 && errno == EINTR);         // Error handling for the wait call.Professor Ben put this in the discussions
            
            int fd = open(filename, O_APPEND | O_RDWR, S_IWUSR | S_IRUSR );     // opening the file descriptor again, Professor Ben Reed showed the code using iClicker, implemented that code.
        
        
        if(WIFSIGNALED(stat))                           //Checking if the child process was signaled.
            {
                
                int term_sig = WTERMSIG(stat);
                dprintf(fd,"Stopped executing %s %s signal = %d\n",             // The output of the child process here if it was signaled.
                        argv[1],argv[i],term_sig);                              // Professor ben reed during office hours showed how to do this.
            }
        
//            if(!WIFEXITED(stat))                                                // Checking if the child process executed properly
//            {
//                printf("Failed to write in %d\n",pid);                            // For debugging purposes only ! Not supposed to be in the code.
//            }
        
            if(WIFEXITED(stat))                                                 // Checking the exit status of the child.
            {
                int exit_stat = WEXITSTATUS(stat);                                  // if the child executed properly the next statement will be printed.
                dprintf(fd,"Finished executing %s %s exit code = %d\n",                     // During office hours professor ben reed showed how to do this.
                        argv[1],argv[i],exit_stat);
            }
//       }

    }

    return 1;                           // The program executed.
}

