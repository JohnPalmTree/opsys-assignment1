#define _POSIX_C_SOURCE 200809L

// Initial code for shell along with header files which maybe required for reference

# include <stdio.h> 
# include <stdlib.h>   // used to execute subprocess and commands
# include <string.h>
# include <unistd.h>   // used for exit, getcwd,read, write, exec
# include <sys/wait.h>  
# include <sys/types.h>
# include <dirent.h> // for ls
# include <errno.h>
# include <sys/stat.h>
# include <fcntl.h>  // used for open
# include <ctype.h> // used to 

// read command line for the input line in shell
char *read_line(void)
{
    return ;
}

// parse the input command
char **parse(char *my_line)
{
    
    return ;
}

int main(int argc, char** argv)
{
    printf("Welcome to Assignment 1 ! \n");

    // HINT1 : You will have to create a  while loop to take the commands

    // HINT2 : Further You will create functions to read commands, parse commands and then execude the commands.

    // HINT3 : You will 

    return 0;
}