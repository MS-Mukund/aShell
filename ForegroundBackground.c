#include "headers.h"

int ProcessCount = 0;
int AllProcess = 0;
BackPro *ProcessList = NULL;
BackPro *FgPro = NULL;

void waiting_func(int signum);
void FindAndDelProcess(pid_t pid, char str[], int is_bg );

int AddProcess(pid_t pid, char **tmp, int argc, int is_bg)
{
    // printf( "entered\n");
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
    if( is_bg )
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

    BackPro *Head = ProcessList, *prev = NULL;
    if( !is_bg )
    {
        Head = FgPro;
    }
    while( Head != NULL)
    {
        if( strcmp( new->argv[0], Head->argv[0]) < 0) // alphabetical order
        {
            if( prev == NULL)
            {
                new->next = Head;
                if( is_bg )
                    ProcessList = new;
                else
                    FgPro = new;
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
            if( is_bg )
                ProcessList = new;
            else
                FgPro = new;
        }
        else
        {
            prev->next = new;
            new->next = NULL;
        }
    }

    if( is_bg )
        ProcessCount++;

    return 0;
}

void FindAndDelProcess(pid_t pid, char str[], int is_bg )
{   
    BackPro *tmp = ProcessList;
    if( !is_bg )
        tmp = FgPro;
    BackPro *prev = NULL;

    // printf( "deleting %d\n", pid);
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
                if( is_bg )
                    ProcessList = tmp->next;
                else
                    FgPro = tmp->next;
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
            
            if( is_bg )
                ProcessCount--;
            break;
        }

        if( !is_bg )
        {
            FgPro = NULL;
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
    pid_t pid = waitpid(-1, &child_stat, WNOHANG );
    if( pid == -1 || pid == 0 )
    {
        return;
    }
    printf( "%d here\n", pid );

    char str[MAX_SIZE];
    FindAndDelProcess(pid, str, 1);
    if (str[0] == '\0')
    {
        fprintf(stderr, "error: process not found\n");
        return;
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
    char **MegaTmp = (char **)malloc(sizeof(char *) * (argc + 1));
    char RandStr[MAX_SIZE];
    if (MegaTmp == NULL)
    {
        perror("malloc error");
        _exit(errno);
    }
    int backgr = 0;
    for (int i = 0, j = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "&") == 0)
        {
            backgr = 1;
            argv[i][0] = '\0';
            continue;
        }
        MegaTmp[j] = (char *)malloc(sizeof(char) * MAX_SIZE);
        if (MegaTmp[j] == NULL)
        {
            perror("malloc error");
            _exit(errno);
        }

        strcpy(MegaTmp[j], argv[i]);
        j++;
    }
    
    if (backgr == 1)
        argc--;
    MegaTmp[argc] = NULL;

    if (backgr == 1)
    {
        sig_t sig = signal(SIGCHLD, waiting_func);
        printf( "%p\n", sig);
    }

    pid_t par = getpid();
    pid_t pid = fork();

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
            // signal( SIGTTIN, SIG_DFL );
            // signal( SIGTTOU, SIG_DFL );
        }

        if (execvp(MegaTmp[0], MegaTmp) == -1)
        {
            fprintf( stderr, "Exec error: Either the command is not applicable, or it doesn't exist\n");
            _exit(errno);
        }
    }

    else // Parent
    {
        if ( backgr == 1 && AddProcess( pid, MegaTmp, argc, 1 ) == -1)
        {
            printf("Error: process already exists\n");
            return -1;
        }

        if (backgr == 0)
        {
            FgId = pid;
            AddProcess( pid, MegaTmp, argc, 0 );
            int ret2 = waitpid(pid, &ret, WUNTRACED );
            if( ret2 == -1)
            {
                perror("waitpid error");
                return errno;
            }

            FindAndDelProcess(pid, RandStr, 0);
            FgId = -1;
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
