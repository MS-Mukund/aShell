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

        fclose( fp );
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
    // printf( "%d\n", sig_no);

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
                if( kill( Ptr->pid, SIGTSTP ) < 0)
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

            fclose( fp );
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

            char runn[3][MAX_SIZE];
            for( int i = 0; i < 3; i++)
            {
                fscanf( fp, "%s", runn[i] );
            }

            if( kill( Ptr->pid, SIGCONT ) < 0)
            {
                perror( "kill() failed");
                return errno;
            }  
            char str[MAX_SIZE];

            // wait for the process to finish
            pid_t Pr_pid = Ptr->pid;
            FgId = Pr_pid;
            int status;
            int ret;
            
            // printf( "%d\n", Ptr->pid);
            AddProcess( Pr_pid, Ptr->argv, Ptr->argc, 0 );
            
            FindAndDelProcess( Pr_pid, str, 1 );
            // printf( "%d\n", FgId);

            if( ( ret = waitpid( Pr_pid, &status, WUNTRACED ) ) == -1 )
            {
                perror( "waitpid() error");
                return errno;
            }

            fclose( fp );
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

            // printf( "hello %c\n", runn[0]);
            if( kill( Ptr->pid, SIGCONT ) < 0)
            {
                perror( "kill() failed");
                return errno;
            }                       
            // printf( "bye\n");

            fclose(fp);
            return 0;
        }
    }
}