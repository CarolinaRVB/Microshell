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

**Notes**
The program does not support advanced shell features like background jobs, command history, or built-in command extensions (except cd).
Proper error handling is implemented for invalid commands or incorrect arguments.
