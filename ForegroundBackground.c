#include "headers.h"

typedef struct BackgroundProcess
{
    pid_t pid;
    char name[MAX_SIZE];
    struct BackgroundProcess *next;
} BackPro;

int ProcessCount = 0;
BackPro *ProcessList = NULL;

int AddProcess(pid_t pid, char *name)
{
   
    if (ProcessCount >= MAX_PROCESS)
    {
        if (kill(ProcessList->pid, SIGKILL) == -1)
        {
            printf("Error killing process\n");
            return -1;
        }

        ProcessList->pid = pid;
        strcpy(ProcessList->name, name);
    }

    else
    {
        BackPro *new = (BackPro *)malloc(sizeof(BackPro));
        if (new == NULL)
        {
            printf("Not enough memory\n");
            return -1;
        }

        new->pid = pid;
        strcpy(new->name, name);
        new->next = ProcessList;

        ProcessList = new;
        ProcessCount++;
    }

    return 0;
}

void FindAndDelProcess(pid_t pid, char str[])
{   
    BackPro *tmp = ProcessList;
    BackPro *prev = NULL;
    int i = 0;
    for ( i = 0; i < MAX_PROCESS; i++)
    {
        if( tmp == NULL)
        {
            i = MAX_PROCESS;
            break;
        }
        
        if( tmp->pid == pid)
        {
            strcpy(str, tmp->name);
            if( prev == NULL)
            {
                ProcessList = tmp->next;
            }
            else
            {
                prev->next = tmp->next;
            }
            free(tmp);
            ProcessCount--;
            break;
        }

        prev = tmp;
        tmp = tmp->next;
    }

    if( i == MAX_PROCESS)
    {
        str[0] = '\0';
    }
}

void waiting_func(int signum)
{
    // waitpid( signum, NULL, 0 );
    int child_stat;
    pid_t pid = waitpid(-1, &child_stat, WNOHANG | WUNTRACED);
    if( pid == -1 )
    {
        // printf( "error: waitpid() failed\n");
        return;
    }

    if( pid == 0 ) // no change
    {
        return;
    }

    char str[MAX_SIZE];
    FindAndDelProcess(pid, str);
    if (str[0] == '\0')
    {
        fprintf(stderr, "error: process not found\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "\n%s with pid %d exited ", str, pid);

    if (WIFEXITED(child_stat))
    {
        fprintf(stderr, "normally\n");
    }
    else
    {
        fprintf(stderr, "abnormally\n");
    }

    yellow();
    PromptDisplay();
    reset();

    fflush(stdout);

    return;
}

int ForegrBackgr(char argv[][MAX_SIZE], int argc)
{
    // printf( "Foreground and background shell processes\n" );

    char **tmp = (char **)malloc(sizeof(char *) * (argc + 1));
    if (tmp == NULL)
    {
        printf("Error: not enough memory\n");
        return -1;
    }
    int backgr = 0;

    for (int i = 0; i < argc; i++)
    {
        // printf( "%d  %d\n", i, argc);
        if (strcmp(argv[i], "&") == 0)
        {
            // printf( "ok\n");
            backgr = 1;
            argv[i][0] = '\0';
            continue;
        }

        tmp[i] = (char *)malloc(sizeof(char) * MAX_SIZE);
        if (tmp[i] == NULL)
        {
            printf("Error: not enough memory\n");
        }

        strcpy(tmp[i], argv[i]);
    }
    tmp[argc] = NULL;

    if (backgr == 1)
    {
        sig_t sig = signal(SIGCHLD, waiting_func);
        const sigset_t TermIO[2] = { SIGTTIN, SIGTTOU };    
        if( sigprocmask( SIG_BLOCK, TermIO, NULL ) < 0)
        {
            perror( "Sigprocmask() error: ");
            return errno;
        }

    }

    pid_t pid = fork();
    // setpgid(0, 0);
    // printf( "forked\n");
    int ret = 0;
    if (pid < 0)
    {
        perror("Fork error");
        return errno;
    }

    if (pid == 0) // Child
    {   
        if( backgr == 1)
        {
            setpgid(0, 0);
            signal( SIGTTIN, SIG_DFL );
            signal( SIGTTOU, SIG_DFL );
        }
        if (execvp(tmp[0], tmp) == -1)
        {
            fprintf( stderr, "Exec error: Either the command is not applicable, or it doesn't exist\n");
            return errno;
        }

        for( int i = 0; i < argc; i++)
        {
            // printf( "ok \n");
            //printf( "what ");
            free(tmp[i]);
        }
        // free(tmp);
    }

    else // Parent
    {
        // printf( "parent\n");
        if ( backgr == 1 && AddProcess( pid, tmp[0] ) == -1)
        {
            printf("Error: process already exists\n");
            return -1;
        }

        if (backgr == 0)
        {
            waitpid(pid, &ret, 0);
        }

        if (backgr == 1)
        {
            printf("%d\n", pid);
        }
    }

    return 0;
}
