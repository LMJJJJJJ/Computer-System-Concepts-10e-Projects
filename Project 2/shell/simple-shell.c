#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h> // malloc
#include <fcntl.h> // O_APPEND

#define MAX_LINE 80 /* 80 chars per line, per command */

// make these global for sake of convenience of coding.
char *args[MAX_LINE/2 + 1]; /* command line (of 80) has max of 40 arguments */
int i = 0;
int should_run = 1;
char cmd[MAX_LINE];
char word[MAX_LINE];
char end;
int ampersand = 0;
int out_enable = 0;
int in_enable = 0;
int pipe_enable = 0;
// int is_empty = 0;
char out_file[MAX_LINE];
char in_file[MAX_LINE];


void welcome(){
    printf("                                                      \n");
    printf("  ****************************************************\n");
    printf("  *************   W  E  L  C  O  M  E   **************\n");
    printf("  ****************************************************\n");
    printf("  *                                                  *\n");
    printf("  *      This is a simple shell written by LMJ       *\n");
    printf("  *              It is easy to use!                  *\n");
    printf("  *                    Enjoy!                        *\n");
    printf("  *                                                  *\n"),
    printf("  ****************************************************\n");
    printf("                                                      \n");
}

void help(){
    printf("                                                    \n");
    printf("  ****************************************************\n");
    printf("  **************   G   U   I   D   E   ***************\n");
    printf("  ****************************************************\n");
    printf("  *                                                  *\n");
    printf("  *  1. You can use normal linux terminal commands.  *\n");
    printf("  *     e.g. 'ls', 'ls -l' ...                       *\n");
    printf("  *                                                  *\n");
    printf("  *  2. Use '!!' to execute the last command.        *\n");
    printf("  *                                                  *\n");
    printf("  *  3. Use '<' or '>' to change I/O                 *\n");
    printf("  *     e.g. 'sort < in.txt > out.txt'               *\n");
    printf("  *                                                  *\n");
    printf("  *  4. Pipe support -- use '|'                      *\n");
    printf("  *     e.g. 'ls -l | less'                          *\n");
    printf("  ****************************************************\n");
    printf("                                                    \n");
}

void reset_shell(){
    for (int j = 0; j < i; j += 1) free(args[i]);
    i = 0;
    ampersand = 0;
    out_enable = 0;
    in_enable = 0;
    pipe_enable = 0;
}

void exec_normal(){
    pid_t pid;
    int status;
    pid = fork();
    if (pid < 0) { // fork error
        printf("fork error!\n");
    }
    else if (pid == 0) { // child process
        if (execvp(args[0], args) == -1){
            perror("osh");
            exit(0);
        }
    }
    else { // parent process
        if (! ampersand){
            do{
                waitpid(pid, &status, WUNTRACED);
            }while(!WIFEXITED(status) && !WIFSIGNALED(status)); 
        }
    }
}

void exec_redirect(){
    pid_t pid;
    int status;
    pid = fork();
    if (pid < 0) { // fork error
        printf("fork error!\n");
    }
    else if (pid == 0) { // child process
        if (out_enable && !in_enable){
            int fd = open(out_file, O_CREAT|O_RDWR, 0666);
            if(fd < 0){
                printf("open error!\n");
            }
            dup2(fd, STDOUT_FILENO);
            execvp(args[0], args);
            close(fd);
        }
        else if (in_enable && !out_enable){
            int fd = open(in_file, O_CREAT|O_RDWR, 0666);
            if(fd < 0){
                printf("open error!\n");
            }
            dup2(fd, STDIN_FILENO);
            execvp(args[0], args);
            close(fd);
        }
        else { // in_enable && out_enable
            int out_fd = open(out_file, O_CREAT|O_RDWR, 0666);
            if(out_fd < 0){
                printf("open error!\n");
            }
            int in_fd = open(in_file, O_CREAT|O_RDWR, 0666);
            if(in_fd < 0){
                printf("open error!\n");
            }
            dup2(out_fd, STDOUT_FILENO);
            dup2(in_fd, STDIN_FILENO);
            execvp(args[0], args);
            close(out_fd);
            close(in_fd);
        }
    }
    else { // parent process
        if (! ampersand){
            do{
                waitpid(pid, &status, WUNTRACED);
            }while(!WIFEXITED(status) && !WIFSIGNALED(status)); 
        }
    }
}

