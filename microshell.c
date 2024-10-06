/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: crebelo- <crebelo-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 09:35:16 by crebelo-          #+#    #+#             */
/*   Updated: 2024/10/06 10:29:53 by crebelo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int count_pipes(int argc, char **argv) {
	int count = 0;
	for (int i = 0; i < argc && argv[i]; i++) {
		if (strcmp(argv[i], "|") == 0)
			count++;
	}
	return count;
}

void print_error(const char *msg1, const char *msg2, int fd) {
	while (*msg1)
		write(fd, msg1++, 1);
	if (msg2) {
		while (*msg2)
			write(fd, msg2++, 1);
	}
	write(fd, "\n", 1);
}

int run_cd(int argc, char **argv) {
	if (argc != 3) {
		print_error("error: cd: bad arguments", NULL, STDERR_FILENO);
		return 1;
	}
	if (chdir(argv[2]) == -1) {
		print_error("error: cd: cannot change directory to ", argv[2], STDERR_FILENO);
		return 1;
	}
	return 0;
}

void skip_semi(char ***argv, int *index) {
	while ((*argv)[0] && strcmp((*argv)[0], ";") == 0) {
		(*index)++;
		(*argv)++;
	}
}

int execute_single_cmd(char **argv, char **envp) {
	execve(argv[0], argv, envp);
	print_error("error: cannot execute ", argv[0], STDERR_FILENO);
	exit(1);
}

void handle_pipe_redirection(int fds[2], int first_fd) {
	if (first_fd != -1) {
		dup2(first_fd, STDIN_FILENO); // Redirect stdin to previous pipe read end
		close(first_fd);
	}
	dup2(fds[1], STDOUT_FILENO); // Redirect stdout to pipe write end
	close(fds[1]);
}

int next_cmd(char **argv, int *last) {
	for (int i = 0; argv[i]; i++) {
		if (strcmp(argv[i], ";") == 0) {
			*last = i;
			return 0;
		}
	}
	return -1;
}

int execute_cmds(int argc, char **argv, char **envp) {
	int i = 0;
	int status = 0;
	int first_fd = -1;
	int npipes = count_pipes(argc, argv);
	int fds[2];
	pid_t pid;

	while (npipes > 0) {
		if (pipe(fds) == -1) {
			print_error("error: fatal", NULL, STDERR_FILENO);
			exit(1);
		}
		i = 0;
		while (argv[i]) {
			if (strcmp(argv[i], "|") == 0) {
				argv[i] = NULL;
				break;
			}
			i++;
		}
		pid = fork();
		if (pid < 0) {
			print_error("error: fatal", NULL, STDERR_FILENO);
			exit(1);
		}
		if (pid == 0) { // Child process
			handle_pipe_redirection(fds, first_fd); // Manage pipe redirection
			execute_single_cmd(argv, envp); // Execute the command
		}
		else { // Parent process
			waitpid(pid, &status, 0); // Wait for the child process
			if (first_fd != -1)
				close(first_fd); // Close the previous read end
			close(fds[1]); // Close the write end of the current pipe
			first_fd = fds[0]; // Save the read end of the current pipe
			argv += i + 1; // Move to the next command in argv
			npipes--; // Decrease the number of pipes left
		}
	}
	// Handle the final command (without pipe)
	if (first_fd != -1) {
		dup2(first_fd, STDIN_FILENO); // Set the final command's input to the last pipe's read end
		close(first_fd);
	}

	if (argv[0] != NULL) {
		execute_single_cmd(argv, envp);
	}
	return status;
}

int main(int argc, char **argv, char **envp) {
	int status = 0;
	int index = 1;
	int last = 0;
	pid_t pid;

	if (argc == 1)
		return 0;
	argv++;
	skip_semi(&argv, &index);
	while (index < argc) {
		if (next_cmd(argv, &last) != -1)
		{
			argv[last] = NULL;
			pid = fork();
			if (pid < 0) {
				print_error("error: fatal", NULL, STDERR_FILENO);
				exit(status);
			}
			if (pid == 0) {
				if (strcmp(argv[0], "cd") == 0){
					run_cd(argc, argv);
					exit(status);
					}
				else
					status = execute_cmds(argc, argv, envp);
			}
			else {
				argv += last + 1;
				index += last + 1;
				skip_semi(&argv, &index);
			}
			waitpid(pid, &status, 0);
		} 
		else {
			status = execute_cmds(argc, argv, envp);
			index = argc;
		}
	}
	return status;
}
