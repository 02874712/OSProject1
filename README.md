**Project 1 - Create Your own Shell (Quash)**

The Quash shell is a simple implementation of a basic shell to demonstrate the workings of a command line interface, process forking, and signals.  The Quash shell incorporates a set of 6 initial built-in commands which include: cd, pwd, echo, exit, env, setenv.  When the Quash shell initially runs, it prints its current working directory along with the ‘>’ delimiter. It is furthermore concatenated as a user works within any of a given shell’s directories. The shell operates within a do while loop to give the seamless implementation of the Quash shell.  Global variables aid in numerous features that are provided within the shell. They include variables to distinguish background processes vs foreground processes via pid (fpid), delimiters to aid in tokenizing the command line (delimiters []), a character array to store the current working directory (cwd[MAX_COMMAND_LINE_LEN]), and a global timeout value to distinguish  long running processes (TIMEOUT). In this implementation, our processes terminate after 10 seconds if the given process has not completed within that window of time. Quash’s implementation is further discussed in detail along with the decisions of the breakdown of its execution.  
     Before the Quash shell features can be discussed, the first thing that is worth mentioning is local variables inside the main function outside of the do while loop. The main process that runs the shell registers its new signal handlers for CTRL-C, which is handled by the foreground process signal handler function, (or fproc_handler), and ALARM, which is handled by the long running process signal handler, (or lrproc_handler) and will be discussed later in the model of the shell. However, when CTRL-C is pressed, it is handled by fproc_handler in conjunction with the fpid initially set to –1 to represent the parent process running the shell. The fpid will be later discussed as it pertains to the forking of processes and distinguishing the parent process from the foreground process running. There are three important character arrays that aid in the command line handling. The first array, command_line [MAX_COMMAND_LINE_LEN], holds the initial command line input as a character array with the max length of 1024 characters, a limit defined by MAX_COMMAND_LINE_LEN. With this limit in place, we create a second array, *arguments [MAX_COMMAND_LINE_ARGS], that holds pointers to the initial command line as it has been tokenized by the set delimiters. The delimiters include space, tab, newline, and carriage return when separating tokens. The last set of variables worth noting are the integer declared variables that aid in the shell’s implementation deeper within its structure. These variables include the pid to keep track of a parent and child after forking, boolean int variable, background, to denote which environment a process is running, status, to keep track of the child processes and i, the initialization for tokenizing the command line and to furthermore keep track of the amount of commands tokenized on the command line. When the command line is first tokenized into the arguments array, string tokenization(or strtok) is used to separate the commands and once the while loop detects the EOL NULL terminator, it completes. Initially, the idea was to grab the first command passed on the command line and compare it to the built-in commands, however, it increases the time complexity of the if-else statement by having to tokenize the remaining command line. It also raises an issue when trying to tokenize the rest of the command line and string tokenization has been used in another block of the code. Therefore, it was decided to tokenize the complete command line before executing commands.  
    The Built In Commands are executed via an if-else statement that is to be handled by the parent process running the Quash shell. It works by string comparing the first arg tokenized in the arguments array to the equivalent built in commands. Quash shell commands include “exit”, “cd”, “env”, “setenv”, “echo”, and “pwd”. These commands can be fulfilled by calling system functions that easily handle and return their appropriate caller values. Examples include calling exit() if the (strcmp(arguments[0], “exit”) == 0), or the value of getenv() if the argument evaluates to “env”.  Some of these commands require further evaluation before executing their equivalent system commands. For example, env can be evaluated in several different ways. In the shell, there is an array of environment variables stored externally in a character array of strings. There are three cases that the env command handles once it is found on the command line. If env is only found, the command parses through the array and prints all environment variables found in the shell, including any new environment variables set by the user.  The next situation accounted for is when env is passed just one variable and in turn returns its given value. It is found by removing the $ character and passing the actual variable name to the getenv function and printing its variable VALUE. When the env command receives more than one key, then quash handles the first key passed and then expresses in the terminal that no further commands can be found. This error feature mimics a quality that a user of the bash shell would encounter. Changing directories are equipped with handling environment variables as well. For the directory passed to cd, the statement will check if the first character of the second argument is ‘$’ then the pathway is found via getenv and passed along to the cd command for execution. 
     The built-in commands are limited to a small set of commands that we can compare in the if-else statement. In this case when the command cannot be handled by the parent process, it forks off a child to handle an exec function call. Before execution, the quash shell resets the child’s signal to its default signal and parses the arguments array to check for an ampersand. If an ampersand is found, the global variable background is set to 1 and the child removes the '&’ before execution via the exec command. This allows for the & to not cause the exec function to fail upon execution. Setting the background variable is an important step within the child because it communicates with the parent if it should wait or not. The parent after forking off its child goes through a series of evaluations. Each evaluation is dependent on the background variable and if it should wait for the child to complete. When the background is set to 0, it follows the case in regard to the alarm lrproc_handler,  as will be mentioned later in Quash’s implementation. Otherwise, the parent continues on to print the shell prompt without waiting for the child to finish its execution. This results in a zombie process that terminates without a parent to return to.    
    Given that the Quash shell is running within the bash shell, there are a few signals that can prematurely affect the execution within Quash. Exit, a built-in command, successfully kills the Quash shell and returns the terminal back to the Bash shell. One of the first checks that happens in the shell is to check whether the feof(STDIN), or CTRL-D, has been entered. If so, the shell will flush out any remaining output stored within the kernel’s buffer and/or errors before returning and successfully terminating the shell. CTRL-C is handled by fproc_handler due to the premature termination of CTRL-C within the shell. Without the handler function, the shell would not terminate as intended and the shell would terminate altogether. If the foreground process is –1, meaning that the terminal is within the parent process and no child has not been called, the process is not terminated. Otherwise, the fpid is killed off via the kill function and passing it –9. To accommodate the CTRL-C and the fpid is not a child process, the shell prints a new prompt within the Quash shell as if it were to be operating in the parent Bash shell.  
    Signal functions play a vital role in making the shell run and handling any rogue processes. The SIGALRM handler, lrproc_handler works in a similar nature to the CTRL-C handler function. It utilizes the fpid variable to kill off any process that runs longer than a timeout value defined initially. In the case of Quash shell, any foreground process that has not completed after 10 seconds will be killed off.  In the parent process, after it forks off a child, it sets the fpid to the child’s pid. The parent waits for the child to execute, in which, if the child successfully executes and returns, the parent will reset the fpid to –1.  It is important to mention when the alarm is called. After the pid is set in the parent, it will set the alarm and if it goes off in the allotted time, it is handled by the lrproc_handler. However, if the child process successfully executes, the parent will break out of the if-else statement and reset the alarm to 0 to disarm, essentially killing the alarm. 
    To handle redirection, two character variables are introduced to handle the I/O symbols should they appear within the initial command line. The idea was to create two separate arrays to handle commands should they fall on specific sides of the I/O redirection symbols and then pass them to the exec function. However, the idea was not successful and the two character variables for the input/output files stemmed from this failure. If either of the two characters are present, then they are overwritten with NULL within the command line, and the next command is immediately tokenized from the initial array. This logic avoids running into premature termination of the argument initialization loop. Depending on the I/O character, the input or output file will be set to the next token received on the command line. If no other command exists after the symbol, then it will be handled by standard error.  After the child resets its alarm, it will first check for an input file and/or output file. If one were to exist, then it is opened by the system and its standard input and output are duplicated, respectively. After checking for both, the child continues on to execute the arguments array found on the command line and writes to the terminal.
    The Quash shell is far from perfect and has certain downfalls with its simple implementation. Given the execution of the shell, it could potentially face complexity issues when looping through and forking off a child that has its own complex level of execution. Another issue faced within the Quash shell is the race condition between the parent and child over the background variable. If the parent loops through the final else condition in which it has to fork off a child to execute a command, it also has to evaluate whether it should wait or not for said child. If the child sets the background variable after the parent evaluates its if statement conditions and it were to be a background process, the parent could be in a situation where it waits for a child that could run on and on for an infinite amount of time. The Quash shell has the features of a simple shell implementation that has allowed for me to become familiar with the workings of the command line and how system and kernel calls can be made to execute simple commands. 
