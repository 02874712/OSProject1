#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <assert.h>

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
    
    int i; 

    while (true) {
      
        do{ 
            // Get cwd
            getcwd(cwd, sizeof(cwd));
            strcat(cwd, prompt);

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
 
        } while(command_line[0] == 0x0A);  // while just ENTER pressed
        command_line[strlen(command_line)-1] = '\0';  // takes the \0 character off of the command line so it is not tokenized

        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }


        // Initialize the first command_line arg 
        i = 0;
        arguments[i] = strtok(command_line, delimiters);  


        // Tokenize the remaining args of the command_line
        while (arguments[i] != NULL) {
            i++;
            arguments[i] = strtok(NULL, delimiters);
        }
        

        printf("Arg Count: %d\n", i);
        // printf("Arg 0: %s\n", arguments[0]);   // debugging line for arg collection

        // STRCMP the first argument with built in commands 

  // B.I.C. exit...

        if(strcmp(arguments[0], "exit") == 0){
          exit(1);
        }

  // B.I.C. cd...

        else if(strcmp(arguments[0], "cd") == 0){
          if (chdir(arguments[1]) != 0) { // Checks for chdir failure and print error if chdir fails
            perror("cd error"); 
            break;
          }
          printf("Changing to dir - %s\n", arguments[1]);   // Change to the directory provided
        }
        else if(strcmp(arguments[0], "pwd") == 0){
          if (getcwd(cwd, sizeof(cwd)) == NULL){ // Checks for getcwd() error and print error if pwd fails 
            perror("pwd error"); 
            break;
          }
          printf("%s\n", cwd);    // Prints current working directory            
        } 

  // B.I.C. env...

        else if(strcmp(arguments[0], "env") == 0){
          char **env = environ;
          char *tenv; 

          //  prints all env variables 
          if (i == 1){ 
          // Loop through the environment variables
            while (*env) {
                printf("%s\n", *env);
                env++;
            }
          }
          
          // prints given env variable
          else if (i == 2){
            printf("%s\n", getenv(arguments[1] + 1));

          }

          // prints first env variable given and error message for rest provided
          else{
            tenv = getenv(arguments[1] + 1);
            printf("env: %s: No such file or directory\n", tenv);
          }


        }

  // B.I.C. echo...
        else if(strcmp(arguments[0], "echo") == 0){
          //  initialize parsing through arguments
          int k; 

          for(k = 1; k < i; k++){
            
            // if the given arg [0]=='$' then getenv
            if(arguments[k][0] == '$'){
              arguments[k] = getenv(arguments[k] + 1); 
            }
            printf("%s ", arguments[k]); // echos command line

          }
          
          printf("\n");

        }       

  // B.I.C. setenv...

        else if(strcmp(arguments[0], "setenv") == 0){
        
        // Set a new environment variable or update if it exists (overwrite = 1)
          if (setenv(arguments[1], arguments[2], 1) != 0) {
              perror("setenv failed");
              return 1;
          }

        }
        // else{
        //   break;
        // }
        // TODO:
        // 
        
			  // Completed 0. Modify the prompt to print the current working directory
        // Use getcwd to find cwd and strcat to combine with the prompt of shell
			
        // Completed 1. Tokenize the command line input (split it on whitespace)
        // Use strtok takes the command_line and tokenizes it by the delimiters while it is not NULL
      
        // 2. Implement Built-In Commands
        /*  
            Completed cd: changes the current working directory
            Completed pwd: prints the current working directory
            Completed echo: prints a message and the values of environment variables
            Completed exit: terminates the shell
            Completed env: prints the current values of the environment variables
            setenv: sets an environment variable
        */
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