#include "headers.h"
/**** 
 * PromptDisplay and GetCommand functions are present.
 ****/

int Tokenise( char TempArgv[], char Tokenised[][MAX_SIZE])
{
    char *str = strtok_r(TempArgv, " \t\n", &TempArgv);
    int count = 0;
    while ( str != NULL )
    {
        // printf( "%s ", str);
        strcpy(Tokenised[count], str);
        count++;
        if( Tokenised[count][strlen(str) - 1] == ';' )
        {
            Tokenised[count][strlen(str) - 1] = '\0';
        }
        str = strtok_r(TempArgv, " \t\n", &TempArgv);
    }
    return count;
}

void NoZombie(int signum)
{
    // just return
    return;
}


int HistorySize = 0;
char HomeDirec[MAX_SIZE]; // Path to home directory

int main(void)
{
    char *ret_str = (char *)malloc(MAX_SIZE * sizeof(char));
    if( ret_str == NULL )
    {
        printf( "Not enough memory\n");
        exit(1);
    }

    PrevDirec[0] = '\0';

    ret_str = getcwd( HomeDirec, MAX_SIZE );
    if( ret_str == NULL )
    {
        perror("getcwd() failed");
        exit( errno );
    }

    while( 1 )
    {
        PromptDisplay();
        // printf("yayy\n");
        // fprintf( stderr, "yes\n");
        GetCommand();
    }
    return 0;
}

void PromptDisplay()
{
    char *usernm = (char *)malloc(MAX_SIZE * sizeof(char) );
    if( usernm == NULL )
    {
        printf( "Not enough memory\n");
        exit(1);
    }
    usernm = getlogin();    
    if( usernm == NULL )
    {
        perror( "getlogin() failed");
        _exit( errno );
    }

    char *ret_str = (char *)malloc( MAX_SIZE * sizeof(char) );
    if( ret_str == NULL )
    {
        printf( "Not enough memory\n");
        exit(1);
    }

    yellow();
    printf( "<%s", usernm );

    int ret = gethostname( usernm, MAX_SIZE );
    if( ret == -1 )
    {
        reset();
        perror( "gethostname() failed");
        _exit( errno );
    }

    yellow();
    printf( "@%s:", usernm ); // curr directory

    ret_str = getcwd( usernm, MAX_SIZE );
    if( ret_str == NULL )
    {
        reset();
        perror( "getcwd() failed");
        _exit( errno );
    }

    yellow();
    if( strlen( usernm ) >= strlen( HomeDirec ) )
    {
        if( strncmp( HomeDirec, usernm, strlen( usernm ) ) == 0 )
        {
            printf( "~" );
            printf( "%s", usernm + strlen( HomeDirec ) );
        }

        else
        {
            printf( "%s", usernm );
        }
    }

    else
    {
        printf( "%s", usernm );
    }
     printf( ">");

    reset();
}

void GetCommand()
{
    char *command = malloc(sizeof(char) * MAX_SIZE);
    fgets( command, MAX_SIZE, stdin );

    int inp_fd = dup( 0 );
    int out_fd = dup( 1 );
    
    // remove newline character
    if(  command[strlen(command) - 1] == '\n' )
    {
        command[strlen(command) - 1] = '\0';
    }

    char *args;
    args = strtok_r(command, ";", &command);
    while ( args != NULL )
    {
        char Tokenised[MAX_ARGS][MAX_SIZE];
        char *str;
        char *pipeComp = args;
        int count = 0, ret = 0;

        // handling pipes
        int PipeFd[2], PipeIn = 0, PipeOut = 1;
        int PipeCnt = 0;
        char TempArgv[MAX_PIPES + 1][MAX_SIZE];

        // tokenise on the basis of pipes
        str = strtok_r( pipeComp, "|", &pipeComp );
        for ( ; str != NULL ;  )
        {
            strcpy( TempArgv[PipeCnt], str );
            PipeCnt++;

            str = strtok_r( NULL, "|", &pipeComp );
        }
        TempArgv[PipeCnt][0] = '\0';
        printf( "%d\n", PipeCnt);

        // n processes need n-1 pipes
        for( int i = 0; i < PipeCnt -1; i++ )
        {
            if ( pipe( PipeFd ) < 0 )
            {
                perror( "Pipe() failed");
                _exit( errno );
            }

            signal( SIGCHLD, NoZombie );
            pid_t pid = fork();
            if( pid < 0)
            {
                perror( "Fork() error");
                _exit( errno );
            }

            else if( pid == 0)  // child
            {
                if( PipeFd[1] != 1)
                {
                    if( dup2( PipeFd[1], 1 ) < 0)
                    {
                        perror( "dup2() failed");
                        _exit( errno );
                    }

                    close(PipeFd[1]);
                }

                if( PipeIn != 0)
                {
                    if( dup2( PipeIn, 0 ) < 0)
                    {
                        perror( "dup2() failed");
                        _exit( errno );
                    }

                    close(PipeIn);
                }

                PipeIn = PipeFd[0]; // Input for next pipe                

                count = Tokenise( TempArgv[i], Tokenised );

                ret = ExecCommand(Tokenised, count);
                fprintf( stderr, "w %s\n", Tokenised[0]);

                dup2(inp_fd, 0);
                close(inp_fd);
                InpF = 0;

                dup2(out_fd, 1);
                close(out_fd);
                OutF = 1;
            }
            else // parent
            {
                int status, tmp = 1;
                close(PipeFd[0]);
                close(PipeFd[1]);

                while( tmp > 0 )
                {
                    tmp = wait(&status);
                }
            }

            // close the unused pipe ends
            close(PipeFd[1]);
            // close(PipeFd[0]);

        }

        if( PipeIn != 0)
        {
            if( dup2( PipeIn, 0 ) < 0 )
            {
                perror( "dup2() failed");
                _exit( errno );
            }
            close(PipeIn);
        }

        count = Tokenise( TempArgv[PipeCnt-1], Tokenised );
        for( int i = 0; i < count; i++)
        {
            printf( "%s\n", Tokenised[i]);
        }
        // fprintf( stderr, "%s o ok\n", Tokenised[0]);

        int inp_fd = dup( 0 );
        int out_fd = dup( 1 );
        
        ret = ExecCommand(Tokenised, count);
        fprintf( stderr,"o %s\n", Tokenised[0]);
        dup2(inp_fd, 0);
        InpF = 0;
        
        dup2(out_fd, 1);
        OutF = 1;

        args = strtok_r(command, ";", &command);
    }
    // fprintf( stderr, "ok\n");

    // free(command);
}

void red () {
  printf("\033[1;31m");
}

void yellow() {
  printf("\033[1;33m");
}

void white() {
    printf("\033[1;37m");
}

void reset () {
  printf("\033[0m");
}