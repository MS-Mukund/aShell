#include "headers.h"

int PrintWorkingDir( char argv[][MAX_SIZE], int argc )
{
    char *pwd = getcwd( NULL, 0 );
    printf( "%s\n", pwd ) ;
    return 0;
}