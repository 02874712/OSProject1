#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;



int main() {
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
  
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];

    // Stores current working directory
    char cwd[MAX_COMMAND_LINE_LEN];

    getcwd(cwd, sizeof(cwd));
    strcat(cwd, prompt);

    while (true) {
      
        do{ 
            // Print the shell prompt.
            printf("%s", cwd);
            fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
        
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }
 
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(SIGTSTP)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        int k = 0;
        arguments[k] = strtok(command_line, delimiters);
        while(arguments != NULL){
          arguments[k++] = strtok(NULL, delimiters);
        }

        // TODO:
        // 
        
			  // Completed 0. Modify the prompt to print the current working directory
        // Use getcwd to find cwd and strcat to combine with the prompt of shell
			
        // 1. Tokenize the command line input (split it on whitespace)
        // Use strtok takes the command_line and tokenizes it by the delimiters while it is not NULL
      
        // 2. Implement Built-In Commands
        //
    
        // 3. Create a child process which will execute the command line input

  
        // 4. The parent process should wait for the child to complete unless its a background process
      
      
        // Hints (put these into Google):
        // man fork
        // man execvp
        // man wait
        // man strtok
        // man environ
        // man signals
        
        // Extra Credit
        // man dup2
        // man open
        // man pipes
    }
    // This should never be reached.
    return -1;
}