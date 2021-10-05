#include "headers.h"
/**** 
 * PromptDisplay and GetCommand functions are present.
 ****/

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
        char *components = args, *str;
        int count = 0;
        
        str = strtok_r(components, " \t\n", &components);
        while ( str != NULL )
        {
            // printf( "%s ", str);
            strcpy(Tokenised[count], str);
            count++;

            if( Tokenised[count][strlen(str) - 1] == ';' )
            {
                Tokenised[count][strlen(str) - 1] = '\0';
            }

            str = strtok_r(components, " \t\n", &components);
        }

        int inp_fd = dup( 0 );
        int out_fd = dup( 1 );

        int ret = ExecCommand(Tokenised, count);
        
        dup2(inp_fd, 0);
        InpF = 0;

        dup2(out_fd, 1);
        OutF = 1;

        args = strtok_r(command, ";", &command);
    }

    // free(command);
}