#include "headers.h"

int ProcessInfo( char argv[][MAX_SIZE], int argc )
{
    if( argc < 1 )
    {
        printf( "Too few arguments");
    }

    pid_t pid = -1;
    // if( argc == 1 )
    // {
    //     pid = getpid();
    // }

    // else
    // {
    //     pid = atoi( argv[1] );
    //     if( pid <= 0 )
    //     {
    //         printf( "Invalid PID\n" );
    //         return -1;
    //     }
    // }

    for( int i = 1; i < argc; i++)
    {
        if( argv[i][0] != '\0' )
        {
            if( (pid = atoi( argv[i] )) <= 0 )
            {
                printf( "Invalid PID\n" );
                return -1;
            }
            break;
        }
    }
    // fprintf( stderr, "ok\n");

    if( pid < 0 )
    {
        pid = getpid();
    }


    int status = kill( pid, 0 );

    if( errno == ESRCH ) // No such process
    {
        printf( "No such process\n" );
        return -1;
    }

    printf( "pid -- %d\n", pid );

    char str[MAX_SIZE];
    // printf( "yes\n");
    sprintf( str, "/proc/%d/stat", pid );
    // printf( "ok\n");
    FILE *fp = fopen( str, "r" );
    // printf( "no\n");
    if( fp < 0 )
    {
        perror( "open()");
        return errno;
    }

    char buffer[25][MAX_SIZE];
    // printf( "yes\n");
    int i = 0;
    while( !feof( fp ) && i < 25 )
    {
        fscanf( fp, "%s", buffer[i] );
        i++;
    }

    printf( "Process Status -- %s", buffer[2] ); 
    if( !strcmp(buffer[0], buffer[7]) ) // check if process is running in the foreground
    {
        printf( "+");
    }
    printf( "\n" );

    // memory usage
    printf( "memory -- %lld {Virtual Memory}\n", atoll(buffer[22]) );
    fclose( fp );

    sprintf( str, "/proc/%d/exe", pid );
    // printf( "no\n");
    for( int i = 0; i < MAX_SIZE; i++ )
    {
        buffer[0][i] = '\0';
    }

    if( readlink( str, buffer[0], MAX_SIZE - 1 ) < 0 )
    {
        perror( "readlink()");
        return errno;
    }

    printf( "Executable Path -- " );
    if( strlen( buffer[0] ) >= strlen( HomeDirec ) )
    {
        // printf( "yes\n");
        if( strncmp( HomeDirec, buffer[0], strlen( HomeDirec ) ) == 0 )
        {
            // printf( "yes\n");
            printf( "~" );
            printf( "%s\n", buffer[0] + strlen( HomeDirec ) );
        }

        else
        {
            printf( "%s\n", buffer[0] );
        }
    }

    else
    {
        printf( "%s\n", buffer[0] );
    }

    // fprintf( stderr, "no\n");
    return 0;
}