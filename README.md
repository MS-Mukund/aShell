## DESCRIPTION: ##

The files where code is present headers.h, display.c, ls.c, pinfo.c, history.c, ForegroundBackground.c, cd.c, echo.c,   
exit.c and pwd.c. The file is described in README.md and a Makefile is present for creating the executable easily.  

1) **headers.h:** All global variables declared, macro definitions, function declarations and standard library header file   
   include paths are present here.  
2) **display.c:** Contains the main() function,from which we call GetCommand() and PromptDisplay(). GetCommand() takes the  
   input [ this function is also defined in display.c ] and tokenizes it. PromptDisplay(), which displays the shell   
   prompt, is also defined here.  

3) **ls.c:** The function ExecCommand(), which interprets the tokenised command and calls the required function, is defined  
   here. In addition, ListFiles() function, which implements the "ls" command is present here.  
4) **cd.c:** Contains the ChDir() function, which implements the "cd" command.  
5) **pinfo.c:** Contains the ProcessInfo() function, which implements the pinfo command.  
    
6) **ForegroundBackground.c:** Contains the ForegrBackgr(), which runs a foreground/background process, with or without  
   arguments, which have not been explicitly implemented by me, such as vi, gedit, clear, etc. In case of a background  
   process, it prints the pid of the process when it is started and the exit info. of the process when it is finished.  

7) **echo.c**: Implements the echo command.  
8) **pwd.c:** Implements the pwd command.  
9)  **exit.c** Implements the exit command.  
10) **history.c:** Implements the history command.  

## Assumptions: ##

1) The max_size of a file path or an argument of a command is 1024. 
2) If the date of modification of a file is greater than 180 days, the year of modification of the file is printed instead  
   of time.   
3) In ls -l command, the total no. of blocks allocated to a directory is printed. Each block's size is 512 bytes.  
4) The maximum number of background processes possible at a time is 7. If it is greater than that, a process is removed at  
   random.  