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
#include <time.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128
#define TIMEOUT 10


char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;
pid_t fpid = -1;  

//signal handler CTRL-C
void fproc_handler(int signum)
{ 

  if(fpid != -1){
    kill(fpid,SIGINT);
  }

}

// signal handler Alarm
void lrproc_handler(int signum){

  if(fpid != -1){
    kill(fpid,SIGINT);
  }

}




int main() {
    // Register Signal handler for Ctrl-C
    signal(SIGINT, fproc_handler); 

    // Register Signal handler for Alarm
    signal(SIGALRM, lrproc_handler);

    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
  
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];

    // Stores current working directory
    char cwd[MAX_COMMAND_LINE_LEN];
    
    int i, pid, status, background, pip, lthan, gthan; 

    


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
            printf("%s\n", arguments[i]);
            i++;
            arguments[i] = strtok(NULL, delimiters);
        }
        
        // alarm(TIMEOUT);

        // STRCMP the first argument with built in commands 

  // B.I.C. exit...

        if(strcmp(arguments[0], "exit") == 0){

          exit(1);
        }


  // B.I.C. cd...

        else if(strcmp(arguments[0], "cd") == 0){ 

          // if the given arg [0]=='$' then getenv
          if(arguments[1][0] == '$'){
            arguments[1] = getenv(arguments[1] + 1); 
          }
          if (chdir(arguments[1]) != 0) { // Checks for chdir failure and print error if chdir fails
            perror("cd error"); 
          }
          else{
            printf("Changing to dir - %s\n", arguments[1]);   // Change to the directory provided
          }
          
        }

  // B.I.C. pwd...

        else if(strcmp(arguments[0], "pwd") == 0){
          if (getcwd(cwd, sizeof(cwd)) == NULL){ // Checks for getcwd() error and print error if pwd fails 
            perror("pwd error"); 
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
          if (setenv((arguments[1] + 1), arguments[2], 1) != 0) {
              perror("setenv failed");
              return 1;
          }

        }

  // Else not a B.I.C. fork off a child process and call exec function
        
        else{

          background = 0;   //Initializes bkgd process to wait in parent
          pip = 0;
          lthan = 0;
          gthan =0; 
          
          if (strcmp(arguments[i-1], "&") == 0){
            background = 1;
            arguments[i - 1] = NULL; // Remove '&' from the arguments list
          }

          pid = fork();

          if(pid < 0){
            printf("fork failed!");
            exit(-1);
          }
          else if (pid > 0 && background == 0){   //parent waits for child process
            fpid = pid;
            alarm(TIMEOUT);

            wait(&status);
            fpid = -1;
          }
          else if (pid > 0 && background == 1)  {} //parent doesn't wait for child process
          else{

            //  reset signal to default for child
            signal(SIGINT, SIG_DFL);
            signal(SIGALRM, SIG_DFL);   

            execvp(arguments[0], arguments);
            perror("execvp() failed");
            exit(EXIT_FAILURE);            
          }
        }
        //  This resets the alarm for the completed process.
        alarm(0);
    }
    // This will never be reached.
    return -1;
}