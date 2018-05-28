#define _POSIX_SOURCE

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define INPUT_STRING_SIZE 80
#define CNORMAL "\x1B[0m"
#define CYELLOW "\x1B[33m"

#include "io.h"
#include "parse.h"
#include "process.h"
#include "shell.h"

int cmd_help(tok_t arg[]);
int cmd_quit(tok_t arg[]);
<<<<<<< HEAD
int cmd_pwd(tok_t arg[]);
int cmd_cd(tok_t args[]);
int cmd_wait(tok_t args[]);
int cmd_fg(tok_t args[]);

char currdir[1024];
char home_env[1024];
char path_env[1024];

pid_t pid;
int latest;

int status;

tok_t *paths;
=======
>>>>>>> 565928db65f0bc6190fc7770d311b368d060c960

/** 
 *  Built-In Command Lookup Table Structures 
 */
typedef int cmd_fun_t (tok_t args[]); // cmd functions take token array and return int
typedef struct fun_desc {
    cmd_fun_t *fun;
    char *cmd;
    char *doc;
} fun_desc_t;
/** TODO: add more commands (pwd etc.) to this lookup table! */
fun_desc_t cmd_table[] = {
    {cmd_help, "help", "show this help menu"},
    {cmd_quit, "quit", "quit the command shell"},
<<<<<<< HEAD
    {cmd_pwd, "pwd", "prints current working directory"},
    {cmd_cd, "cd", "changes directory to specified directory"},
    {cmd_wait, "wait", "waits for background processes to terminate"},
    {cmd_fg, "fg", "puts processes in the foreground"}
=======
>>>>>>> 565928db65f0bc6190fc7770d311b368d060c960
};

/**
 *  Determine whether cmd is a built-in shell command
 *
 */
int lookup(char cmd[]) {
    unsigned int i;
    for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
        if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0)) return i;
    }
    return -1;
}

/**
 *  Print the help table (cmd_table) for built-in shell commands
 *
 */
int cmd_help(tok_t arg[]) {
    unsigned int i;
    for (i = 0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
        printf("%s - %s\n",cmd_table[i].cmd, cmd_table[i].doc);
    }
    return 1;
}

/**
 *  Quit the terminal
 *
 */
int cmd_quit(tok_t arg[]) {
    printf("Bye\n");
    exit(0);
    return 1;
}

<<<<<<< HEAD
int cmd_pwd(tok_t args[]){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    fprintf(stdout, "%s\n", cwd);
    return 1;
}

int cmd_cd(tok_t args[]){
    char path[1024];

    if ((args[0][0] != '~') && (args[0][0] != '-')){
        getcwd(currdir, sizeof(currdir));
        strcpy(path, args[0]);
        if (chdir(path)!= 0) {fprintf(stdout, "Not a valid directory\n");};
    } else if (args[0][0] == '~'){
        getcwd(currdir, sizeof(currdir));
        strncpy(path, args[0]+1, strlen(args[0])); 
        strcpy(path, strcat(home_env, path));
        chdir(path);
    } else if (args[0][0] == '-'){
        chdir(currdir);
    }

    return 1;
}

int cmd_wait(tok_t args[]){
    int wpid;

    while ((wpid = wait(NULL) > 0));
    fprintf(stdout, "All background jobs have completed.\n");

    return 1;
}

int cmd_fg(tok_t args[]){
    int l = 0;
    while (args[l] != NULL) {l++;}

    if (l==1){
        kill((atoi(args[0])),SIGCONT);
    } else {
        kill(latest, SIGCONT);
    }
    wait(&status);
    return 1;
}

void doNothing(int signum){}

=======
>>>>>>> 565928db65f0bc6190fc7770d311b368d060c960
/**
 *  Initialise the shell
 *
 */
void init_shell() {
    // Check if we are running interactively
    shell_terminal = STDIN_FILENO;

    // Note that we cannot take control of the terminal if the shell is not interactive
    shell_is_interactive = isatty(shell_terminal);

    if( shell_is_interactive ) {

        // force into foreground
        while(tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp()))
            kill( - shell_pgid, SIGTTIN);

        shell_pgid = getpid();
        // Put shell in its own process group
        if(setpgid(shell_pgid, shell_pgid) < 0){
            perror("Couldn't put the shell in its own process group");
            exit(1);
        }

        // Take control of the terminal
        tcsetpgrp(shell_terminal, shell_pgid);
        tcgetattr(shell_terminal, &shell_tmodes);
    }

    /** TODO */
    // ignore signals
<<<<<<< HEAD
    signal(SIGINT, doNothing);
    signal(SIGTSTP, doNothing);
    signal(SIGTTIN, doNothing);
    signal(SIGTTOU, doNothing);
    signal(SIGCONT, doNothing);
=======
>>>>>>> 565928db65f0bc6190fc7770d311b368d060c960
}

/**
 * Add a process to our process list
 *
 */
void add_process(process* p)
{
    /** TODO **/
}

