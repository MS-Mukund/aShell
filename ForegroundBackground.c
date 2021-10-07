#include "headers.h"

int ProcessCount = 0;
int AllProcess = 0;
BackPro *ProcessList = NULL;

int AddProcess(pid_t pid, char **tmp, int argc)
{
   
    if (ProcessCount >= MAX_PROCESS) // kill a process randomly
    {
        if (kill(ProcessList->pid, SIGKILL) == -1)
        {
            printf("Error killing process\n");
            _exit(errno);
        }
        
        BackPro *tmp = ProcessList;
        ProcessList = ProcessList->next;

        for( int i = 0; i < tmp->argc; i++)
        {
            free(tmp->argv[i]);
        }
        free(tmp->argv);
        free(tmp);
    }

    BackPro *new = (BackPro *)malloc(sizeof(BackPro));
    if (new == NULL)
    {
        printf("Not enough memory\n");
        return -1;
    }
    new->pid = pid;
    AllProcess++;
    new->job_id = AllProcess;
    new->argc = argc;
    new->argv = (char **)malloc(argc * sizeof(char *));
    for( int i = 0; i < argc; i++)
    {
        // printf( "wow %s\n", tmp[i]);
        new->argv[i] = (char *)malloc(strlen(tmp[i]) + 1);
        strcpy(new->argv[i], tmp[i]);
        // printf( "%s\n", new->argv[i]);
    } 
    // new->next = ProcessList;
    // if( ProcessList == NULL )
    // {
    //     new->job_id = 1;
    // }
    // else
    // {
    //     new->job_id = ProcessList->job_id + 1;
    // }
    // ProcessList = new;

    BackPro *Head = ProcessList, *prev = NULL;
    while( Head != NULL)
    {
        if( strcmp( new->argv[0], Head->argv[0]) > 0) // alphabetical order
        {
            if( prev == NULL)
            {
                new->next = Head;
                ProcessList = new;
            }
            else
            {
                prev->next = new;
                new->next = Head;
            }
            break;
        }
        prev = Head;
        Head = Head->next;
    }

    if( Head == NULL)
    {
        if( prev == NULL)
        {
            new->next = NULL;
            ProcessList = new;
        }
        else
        {
            prev->next = new;
            new->next = NULL;
        }
    }

    ProcessCount++;

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
        
        if( tmp->pid == pid )
        {
            strcpy(str, tmp->argv[0]);
            if( prev == NULL)
            {
                ProcessList = tmp->next;
            }
            else
            {
                prev->next = tmp->next;
            }

            for( int i = 0; i < tmp->argc; i++)
            {
                free(tmp->argv[i]);
            }
            free(tmp->argv);
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
    if( pid == -1 || pid == 0 )
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
        perror("malloc error");
        _exit(errno);
    }
    int backgr = 0;
    int flag = 0;
    for (int i = 0, j = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "&") == 0)
        {
            backgr = 1;
            argv[i][0] = '\0';
            flag = 1;
            continue;
        }
        tmp[j] = (char *)malloc(sizeof(char) * MAX_SIZE);
        if (tmp[j] == NULL)
        {
            perror("malloc error");
            _exit(errno);
        }

        strcpy(tmp[j], argv[i]);
        j++;
    }
    
    if (flag == 1)
    {
        argc--;
    }
    tmp[argc] = NULL;

    if (backgr == 1)
    {
        sig_t sig = signal(SIGCHLD, waiting_func);
        const sigset_t TermIO[2] = { SIGTTIN, SIGTTOU };    
        if( sigprocmask( SIG_BLOCK, TermIO, NULL ) < 0)
        {
            perror( "Sigprocmask() error");
            return errno;
        }
    }

    pid_t par = getpid();
    pid_t pid = fork();
    int ret = 0;
    if (pid < 0)
    {
        perror("Fork error");
        return errno;
    }

    tcsetpgrp( STDIN_FILENO, par );
    tcsetpgrp( STDOUT_FILENO, par );

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
        printf( "cpro\n");
    }

    else // Parent
    {
        if ( backgr == 1 && AddProcess( pid, tmp, argc ) == -1)
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

        // for( int i = 0; i < argc; i++)
        // {
        //     free(tmp[i]);
        // }
        // free(tmp);
    }

    return 0;
}
