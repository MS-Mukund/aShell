#include "headers.h"

int Echo( char argv[][MAX_SIZE], int argc )   
{
    for( int i = 1; i < argc; i++ )
    {
        printf( "%s ", argv[i] );
    }
    printf( "\n" );
    return 0;
}