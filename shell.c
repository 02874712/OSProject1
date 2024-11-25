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
char cwd[MAX_COMMAND_LINE_LEN];

//signal handler CTRL-C
void fproc_handler(int signum)
{ 

  if(fpid != -1){
    kill(fpid,SIGINT);
  }
  else{
    // Get cwd
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, prompt);

    // Print the shell prompt.
    printf("\n%s", cwd);
    fflush(stdout);
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

// Stores the tokenized command line input.
char *arguments[MAX_COMMAND_LINE_ARGS];
char *pip_arguments[MAX_COMMAND_LINE_ARGS];

char *input_file, *output_file;



int i, j, pid, pid_c, pip, status, background; 
int pipefd[2];

// create both pipes to communicate and check for failed pipes
if (pipe(pipefd) == -1) {
    perror("pipe");
    return -1;
}


while (true) {
  
  do{  
    // Get cwd
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, prompt);

    // Print the shell prompt.
    printf("%s", cwd);
    fflush(stdout);

    // Read input from stdin and store it in command_line. If there's an error, exit immediately. 
    if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
        fprintf(stderr, "fgets error");
        exit(0);
    }

  } while(command_line[0] == 0x0A);  // while just ENTER pressed
  command_line[strlen(command_line)-1] = '\0';  // takes the \0 character off of the command line

  // If the user input was EOF (ctrl+d), exit the shell.
  if (feof(stdin)) {
      printf("\n");
      fflush(stdout);
      fflush(stderr);
      return 0;
  }

  // Initialize the first command_line arg  
  // Tokenize the remaining args of the command_line 
  // Set flags and initialize counters for arg arrays

  i = 0; 
  j = 0;
  pip = -1;
  input_file = NULL;
  output_file = NULL;
  

  arguments[i] = strtok(command_line, delimiters);  


  // Tokenize the remaining args of the command_line
  while (arguments[i] != NULL) {

    if (strcmp(arguments[i], "<") == 0){

      arguments[i] = NULL; // Remove the redirection symbol
      i++;
      input_file = strtok(NULL, delimiters);

      if(input_file == NULL){
      fprintf(stderr, "Syntax Error: expected input file after '<'\n");
      exit(1);
      }
      arguments[i++] = strtok(NULL, delimiters);

    }
    else if (strcmp(arguments[i], ">") == 0){
      arguments[i] = NULL; // Remove the redirection symbol
      i++;
      output_file = strtok(NULL, delimiters);

      if (output_file == NULL) {
        fprintf(stderr, "Syntax Error: expected output file after '>'\n");
        exit(1);
      }
      arguments[i++] = strtok(NULL, delimiters);

    }
    else if(strcmp(arguments[i], "|") == 0){

      arguments[i] = NULL;  // Remove the redirection symbol
      pip_arguments[j] = strtok(NULL,delimiters); // Parse through remaining string initialize second arg array

      while (pip_arguments[j] != NULL){
        pip_arguments[++j] = strtok(NULL, delimiters);
      }

      arguments[i++] = strtok(NULL, delimiters);
      pip = 1; 
    }
    else{
      i++;
      arguments[i] = strtok(NULL, delimiters);
    }
  }






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

    // parse arguments for &
    if (i > 0 && arguments[i-1] != NULL && strcmp(arguments[i-1], "&") == 0) {
      background = 1;
      arguments[i - 1] = NULL; // Remove '&' from the arguments list
    }

    // forks off a child to handle external command
    pid = fork();

    //  Check for fork failure
    if(pid < 0){
      perror("fork failed!");
      exit(-1);
    }

    else if (pid > 0 && background == 0){   //parent waits for child process
      fpid = pid;
      alarm(TIMEOUT);
      close(pipefd[0]); 
      close(pipefd[1]);
      wait(&status);
      fpid = -1;
    }
    else if (pid > 0 && background == 1)  {} //parent doesn't wait for child process
    else{
      
      //  reset signal to default for child
      signal(SIGINT, SIG_DFL);
      signal(SIGALRM, SIG_DFL);
      
      if (input_file != NULL){
        int fd_in = open(input_file, O_RDONLY);
        if (fd_in < 0){
          perror(input_file);
          exit(1);
        }
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
      }

      if (output_file != NULL){
        int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out < 0){
          perror(output_file);
          exit(1);
        }
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
      }


 // handles pipe i/o redirection 
      if (pip != -1){

        pid_c = fork();

        if (pid_c < 0)
          return -1;
        if (pid_c == 0){
        close(pipefd[1]);

        dup2(pipefd[0], 0);

        close(pipefd[0]);

        printf("child to execute %s\n", pip_arguments[0]);
        // execute args after pipe given stdin via pipe
        if (pip_arguments[0] == NULL) {
          fprintf(stderr, "No command specified\n");
          exit(EXIT_FAILURE);
        }
        execvp(pip_arguments[0], pip_arguments);
        perror("execvp in child failed");
        exit(EXIT_FAILURE);
        }
        else{
        printf("parent closed its pipes\n");
        
        close(pipefd[0]);

        dup2(pipefd[1], 1);

        close(pipefd[1]);
      
       }
      }

      // Execute the command
      if (arguments[0] == NULL) {
        fprintf(stderr, "No command specified\n");
        exit(EXIT_FAILURE);
      }
      
      execvp(arguments[0], arguments);
      perror("execvp() failed");
      exit(EXIT_FAILURE);            
    
  }
  //  This resets the alarm for the completed process.
  alarm(0);
}
}
// This will never be reached.
return -1;
}