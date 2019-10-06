/* Example of using sigaction() to setup a signal handler with 3 arguments
 * including siginfo_t.
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>

static void usrhandler(int sig, siginfo_t *siginfo, void *ignore)
{
    
    printf ("Sending PID: %ld, UID: %ld\n",
            (long)siginfo->si_pid, (long)siginfo->si_uid);
    
}

int main (int argc, char *argv[])
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = usrhandler;
    
    if(sigaction(SIGINT,&sa,NULL) == -1)
    {
        perror("Sigaction");
        exit(errno);
    }
    while(1)
    {
        printf("Hello world I am %d\n",getpid());
        sleep(100);
        pid_t pid = fork();
        if(pid == 0)
        {
            sa.sa_sigaction = SIG_IGN;
            sigaction(SIGUSR1 , &sa, NULL);
            execl("/bin/sleep","Sleep", "100013", NULL);
            perror("exec sleep");
            exit(errno);
            
        }
        printf("Started %d", pid);
        int cstatus;
        pid_t wpid;
        do{
        wpid = wait(&cstatus);
        perror("wait");
        }while(waitpid == -1);
        printf("Child %d exited with %d sig %d\n", wpid, WEXITSTATUS(cstatus), WTERMSIG(cstatus));
    }
    
    return 0;
    
}
