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
# include <ctype.h> // (added) used for isspace.

// built-in command declarations
int cmd_cat(int arg, char**argv); // concatenate (print) files
int cmd_pwd(int arg, char**argv); // print working directory
int cmd_cd(int arg, char**argv); // change directory
int cmd_mkdir(int arg, char**argv); // make directory
int cmd_rmdir(int arg, char**argv); // remove directory
int cmd_echo(int arg, char**argv); // echoes the passed arguments back.
int cmd_touch(int arg, char**argv); // updates the target file's "last modified" time w/o changing contents. creates an empty file if it doesn't exist, too.
int cmd_wc(int arg, char**argv); // prints word + line + bytes of file(s).
int cmd_head(int arg, char**argv); // prints the first "n" line(s) of a file.
int cmd_grep(int arg, char**argv); // prints every line of a file that contains a pattern.
int cmd_ls(int arg, char**argv); // lists folders and files in a dir.
int cmd_history(int arg, char**argv); // prints the history of previous commands, up to 10.
int cmd_exit(int arg, char**argv); // exits.

// structure for commands - the name that the user user types, and function to run it.
struct cmd {
    const char *name;
    int(*f)(int, char **);
};

#define NUM_CMDS 13

// array of commands.
struct cmd cmds[] = {
    {"cat", cmd_cat},
    {"pwd", cmd_pwd},
    {"cd", cmd_cd},
    {"mkdir", cmd_mkdir},
    {"rmdir", cmd_rmdir},
    {"echo", cmd_echo},
    {"touch", cmd_touch},
    {"wc", cmd_wc},
    {"head", cmd_head},
    {"grep", cmd_grep},
    {"ls", cmd_ls},
    {"history", cmd_history},
    {"exit", cmd_exit},
};

#define HIST_SIZE 10            // history max size
static char *hist[HIST_SIZE];   // 10 saved cmd strings
static int hist_count = 0;      // cmds entered

// clears up the history buffer.
void free_history(void)
{
    for (int i = 0; i < HIST_SIZE; i++) {
        free(hist[i]);
        hist[i] = NULL;
    }
}

// saves a copy of line into the history buffer. it overwrites oldest entry once full.
void add_history(const char*line) {
    int slot = hist_count % HIST_SIZE;
    free(hist[slot]); // free up cmd in slot
    hist[slot] = strdup(line); // store copy of new one
    hist_count++;
}

// counts arguments in an array
int count_args(char **args) {
    int n = 0;
    while (args[n] != NULL) {
        n++;
    }
    return n;
}

// returns 1 if line is empty/spaces, 0 otherwise.
int is_blank(const char *line) {
    for (int i = 0; line[i] != '\0'; i++) {
        if (!isspace((unsigned char)line[i])) {
            return 0;
        }
    }

    return 1;
}

// looks up argument in built-in table to the matching fct.
int execute(int arg, char**argv) {
    for (size_t i = 0; i < NUM_CMDS; i++) {
        if (strcmp(argv[0], cmds[i].name) == 0) {
            return cmds[i].f(arg, argv);
        }
    }

    fprintf(stderr, "myshell: %s: cmd not found.\n", argv[0]);
    return 1;
}

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

// cmd_wc: prints the line, word, and byte counts for every file.
int cmd_wc(int arg, char**argv) {
    if (arg < 2) {                                              // if the arguments are empty,
        fprintf(stderr, "unable to obtain word count...");      // print an error.
        return 1;
    }

    int status = 0;

    for (int i = 1; i < arg; i++) {                             // for every argument,
        FILE *f = fopen(argv[i], "r");                              // create a file for it, and read it.
        if (f == NULL) {                                            // if the file is null, throw an error and continue.
            perror(argv[i]);
            status = 1;
            continue;
        }

        long lines = 0;                                         // create variables for lines, words, and bytes.
        long words = 0;
        long bytes = 0;
        int in_word = 0;
        
        int count;

        while ((count = fgetc(f)) != EOF) {                     // while it isn't the end of the file,
            bytes++;                                                // increment the byte variable.

            if (count == '\n') lines++;                             // if there is a linebreak, increment lines variable.
            if (isspace(count)) {                                   // if there is a space, reset the "in_word" count (which keeps track of letter count in-words)
                in_word = 0;
            } else if (!in_word) {                                  // otherwise, add to words variable.
                in_word = 1;
                words++;
            }
        }

        fclose(f);
        printf("%ld %ld %ld %s\n", lines, words, bytes, argv[i]);   // output lines, words, bytes, filename.
    }

    return status;
}