/**
 * Creates a process given the tokens parsed from stdin
 *
 */
<<<<<<< HEAD
process* create_process(tok_t* tokens, bool bg)
{
    int l = 0; 
    int tl=0;
    while (paths[l] != NULL) {l++;} //get length of paths
    while (tokens[tl] != NULL) {tl++;}

    char path_arr[l][1024];

    for (int i = 0; i < l; ++i) {
        strcpy(path_arr[i], paths[i]);
        sprintf(path_arr[i] ,"%s%s%s", path_arr[i],"/",tokens[0]);
    }
  
    if ((pid = fork()) == 0){

        latest = getpid();
        fprintf(stdout, "[%d]\n", latest);
        for (int i = 0; i < l; i++){
            if ((execv(path_arr[i], tokens) == -1) && (i == l-1)) { 
                if (execv(tokens[0], tokens) == -1) {
                    fprintf(stdout, "Could not execute '%s': No such file or directory\n", tokens[0]);
                }
            }
        }
        _exit(0);
        kill(pid,SIGINT);
        kill(pid,SIGTSTP);
    } 
    else{
        if (!bg) {
            wait(&status);
        }
    }

=======
process* create_process(tok_t* tokens)
{
    /** TODO **/
>>>>>>> 565928db65f0bc6190fc7770d311b368d060c960
    return NULL;
}


/**
 * Main shell loop
 *
 */
int shell (int argc, char *argv[]) {
    int lineNum = 0;
    pid_t pid = getpid();	// get current process's PID
    pid_t ppid = getppid();	// get parent's PID
    pid_t cpid;             // use this to store a child PID

    char *s = malloc(INPUT_STRING_SIZE+1); // user input string
    tok_t *t;			                   // tokens parsed from input
    // if you look in parse.h, you'll see that tokens are just C-style strings (char*)

<<<<<<< HEAD
    strcpy(path_env, getenv("PATH"));
    strcpy(home_env, getenv("HOME"));
    paths = getToks(path_env);

=======
>>>>>>> 565928db65f0bc6190fc7770d311b368d060c960
    // perform some initialisation
    init_shell();

    fprintf(stdout, "%s running as PID %d under %d\n",argv[0],pid,ppid);
    /** TODO: replace "TODO" with the current working directory */
<<<<<<< HEAD
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    fprintf(stdout, CYELLOW "\n%d %s# " CNORMAL, lineNum, cwd); 
=======
    fprintf(stdout, CYELLOW "\n%d %s# " CNORMAL, lineNum, "TODO");
>>>>>>> 565928db65f0bc6190fc7770d311b368d060c960
    
    // Read input from user, tokenize it, and perform commands
    while ( ( s = freadln(stdin) ) ) { 
    
        t = getToks(s);            // break the line into tokens
        int fundex = lookup(t[0]); // is first token a built-in command?
        if( fundex >= 0 ) {
            cmd_table[fundex].fun(&t[1]); // execute built-in command
        } else {
            /** TODO: replace this statement to call a function that runs executables */
<<<<<<< HEAD
            int l = 0;
            while (t[l] != NULL) {l++;} //l = length of t

            if ((l >= 2) && (strcmp(t[l-2], ">") == 0)){    // redirect output
                char s[1024] = "";
                for (int i = 0; i < l-2; i++){
                    strcat(s, t[i]);
                    strcat(s, " ");
                }
                tok_t *arguments = getToks(s);

                freopen(t[l-1], "w", stdout);
                create_process(arguments, 0);
                freopen("/dev/tty", "w+", stdout);
            } else if ((l >= 2) && (strcmp(t[l-2], "<") == 0)){     //redirect input
                char s[1024] = "";
                for (int i = 0; i < l-2; i++){
                    strcat(s, t[i]);
                    strcat(s, " ");
                }
                tok_t *arguments = getToks(s);

                freopen(t[l-1], "r", stdin);
                create_process(arguments, 0);
                freopen("/dev/tty", "w+", stdin);
            } else if ((l >= 2) && (strcmp(t[l-1], "&") == 0)){     // ends with & - background process
                char s[1024] = "";
                for (int i = 0; i < l-1; i++){
                    strcat(s, t[i]);
                    strcat(s, " ");
                }
                tok_t *arguments = getToks(s);

                create_process(arguments, 1);
            } else{
                create_process(t, 0);
            }
=======
            fprintf(stdout, "This shell only supports built-in functions. Replace this to run programs as commands.\n");
>>>>>>> 565928db65f0bc6190fc7770d311b368d060c960
        }

        lineNum++;
        /** TODO: replace "TODO" with the current working directory */
<<<<<<< HEAD
        getcwd(cwd, sizeof(cwd));
        fprintf(stdout, CYELLOW "\n%d %s# " CNORMAL, lineNum, cwd);
=======
        fprintf(stdout, CYELLOW "\n%d %s# " CNORMAL, lineNum, "TODO");
>>>>>>> 565928db65f0bc6190fc7770d311b368d060c960
    }
    return 0;
}
