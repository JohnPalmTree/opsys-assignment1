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

// built-in command declarations
int cmd_cat(int arg, char**argv);

// name for the command table - name user types, and function to run it
struct cmd {
    const char *name;
    int(*f)(int, char **);
};

struct cmd cmds[] = {
    {"cat", cmd_cat},

};

// cmd_cat: prints the contents of the file given.
int cmd_cat(int arg, char**argv) {
    if (arg < 2) {  // no file name, return
        fprintf(stderr, "cannot cat file...");
        return 1;
    }

    for (int i = 1; i < arg; i++) {     // skip 'cat' and handle all files
        FILE *f = fopen(argv[i], "r");  // open file to read
        if (f == NULL) {                    // return if unable to open file
            perror(argv[i]);
            continue;
        }
        int c;  // use an int, not char, because we want to return a byte or EOF (-1)
        while ((c = fgetc(f)) != EOF) {     // if it isn't EOF, print it and repeat.
            putchar(c);
        }
        fclose(f);  // close the file.
    }
    return 0; // success!
}

// read command line for the input line in shell
/*char *read_line(void)
{
    return ;
}

// parse the input command
char **parse(char *my_line)
{
    
    return ;
}*/

int main(int argc, char** argv)
{
    printf("Welcome to Assignment 1 ! \n");

    // HINT1 : You will have to create a  while loop to take the commands

    // HINT2 : Further You will create functions to read commands, parse commands and then execude the commands.

    // HINT3 : You will 

    // temp test - delete
    char *test[] = {"cat", "Makefile", "nope.txt", NULL};
    cmd_cat(3, test);

    return 0;
}