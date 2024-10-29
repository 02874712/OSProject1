****Project 1 - Create Your own Shell (Quash)****

The Quash shell is a simple implementation of a basic shell 
to demonstrate the workings of a command line interface, 
process forking, and signals.  The Quash shell incorporates a 
set of 6 initial built in commands which include: cd, pwd, 
echo, exit, env, setenv. When the Quash shell initially runs, 
it prints its current working directory along with the ‘>’ 
delimiter. It is furthermore concatenated as a user works 
within any of the given directories. The shell operates within
a do while loop to give the seamless implementation of the 
Quash shell. Global variables help aid in the running of many
implementations of features that are provided within the shell.
They include variables to distinguish background processes vs 
foreground processes via pid (fpid), delimiters to aid in 
tokenizing the command line (delimiters []), a character array
to store the current working directory (cwd[MAX_COMMAND_LINE_LEN]),
and a global timeout value to aid in long running processes 
(TIMEOUT). In this implementation, our processes terminate 
after 10 seconds if the given process has not completed within
that window of time.   

Before the Quash shell features can be discussed, the first thing that is worth mentioning is local variables inside the main function outside of the do while loop. The main process that runs the shell registers its new signal handlers for CTRL-C, which is handled by the foreground process signal handler function, (or fproc_handler), and ALARM, which is handled by the long running process signal handler, (or lrproc_handler) and will be discussed later in the model of the shell. However, when CTRL-C is pressed, it is handled by fproc_handler in conjuncture with the fpid initially set to –1 to represent the parent process running the shell. The fpid will be later discussed as it pertains to the forking of process and distinguishing the parent process from the foreground process running. There are three important character arrays that aid in the command line handling. The first array, command_line[MAX_COMMAND_LINE_LEN], holds the initial command line input as a character array with the max length of 1024 characters. This limit is defined by MAX_COMMAND_LINE_LEN. With this limit in place, we create a second array, *arguments [MAX_COMMAND_LINE_ARGS], that holds pointers to the initial command line as it has been tokenized by the set delimiters. The delimiters include space, tab, newline, and carriage return when separating words. The last variables worth mentioning are the integers that help aid in the shell’s implementation deeper within the structure. They include the pid to keep track of a parent and child after forking, boolean int variable, background, to denote which environment a process is running, status to keep track of  the child processes and i, initialization for tokenizing the command line and to furthermore keep track of the amount of commands tokenized on the command line.  
