#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINES_LEN 510
#define MAX_ARG_NUM 10

int cmdCount = 0;
int argsCount = 0;

void execute_command(char **args);
void tokenize(char *line, char **args);

int main() {
    char lines[MAX_LINES_LEN];
    char *args[MAX_ARG_NUM];
    int num_empty = 0;  // keep track of how many empty lines entered
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    memmove(cwd, cwd + 1, strlen(cwd));
    while (num_empty < 3) {
        printf("#cmd %d|#args: %d@%s>", cmdCount,argsCount, cwd);

        // read command line
        if (fgets(lines, MAX_LINES_LEN, stdin) == NULL) {
            break;
        }
        
        if (strcmp(lines, "\n") == 0){
            num_empty++;
            continue;
        }
        else{
            num_empty = 0;
            cmdCount++;
            argsCount++;
        }

        // remove trailing newline character
        if (lines[strlen(lines) - 1] == '\n') {
            lines[strlen(lines) - 1] = '\0';
        }

        // tokenize command line into commands and arguments
        tokenize(lines, args);
    }

    return 0;
}

void execute_command(char **args) {
    if (strcmp(args[0], "cd") == 0){
        fprintf(stderr, "Error: cd is not supported\n");
        cmdCount--;
        argsCount--;
        return;
    }
    else if (strcmp(args[0], "echo") == 0) {
        // handle echo command
        for (int i = 1; args[i] != NULL; i++) {
            char *arg = args[i];
            if (arg[0] == '"' ) {
                arg++;
            }
            if(arg[strlen(arg)-1] == '"'){
                arg[strlen(arg)-1] = '\0';
            }
            printf("%s ", arg);
        }
        printf("\n");
        return;
    }
    // fork and execute command
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork failed\n");
            cmdCount--;
            argsCount--;
        return;
    } else if (pid == 0) {
        // child process
        execvp(args[0], args);
        fprintf(stderr, "execvp failed\n");
        cmdCount--;
        argsCount--;
        exit(1);
    } else {
        // parent process
        wait(NULL);
    }
}


void tokenize(char *line, char **args) {
    // tokenize command line into commands
    char *token = line;
    char *command;
    while ((command = strsep(&token, ";")) != NULL) {
        // check for variable assignment
        char *equal_sign = strchr(command, '=');
        if (equal_sign != NULL) {
            // set environment variable
            *equal_sign = '\0';
            char *var_name = command;
            char *var_value = equal_sign + 1;
            setenv(var_name, var_value, 1);
            continue;
        }
        // tokenize command into arguments
        char *arg;
        int i = 0;
        while ((arg = strsep(&command, " ")) != NULL) {
            if (*arg == '\0') continue;
            if (*arg == '$') {
                // substitute variable value
                char *var_name = arg + 1;
                char *var_value = getenv(var_name);
                if (var_value == NULL) {
                    fprintf(stderr, "Undefined variable: %s\n", var_name);
                    cmdCount--;
                    argsCount--;
                    continue;
                }
                arg = var_value;
            }
            // remove double quotes from argument
            if (arg[0] == '"' && arg[strlen(arg)-1] == '"') {
                arg[strlen(arg)-1] = '\0';
                arg++;
            }
            args[i++] = arg;
            if (i == MAX_ARG_NUM - 1) {
                fprintf(stderr, "Too many arguments\n");
                argsCount--;
                break;
            }
        }
        args[i] = NULL;
        execute_command(args);
    }
}


