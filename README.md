# Microshell

## Description
This project implements a simple shell in C, which can execute basic commands and handle pipe (|) and semicolon (;) operators for chaining commands. It provides basic functionality similar to a Unix Bash shell, where you can run programs, change directories, and handle multiple commands in a single input line.

The shell supports the following features:

- Running commands using execve()
- Chaining multiple commands separated by ; (sequential execution)
- Piping commands using | (redirecting output from one command to the input of the next)
- Handling the cd command to change directories
- Error handling for various conditions like command execution failure, fatal errors, and bad arguments for cd.

## Usage
Compile the code using a C compiler:
```bash
cc -Wall -Werror -Wextra microshell.c -o microshell
```

Run the executable with commands:

```bash
./microshell [COMMANDS]
```
For example:
```bash
./microshell ls -l ; echo "Hello World" | cat ; cd ..
```
## Features

**Multiple commands:** Use semicolons (;) to separate and run multiple commands sequentially.
**Piping:** Use pipes (|) to pass the output of one command as input to the next command.
**Change directory (cd):** Change directories using cd. Only works with one argument (target directory).
**Error messages:** The shell provides descriptive error messages if a command fails or a wrong argument is given.

Example
```bash
./microshell ls -la | grep microshell ; cd ..
```
This example runs ls -la, filters the output through grep microshell, then changes the directory to the parent folder.

## Functions
- **count_pipes():** Counts the number of pipes (|) in the input arguments.
- **print_error():** Prints error messages to the standard error.
- **run_cd():** Executes the cd command to change directories.
- **skip_semi():** Skips semicolons (;) in the argument list.
- **execute_single_cmd():** Executes a single command using execve().
- **handle_pipe_redirection():** Manages pipe redirection for commands connected by |.
- **next_cmd():** Finds the next command in the input.
- **execute_cmds():** Executes a sequence of piped commands.
- **main():** Entry point for the program, managing command execution and child process creation.

### code logic

**main():**
- Command Parsing: The function iterates through the provided commands in argv, identifying individual commands separated by semicolons using helper functions like skip_semi() and next_cmd().

- Forking and Process Management: For each valid command, the program forks a new process. The child process executes the command, while the parent waits for the child process to finish using waitpid(). If the command is cd, it is executed directly in the child process. If no individual commands are found, the program simply executes the current command.

- Command Execution: Commands are executed via the execute_cmds() function, which interacts with system calls to run the specified commands in the shell. If an error occurs during fork(), an error message is printed and the program exits.

- Handling Special Commands: The function includes special handling for the cd (change directory) command using run_cd().

- Return Status: The function returns the final exit status of the executed commands. 

**execute_cmds():**
- Pipe Management:
The function calculates the number of pipes (npipes) in the input command string using count_pipes().
For each command, a pipe is created using pipe(fds) to facilitate communication between the current and subsequent processes.

- Command Execution with Piping:
It forks a new process for each command. The child process performs necessary pipe redirections via handle_pipe_redirection(), ensuring the output of one command becomes the input of the next.
The command is executed in the child process using execute_single_cmd(), which interacts with system call execve to run the command.

- Parent Process Management:
The parent process waits for the child process to complete using waitpid().
It manages pipe file descriptors, closing the write end of the current pipe and keeping track of the read end for the next command.

- Final Command Handling:
After processing all commands with pipes, the function handles the final command. If there are pipes, the input for this command is set to the read end of the last pipe.

- Return Status:
The function returns the exit status of the final command executed. However the current implementation of this feature is faulty.

**Notes**
The program does not support advanced shell features like background jobs, command history, or built-in command extensions (except cd).
Proper error handling is implemented for invalid commands or incorrect arguments.