void exec_pipe(){
    // pipe implementation
    pid_t pid;
    int status;
    pid = fork();
    if (pid < 0) { // fork error
        printf("fork error!\n");
    }
    else if (pid == 0) { // child process
        // get the pos of "|" and split the left and right parts
        int mid = 0;
        for (mid = 0; strcmp(args[mid], "|"); mid += 1);
        // printf("%d\n", mid);
        char *left[MAX_LINE/2 + 1];
        char *right[MAX_LINE/2 + 1];
        int j;
        for (j = 0; j < mid; j += 1){
            left[j] = (char*) malloc(sizeof(char) * (strlen(args[j]) + 1));
            strcpy(left[j], args[j]);
        }
        left[j] = NULL;
        for (j = mid + 1; j < i; j += 1){
            right[j - mid - 1] = (char*) malloc(sizeof(char) * (strlen(args[j]) + 1));
            strcpy(right[j - mid - 1], args[j]);
        }
        right[j - mid - 1] = NULL;
        pid_t inner_pid;
        int inner_status;
        int fd[2];
        if (pipe(fd) == -1){
            printf("pipe error\n");
        }
        inner_pid = fork();
        if (inner_pid < 0) {
            printf("fork error!\n");
        }
        else if (inner_pid == 0){
            close(fd[0]);
            dup2(fd[1],STDOUT_FILENO);
            execvp(left[0], left);
            close(fd[1]);
        }
        else {
            close(fd[1]);
            waitpid(inner_pid, &inner_status, WUNTRACED);
            dup2(fd[0], STDIN_FILENO);
            execvp(right[0], right);
            close(fd[0]);
        }
    }
    else { // parent process
        if (! ampersand){
            do{
                waitpid(pid, &status, WUNTRACED);
            }while(!WIFEXITED(status) && !WIFSIGNALED(status)); 
        }
    }
}

int main(void){
    welcome();
    while (should_run){   
        printf("osh > ");
        fflush(stdout);
        int first_time = 1;
        fgets(cmd, MAX_LINE, stdin);
        cmd[strlen(cmd)-1] = '\0';
        char *cmdremain = cmd;
        char *word;
        
        word = strsep(&cmdremain, " ");
        if (strlen(word) == 0) continue;
        do {
            if (first_time){
                if (strcmp(word, "!!") == 0) {
                    if (i == 0) {
                        printf("No commands in history!\n");
                        break;
                    }
                    for (int j = 0; j < i; j += 1){
                        printf("%s ", args[j]);
                    }
                    if (in_enable){
                        printf("< %s", in_file);
                    }
                    if (out_enable){
                        printf("> %s", out_file);
                    }
                    printf("\n");
                    break;
                }
                else {
                    reset_shell();
                    first_time = 0;
                }
            }
            if (strcmp(word, "exit") == 0) return 0;
            if (strcmp(word, "help") == 0) {
                help();
                break;
            }
            if (strcmp(word, "&") == 0) {
                ampersand = 1;
                break;
            }
            if (strcmp(word, "<") == 0){
                in_enable = 1;
                char *p;
                p = strsep(&cmdremain, " ");
                strcpy(in_file, p);
                continue;
            }
            if (strcmp(word, ">") == 0){
                out_enable = 1;
                char *p;
                p = strsep(&cmdremain, " ");
                strcpy(out_file, p);
                continue;
            }
            if (strcmp(word, "|") == 0){
                pipe_enable = 1;
            }

            args[i] = (char*) malloc(sizeof(char) * (strlen(word) + 1));
            strcpy(args[i], word);
            i += 1;
            // printf("%d %s\n", i, args[i-1]); // for debugging

            
        } while (word = strsep(&cmdremain, " "));

        args[i] = NULL;


        if (out_enable == 0 && in_enable == 0 && pipe_enable == 0) exec_normal();
        else if (out_enable == 1 || in_enable == 1) exec_redirect();
        else if (pipe_enable == 1) exec_pipe();

    }
    
	return 0;
}