// cmd_head: prints the first "n" lines of each file. in this case, n = 10.
int cmd_head(int arg, char**argv) {
    int n = 10;         // our default line-count
    int start = 1;      // where filenames begin

    if (arg >= 3 && strcmp(argv[1], "-n") == 0) {   // as long as the first 2 arguments exist before looping,
        n = atoi(argv[2]);
        start = 3;                                  // start at the 3rd index to read properly.
    }

    if (start >= arg) {     // if the filename isn't given, print error and return.
        fprintf(stderr, "unable to print the header of the file...");
        return 1;
    }

    int status = 0;
    for (int i = start; i < arg; i++) {     // for every argument, open a file.
        FILE *f = fopen(argv[i], "r");

        if (f == NULL) {                        // if the file is null, print an error.
            perror(argv[i]);
            status = 1;
            continue;
        }

        char buf[4096];
        int count = 0;
        while (count < n && fgets(buf, sizeof buf, f) != NULL) {    // while the count isn't hitting its limit, and the line read isn't null,
            fputs(buf, stdout);                                         // print the line as read and increment "count".
            count++;
        }
        fclose(f);                                                  // close file. return status.
    }

    return status;
}

// cmd_grep: prints every line of a file that contains a pattern.
int cmd_grep(int arg, char**argv) {
    if (arg < 3) {
        fprintf(stderr, "cannot return the pattern or file...");
        return 1;
    }

    const char *pattern = argv[1]; // pattern passed.
    int status = 0;

    for (int i = 2; i < arg; i++) {     // loop thru arguments, open file, if null throw error.
        FILE *f = fopen(argv[i], "r");

        if (f == NULL) {
            perror(argv[i]);
            status = 1;
            continue;
        }

        char buf[4096];
        while (fgets(buf, sizeof buf, f) != NULL) {
            if(strstr(buf, pattern) != NULL) {      // if the pattern matches..
                if (arg > 3) {                          // ..and if the argument count is > 3..
                    printf("%s:", argv[i]);                 // ..print the line.
                }
                fputs(buf, stdout);
            }
        }
        fclose(f);
    }

    return status;
}

// cmd_ls: lists all of the non-hidden entries of a directory
int cmd_ls(int arg, char**argv) {
    const char *path;
    
    if (arg > 1) {
        path = argv[1]; // if user gave folder, ls the folder
    } else {
        path = ".";     // no folder given, use current
    }

    DIR *d = opendir(path);
    if (d == NULL) {
        perror(path);
        return 1;
    }

    struct dirent *entry;       // read first entry
    entry = readdir(d);

    while (entry != NULL) {  // 
        char *name = (*entry).d_name;   // entry's file/folder name
        if (name[0] != '.') {           // skip "." or ".." or etc.
            printf("%s\n", name);           // print
        }
        entry = readdir(d);             // next entry
    }

    closedir(d);    // close directory
    return 0;
}

// cmd_history: prints the last 10 commands entered, w/ oldest first.
int cmd_history(int arg, char**argv) {
    int start;
    if (hist_count > HIST_SIZE) {
        start = hist_count - HIST_SIZE; // more than 10 entered, skip old ones.
    } else {
        start = 0;
    };

    for (int i = start; i < hist_count; i++) {
        int slot = i % HIST_SIZE;
        printf("%4d %s\n", i + 1, hist[slot]);
        
    }
    
    return 0;
}

int cmd_exit(int arg, char**argv) {
    free_history(); // free the history, and exit.
    exit(0);
}
// read command line for the input line in shell
char *read_line(void)
{
    char *line = NULL;
    size_t cap = 0;

    ssize_t length = getline(&line, &cap, stdin); // length of line.

    if (length == -1) {
        free(line);
        free_history();
        printf("\n");
        exit(0);
    }

    if (length > 0 && line[length - 1] == '\n') {
        line[length - 1] = '\0';
    }

    return line;
}

// parse the input command
char **parse(char *my_line)
{
    int cap = 8;
    int n = 0;
    char **args = malloc(cap * sizeof(char*));

    if (args == NULL) {
        perror("memory allocation error..."); // if no arguments..
        exit(1);
    }

    char *token = strtok(my_line, " \t"); // tokenize line
    
    while (token != NULL) { // while the token isn't null, loop through it and keep tokenizing.
        if (n + 1 >= cap) {
            cap = cap * 2;
            char **larger = realloc(args, cap * sizeof(char *));

            if (larger == NULL) {
                perror("reallocation error...");
                free(args);
                exit(1);
            }
            args = larger;
        }
        args[n] = token;
        n++;
        token = strtok(NULL, " \t");
    }

    args[n] = NULL;

    return args;
}

int main(int argc, char** argv)
{
    printf("Welcome to Assignment 1 ! \n");

    while (1) {
        printf("myshell> ");
        fflush(stdout);

        char *line = read_line(); // reads lines

        if (is_blank(line)) { // if the line is blank, free and continue
            free(line);
            continue;
        }

        add_history(line); // add line to history

        char **args = parse(line); // parse arguments, store the number of them. execute command.
        int numargs = count_args(args);
        execute(numargs, args);
        
        free(args);
        free(line);
    }

    return 0;
}