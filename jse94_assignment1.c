#define _POSIX_C_SOURCE 200809L

/*
to-do:
    - decide if switching methods from "int" to "void" and considering operating procedures
*/

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
int cmd_cat(int arg, char**argv); // concatenate (print) files
int cmd_pwd(int arg, char**argv); // print working directory
int cmd_cd(int arg, char**argv); // change directory
int cmd_mkdir(int arg, char**argv); // make directory
int cmd_rmdir(int arg, char**argv); // remove directory
int cmd_echo(int arg, char**argv); // echoes the passed arguments back.
int cmd_touch(int arg, char**argv); // updates the target file's "last modified" time w/o changing contents. creates an empty file if it doesn't exist, too.

// structure for commands - the name that the user user types, and function to run it.
struct cmd {
    const char *name;
    int(*f)(int, char **);
};

struct cmd cmds[] = {
    {"cat", cmd_cat},
    {"pwd", cmd_pwd},
    {"cd", cmd_pwd},
    {"mkdir", cmd_mkdir},
    {"rmdir", cmd_rmdir},
    {"echo", cmd_echo},
    {"touch", cmd_touch},
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

// cmd_pwd: prints the current working directory.
int cmd_pwd(int arg, char**argv) {
    char buf[4096]; // space on the stack for file path, set at linux max
    if (getcwd(buf, sizeof buf) == NULL) {      // if the current directory is null, print an error and return.
        perror("pwd not found...");
        return 1;
    }

    printf("%s\n", buf); // print the path
    return 0;   // success
}

// cmd_cd: changes the current directory
int cmd_cd(int arg, char**argv) {
    const char *path;
    if (arg < 2) path = getenv("HOME"); // if the user just types cd, set the dir to home
    else         path = argv[1];

    if (path == NULL) {     // if the path is null, print an error
        fprintf(stderr, "cd: HOME is not currently set");
        return 1;
    }
    if (chdir(path) != 0) { // system call, change directory to the path
        perror("cd");
        return 1;
    }

    return 0;
}

// cmd_mkdir: makes a new directory
int cmd_mkdir(int arg, char**argv) {
    if (arg < 2) {
        fprintf(stderr, "cannot make directory.."); // if nothing was passed, return 1 + print error.
        return 1;
    }

    int status = 0; // return variable
    for (int i = 1; i < arg; i++) {
        if (mkdir(argv[i], 0755) != 0) {    // if the creation failed,
            perror(argv[i]);
            status = 1;                     // ..change return variable but keep looping
        }
    }

    return status;
}

// cmd_rmdir: removes a directory
int cmd_rmdir(int arg, char**argv) {
    if (arg < 2) {
        fprintf(stderr, "unable to remove directory..."); // if nothing was passed, return 1 + print error.
        return 1;
    }

    int status = 0;
    for (int i = 1; i < arg; i++) {     // for every argument
        if (rmdir(argv[i]) != 0) {      // if removing the directory fails,
            perror(argv[i]);            // ..change return variable but keep looping
            status = 1;
        }
    }

    return status;
}

// cmd_echo: echoes (prints) back the passed arguments.
int cmd_echo(int arg, char**argv) {
    for (int i = 1; i < arg; i++) { // for every argument,
        printf("%s", argv[i]);          // print it, and use string handling

        if (i < arg - 1) {              // add a space between the words, but not after the last,
            printf(" ");
        }
    }
    printf("\n");                       // ..because the last has a new line afterwards.
    return 0;
}

// cmd_touch: updates file's timestamp + creates a new one if missing.
int cmd_touch(int arg, char**argv) {
    if (arg < 2) {
        fprintf(stderr, "cannot touch file...");
        return 1;
    }

    int status = 0;

    for (int i = 1; i < arg; i++) {
        int file = open(argv[i], O_CREAT | O_WRONLY, 0644); // open(target file, [create/write only])

        if (file < 0) {         // if the file wasn't able to be opened,
            perror(argv[i]);    // ..print an error and keep looping
            status = 1;
            continue;
        }

        if (futimens(file, NULL) != 0) {    // if you cannot set the file's timestamp,
            perror(argv[i]);                // ..print an error and keep looping 
            status = 1;
        }
        close(file);                        // close file, return status.
    }

    return status;
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

    return 0;
}