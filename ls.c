#include "headers.h"
int InpF = 0;
int OutF = 1;

int ExecCommand( char argv[][MAX_SIZE], int argc )
{
    
   // printf( "%s%d\n", argv[0], argc );
   if( argc < 1)
   {
       printf("No command given\n");
       return -1;
   }

    // handling input-output redirection. 
   char NullStr[MAX_SIZE] = "";
   for( int i = 0; i < argc; i++ )
   {
        int Lett = argv[i][0] - '0';

        switch ( Lett )
        {
            case ('<' - '0'):
                if( i == argc - 1)
                {
                    fprintf( stderr, "No file name given\n" );
                    return -1;
                }
                InpF = 0;
                if( (InpF = open( argv[i+1], O_RDONLY ) ) < 0 )
                {
                    perror( "open()");
                    return errno;
                }
                
                if( dup2( InpF, STDIN_FILENO ) < 0 )
                {
                    perror( "dup2()");
                    return errno;
                }
                close(InpF);
                strcpy( argv[i], NullStr );
                strcpy( argv[i+1], NullStr );
                // argv[i][0] = '\0';
                // argv[i+1][0] = '\0';
                break;
            
            case ('>' - '0'):
                if( i == argc - 1)
                {
                    fprintf( stderr, "No file name given\n" );
                    return -1;
                }
                OutF = 1;
                if( argv[i][1] == '>' )     // append
                {
                    if( (OutF = open( argv[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644 ) ) < 0 )
                    {
                        perror( "open()");
                        return errno;
                    }
                }

                else                        //overwrite
                {
                    if( (OutF = open( argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644 ) ) < 0 )
                    {
                        perror( "open()");
                        return errno;
                    }
                }
    
                if( dup2( OutF, STDOUT_FILENO ) < 0 )
                {
                    perror( "dup2()");
                    return errno;
                }
                close(OutF);
                strcpy( argv[i], NullStr );
                strcpy( argv[i+1], NullStr );
                break;
        }
   }   

   if( strcmp(argv[0], "ls") == 0 )
   {
        return ListFiles( argv, argc );
   }

    else if( strcmp(argv[0], "cd") == 0 )
    {
        return ChDir( argv, argc );
    }

    else if( strcmp(argv[0], "echo") == 0 )
    {
        return Echo( argv, argc );
    }

    else if( strcmp(argv[0], "pwd") == 0 )
    {
        return PrintWorkingDir( argv, argc );
    }

    else if( strcmp(argv[0], "pinfo") == 0 )
    {
        return ProcessInfo( argv, argc );
    }

    else if( strcmp(argv[0], "repeat") == 0 )
    {
        if( argc <= 2 )
        {
            printf("Insufficient no. of arguments\n");
            return -1;
        }

        else
        {
            for( int i = 0; i < strlen(argv[1]); i++ )
            {
                if( !isdigit(argv[1][i]) )
                {
                    printf("Invalid no of repititions\n");
                    return -1;
                }

                // printf( "%c  ", argv[1][i] );
            }

            int Num = atoi(argv[1]);
            if( Num < 0 )
            {
                printf("Invalid no of repititions\n");
            }


            for( int i = 2; i < argc; i++ )
            {
                strcpy(argv[i-2], argv[i]);
            }
            argc -= 2;

            for( int i = 0; i < Num; i++ )
            {
                ExecCommand( argv, argc );
            }
        }
    }

    else if( strcmp(argv[0], "history") == 0 )
    {
        return History( argv, argc );
    }

    else if( strcmp(argv[0], "exit") == 0 )
    {
        // return Exit( argv, argc );
        exit( EXIT_SUCCESS );
    }

    else 
    {
        // printf( "yes\n");
        return ForegrBackgr( argv, argc );
    }

   return 0;   
}

int ListFiles( char argv[][MAX_SIZE], int argc )
{

    int IsA = 0, IsL = 0;

    char *dirName[15] = {NULL};
    int DirIndex = 0;
    for( int i = 1; i < argc; i++ )
    {
        if( argv[i][0] == '\0' )
        {
            continue;
        }

        if( argv[i][0] == '-')  // flags
        {
            if( strcmp( argv[i], "-l") == 0 )
            {
                IsL = 1;
            }

            else if( strcmp( argv[i], "-a") == 0 )
            {
                IsA = 1;
            }

            else if( strcmp( argv[i], "-la") == 0 || strcmp( argv[i], "-al") == 0 )
            {
                IsA = 1;
                IsL = 1;
            }

            else
            {
                dirName[DirIndex] = argv[i];
                DirIndex++;
            }
        }

        else
        {
            dirName[DirIndex] = argv[i];
            DirIndex++;

            if( strcmp( argv[i], "~") == 0 )
            {
                dirName[DirIndex-1] = HomeDirec;
            }
        }
    }

    DIR *dir;
    struct dirent *ent;
    if( DirIndex == 0 )
    {
        dirName[0] = ".";
        DirIndex = 1;
    }

    for(int ind = 0; ind < DirIndex; ind++ )
    {

        if( (dir = opendir(dirName[ind])) == NULL )
        {
            printf("error: Could not open directory %s\n", dirName[ind]);
            return 1;
        }

        if( IsL == 0 )
        {
            while( (ent = readdir(dir)) != NULL )
            {
                if( IsA == 0 )
                {
                    if( ent->d_name[0] != '.' )
                    {
                        printf("%s ", ent->d_name);
                    }
                }
                else
                {
                    printf("%s ", ent->d_name);
                }
            }
            printf( "\n");
            continue;
        }

        struct stat st = {0};           
        blkcnt_t block = 0;
        
        while( (ent = readdir(dir)) != NULL )
        {
             if( IsA == 1 || ent->d_name[0] != '.' ) 
            {
                stat( ent->d_name, &st );
                stat( ent->d_name, &st );
                block += st.st_blocks; 
            }
        }
        printf( "total %ld\n", block );
        
        if( (dir = opendir( dirName[ind] ) ) == NULL )
        {
            printf("error: Could not open directory %s\n", dirName[ind]);
            return 1;
        }

        while( (ent = readdir(dir)) != NULL )
        {
            if( IsA == 1 || ent->d_name[0] != '.' ) 
            {
                stat( ent->d_name, &st );
                
                //////////////////////////////////////////////////////////////
                if( S_ISDIR(st.st_mode) )
                    printf( "d" );
                else
                    printf( "-" );
                if( st.st_mode & S_IRUSR )       // user has read permission
                    printf( "r" );
                else
                    printf( "-" );
                if( st.st_mode & S_IWUSR )       // user has write permission
                    printf( "w" );
                else
                    printf( "-" );
                if( st.st_mode & S_IXUSR )       // user has execute permission
                    printf( "x" );
                else
                    printf( "-" );
                if( st.st_mode & S_IRGRP )       // group has read permission
                    printf( "r" );
                else
                    printf( "-" );
                if( st.st_mode & S_IWGRP )       // group has write permission
                    printf( "w" );
                else
                    printf( "-" );
                if( st.st_mode & S_IXGRP )       // group has execute permission
                    printf( "x" );
                else
                    printf( "-" );
                if( st.st_mode & S_IROTH )       // others has read permission
                    printf( "r" );
                else
                    printf( "-" );
                if( st.st_mode & S_IWOTH )      // others has write permission
                    printf( "w" );
                else
                    printf( "-" );
                if( st.st_mode & S_IXOTH )      // others has execute permission
                   printf( "x " );
                else
                    printf( "- " );
                //////////////////////////////////////////////////////////////

                printf( "%lu ", st.st_nlink ); // number of hard links

                struct passwd *pw = getpwuid( st.st_uid );
                if( pw == NULL )
                {
                    printf( "Error fetching ownername: aborting\n" );
                    return 1;
                }

                struct group *grp = getgrgid( st.st_gid );
                if( grp == NULL )
                {
                    printf( "Error fetching groupname: aborting\n" );
                    return 1;
                }

                printf( "%s ", pw->pw_name );
                printf( "%s ", grp->gr_name );

                // file size
                printf( "%10lu ", st.st_size );

                // last modified time
                struct tm *ModifTm = localtime( &st.st_mtime );
                if( ModifTm == NULL )
                {
                    printf( "Error fetching modification time: aborting\n" );
                    return 1;
                }

                char Month[15];
                strftime( Month, sizeof(Month), "%b", ModifTm );
                printf( "%s %2d ", Month, ModifTm->tm_mday );

                long long ModifTime = st.st_mtime;
                long long CurrTime = time( NULL );

                if( CurrTime - ModifTime >= 15552000 ) // more than 180 days
                {
                    printf( "%4d", 1990 + ModifTm->tm_year );
                }

                else
                {
                    printf( "%02d:%02d ", ModifTm->tm_hour, ModifTm->tm_min );
                }

                printf( "%s\n", ent->d_name );

            }

        }

        closedir( dir );
    }

    return 0;
}