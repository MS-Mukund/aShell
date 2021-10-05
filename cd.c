#include "headers.h"
char PrevDirec[1024];

int ChDir( char argv[][MAX_SIZE], int argc )
{
    char *ret_str = getcwd( NULL, 0 );
    if( ret_str == NULL )
    {
        printf( "cd: getcwd() error\n" );
    }

    int ret = 0;
    if( argc == 1 || ( argc == 2 && strcmp(argv[1], "~") == 0 ) )
    {
        // change to directory in which shell is present
        ret = chdir( HomeDirec );
        if( ret == -1 )
        {
            printf( "cd: %s: No such file or directory\n", HomeDirec );
            return 1;
        }
        // printf( "HomeDirec %s", HomeDirec);
        return 0;
    }

    else if( argv[1][0] == '-' )
    {
        // printf( "yes %s %d\n", PrevDirec, PrevDirec[0]);
        if( PrevDirec[0] != '\0' )
        {
            ret = chdir( PrevDirec );
            if( ret == -1 )
            {
                printf( "cd: %s: No such file or directory\n", PrevDirec );
                return 1;
            }
        }
    }

    else
    {
        ret = chdir( argv[1] );
        if( ret == -1 )
        {
            printf( "cd: %s: No such file or directory\n", argv[1] );
            return 1;
        }
    }

    strcpy( PrevDirec, ret_str );
    
    return 0;
}