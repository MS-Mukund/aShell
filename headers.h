#ifndef HEADERS_H
#define HEADERS_H

// headers
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>
#include <dirent.h>
#include <grp.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <signal.h>

#include <ctype.h>

// macros
#define MAX_SIZE 1024
#define MAX_ARGS 12
#define MAX_PROCESS 128
#define SPEC_CODE 123456
#define MAX_PIPES 12

// structs
typedef struct BackgroundProcess
{
    int job_id;
    pid_t pid;

    char **argv;
    int argc;

    struct BackgroundProcess *next;
} BackPro;

// global variables
extern char HomeDirec[MAX_SIZE];
extern char PrevDirec[MAX_SIZE];
extern int HistorySize;
extern int InpF;
extern int OutF;
extern int ProcessCount;
extern int AllProcess;
extern BackPro *ProcessList;    // points to head of LL

// function declarations
void PromptDisplay();
int ExecCommand( char argv[][MAX_SIZE], int argc);
void GetCommand();

int Echo( char argv[][MAX_SIZE], int argc );
int ListFiles( char argv[][MAX_SIZE], int argc ); 
int ChDir( char argv[][MAX_SIZE], int argc );
int PrintWorkingDir( char argv[][MAX_SIZE], int argc );
int Exit( char argv[][MAX_SIZE], int argc );
int ProcessInfo( char argv[][MAX_SIZE], int argc );

int ForegrBackgr( char argv[][MAX_SIZE], int argc );

int History( char argv[][MAX_SIZE], int argc );
int jobs( char argv[][MAX_SIZE], int argc );
int sig( char argv[][MAX_SIZE], int argc );

int BringFg( char argv[][MAX_SIZE], int argc );
int BringBg( char argv[][MAX_SIZE], int argc );

void FindAndDelProcess(pid_t pid, char str[]);
int AddProcess(pid_t pid, char **tmp, int argc);

void red ();
void yellow();
void white();
void reset ();

#endif
