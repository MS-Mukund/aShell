#include "headers.h"

/********************************************
 * Contains:
 * jobs() - list of all background processes
 * sig() - Send a signal to a process
 * BringFg() - Bring a process to the foreground
 * BringBg() - Send a process to the background
********************************************/

int jobs( char argv[][MAX_SIZE], int argc )
{
    int running = 1;
    int stopped = 1;
    for( int i = 0; i < argc; i++ )
    {
        if( argv[i][0] == '-' )
        {
            if( argv[i][1] == 'r')
            {
                running = 1;
            }

            else if( argv[i][1] == 's' )
            {
                stopped = 1;
            }
        }
    }

    if( running == 0 && stopped == 0) // invalid args
    {
        running = 1;
        stopped = 1;
    }

    // print all bg processes
    char str[MAX_SIZE];

    for( BackPro *Ptr = ProcessList; Ptr != NULL; Ptr = Ptr->next )
    {
        // printf( "ok\n");
        sprintf( str, "/proc/%d/stat", Ptr->pid );
        FILE *fp;
        // printf( "got it\n");
        if( (fp = fopen( str, "r" ) ) < 0 )
        {
            perror( "open error");
            return 0;
        }
        // printf( "got it\n");

        char runn[MAX_SIZE];
        for( int i = 0; i < 3; i++)
        {
            // xprintf( "%d %d\n", i, sizeof(fp));
            fscanf( fp, "%s", runn );
        }
        // printf( "got it\n");
        printf( "[%d] ", Ptr->job_id );
    
        if( strcmp( runn, "S") == 0 || strcmp(runn, "R") == 0 )
        {
            if( running == 1 )
            {
                printf( "running ");
            }
        }
        else
        {
            if( stopped == 1 )
            {
                printf( "stopped ");
            }
        }
        // printf( "joke\n");

        for( int j = 0; j < Ptr->argc ; j++ )
        {
            printf( "%s ", Ptr->argv[j] );
        }
        printf( "[%d]\n", Ptr->pid );
    }

    return 0;
}

int sig ( char argv[][MAX_SIZE], int argc )
{
    if( argc != 3 )
    {
        printf( "Signal usage: \"sig [job_id] [signal_no]\" \n");
        return 1;
    }

    int job_id = atoi( argv[1] );
    int sig_no = atoi( argv[2] );

    if( sig_no > 31 || sig_no < 1 )
    {
        printf( "Invalid signal number\n");
        return 3;
    }

    BackPro *Ptr = ProcessList;
    for( Ptr = ProcessList; Ptr != NULL; Ptr = Ptr->next )
    {
        if( Ptr->job_id == job_id )
        {
            char str[MAX_SIZE];
            sprintf( str, "/proc/%d/stat", Ptr->pid );

            FILE *fp;
            if( (fp = fopen( str, "r" ) ) < 0 )
            {
                perror( "open error");
                _exit(errno);
            }

            char runn[MAX_SIZE];
            for( int i = 0; i < 3; i++)
            {
                fscanf( fp, "%s", runn );
            }
            if( strcmp( "S", runn) == 0 || strcmp( "R", runn) == 0 )
            {
                if( kill( Ptr->pid, sig_no ) < 0)
                {
                    perror( "kill error");
                    return errno;
                }
                return 0;
            }
            else
            {
                printf( "The process is not running\n");
                return 2;
            }
        }
    }

    if( Ptr == NULL )
    {
        printf( "Invalid job id\n" );
        return 2;
    }
}

int BringFg( char argv[][MAX_SIZE], int argc )
{
    if( argc != 2 )
    {
        printf( "BringFg usage: \"BringFg [job_id]\" \n");
        return 1;
    }

    int job_id = atoi( argv[1] );
    char str[MAX_SIZE];

    BackPro *Ptr = ProcessList;
    for( ; Ptr != NULL; Ptr = Ptr->next )
    {
        if( Ptr->job_id == job_id )
        {
            // send this process to the foreground
            sprintf( str, "/proc/%d/stat", Ptr->pid );
            FILE *fp;
            if( (fp = fopen( str, "r" ) ) < 0 )
            {
                perror( "open() error");
                _exit(errno);
            }

            char runn[9][MAX_SIZE];
            for( int i = 0; i < 3; i++)
            {
                fscanf( fp, "%s", runn[i] );
            }

            // runn[8] contains foreground process grp id of controlling terminal
            // runn[2] contains status of process
            // runn[4] contains process group id of process

           
            if( kill( Ptr->pid, SIGCONT ) < 0)
            {
                perror( "kill() failed");
                return errno;
            }
                
            char str[MAX_SIZE];

            signal(SIGTTOU, SIG_IGN);   // parent should not take input
            signal(SIGTTIN, SIG_IGN);

            // giving terminal control to the process
            if( tcsetpgrp( STDIN_FILENO, Ptr->pid) < 0)
            {
                perror( "tcsetpgrp() error");
                return errno;
            }
            
            // wait for the process to finish
            pid_t Pr_pid = Ptr->pid;
            FgId = Pr_pid;
            int status;
            int ret;
            // printf( "%d\n", Ptr->pid);
            if( ( ret = waitpid( Pr_pid, &status, WUNTRACED | WCONTINUED ) ) == -1 )
            {
                perror( "waitpid() error");
                return errno;
            }

            // after child exits
            if( tcsetpgrp( STDIN_FILENO, ParId ) < 0 )
            {
                perror( "tcsetpgrp() error");
                return errno;
            }

            // give parent control of terminal
            signal(SIGTTOU, SIG_DFL);
            signal(SIGTTIN, SIG_DFL);

            if( !WIFSTOPPED(status) ) // not stopped
            {
                printf( "not stopped\n");
                FindAndDelProcess( Pr_pid, str );
            }
            
            return 0;
        }
    }

    if( Ptr == NULL )
    {
        printf( "No such job id\n" );
        return 2;
    }
}

int BringBg( char argv[][MAX_SIZE], int argc )
{
    if( argc != 2 )
    {
        printf( "BringBg usage: \"BringBg [job_id]\" \n");
        return 1;
    }

    int job_id = atoi( argv[1] );
    char str[MAX_SIZE];
    BackPro *Ptr = ProcessList;

    for( ; Ptr != NULL; Ptr = Ptr->next )
    {
        if( Ptr->job_id == job_id )
        {
            // printf( "yes\n");
            // send this process to the background
            sprintf( str, "/proc/%d/stat", Ptr->pid );
            FILE *fp;
            if( (fp = fopen( str, "r" ) ) < 0 )
            {
                perror( "open() error");
                return 2;
            }

            char runn[MAX_SIZE];
            for( int i = 0; i < 3; i++)
            {
                fscanf( fp, "%s", runn );
            }
            
            if( runn[0] == 'S' || runn[0] == 'R') // process already running
            {
                return 0;
            }

            else
            {
                // AddProcess( Ptr->pid, Ptr->argv, Ptr->argc);
                if( kill( Ptr->pid, SIGCONT ) < 0)
                {
                    perror( "kill() failed");
                    return errno;
                }
                // printf("received it\n");
            }
            return 0;
        }
    }
}