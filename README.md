# DESCRIPTION: 

## Starting the shell:

Type the following commands to start the shell:
1) make
2) ./aShell

To exit, type "exit" on the terminal.  

## File Contents:

1) **headers.h:** All global variables declarations, macro definitions, function declarations and standard library header file   
   include paths are present here.  
2) **display.c:** Contains the main() function,from which we call GetCommand() and PromptDisplay(). GetCommand() takes the  
   input [ this function is also defined in display.c ] and tokenizes it based on semi-colons, spaces, tabs and pipes. PromptDisplay(),
   which displays the shell prompt, is also defined here.  ControlZ() and ControlC(), which are signal handler functions when Ctrl+Z  
   or Ctrl+C is pressed, are also defined here.   

3) **ls.c:** The function ExecCommand(), which interprets the tokenised command and calls the required function (also handles IO  
   redirection), is defined here. In addition, ListFiles() function, which implements the "ls" command is present here.  
4) **cd.c:** Contains the ChDir() function, which implements the "cd" command.  
5) **pinfo.c:** Contains the ProcessInfo() function, which implements the pinfo command.  
    
6) **ForegroundBackground.c:** Contains the ForegrBackgr(), which runs a foreground/background process, with or without  
   arguments, which have not been explicitly implemented by me, such as vi, gedit, clear, etc. Also contains some other helper functions 
   which maintain the list of background processes.  
7)  **echo.c**: Implements the echo command.  
8) **pwd.c:** Implements the pwd command.  
9)  **exit.c** Implements the exit command.  
10) **history.c:** Implements the history command.  
    
11) **jobs.c:** Implements the user-defined commands jobs, sig, fg and bg.

## Assumptions: ##

1) The max_size of a file path or an argument of a command is 1024. 
2) If the date of modification of a file is greater than 180 days, the year of modification of the file is printed instead  
   of time.   
3) In ls -l command, the total no. of blocks allocated to a directory is printed. Each block's size is 512 bytes.  
4) The maximum number of background processes possible at a time is 128. If it is greater than that, a process is killed at  
   random.  

5) As given in the specifications, I have assumed that the sig() function   signal is sent to a running job only


// In case of a background  
   process, it prints the pid of the process when it is started and the exit info. of the process when it is finished.  
