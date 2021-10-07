#include "headers.h"

int jobs( char argv[][MAX_SIZE], int argc )
{
    int running = 1;
    int sleeping = 1;
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
                sleeping = 1;
            }
        }
    }

    if( running == 0 && sleeping == 0) // invalid args
    {
        running = 1;
        sleeping = 1;
    }

    // print all bg processes
    char str[MAX_SIZE];

    for( BackPro *Ptr = ProcessList; Ptr != NULL; Ptr = Ptr->next )
    {
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

        printf( "[%d] ", Ptr->job_id );
    
        if( strcmp( runn, "S") == 0 || strcmp(runn, "R") == 0 )
        {
            if( running == 1 )
            {
                printf( "running    ");
            }
        }
        else
        {
            if( sleeping == 1 )
            {
                printf( "sleeping   ");
            }
        }

        for( int j = 0; j < Ptr->argc ; j++ )
        {
            printf( "%s ", Ptr->argv[j] );
        }
        printf( "\n" );

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
            if( kill( Ptr->pid, sig_no ) < 0)
            {
                perror( "kill error");
                return errno;
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