/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: crebelo- <crebelo-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 09:35:16 by crebelo-          #+#    #+#             */
/*   Updated: 2024/10/17 10:09:26 by crebelo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>


void	print_error(char *msg1, char *msg2, int fd) {
	for (int i = 0; msg1[i]; i++)
		write(fd, &msg1[i], 1);
	if (msg2) {
		for (int i = 0; msg2[i]; i++)
			write(fd, &msg2[i], 1);	
	}
	write(fd, "\n", 1);
}

int next_cmd(char **argv, int *last, char *c)
{
	for (int i = 0; argv[i]; i++) {
		if (strcmp(argv[i], c) == 0) {
			*last = i;
			return (1);
		}
	}
	return (0);
}

void	skip_semis(char ***argv) {
	for (int i = 0; (*argv)[i] && strcmp((*argv)[i], ";") == 0; i++) {
		(*argv)++;
	}
}

void	run_cd(char **argv) {
	if (!argv[1] || argv[2]) {
		print_error("error: cd: bad arguments", NULL, STDERR_FILENO);
		exit(1);
	}
	
	if (chdir(argv[1])) {
		print_error("error: cd: cannot change directory to ", argv[1], STDERR_FILENO);
		exit(1);
	}
	exit(0);	
}


int	count_pipes(char **argv) {
	int count = 0;
	for (int i = 0; argv[i]; i++) {
		if (strcmp(argv[i], "|") == 0)
			count++;
	}
	return (count);
}


void	execute_cmds(char **argv, char **envp) {
	pid_t	pid;
	int	fds[2];
	int next = 0;
	int	pre = -1;
	int npipes = count_pipes(argv);

	while (npipes) {
		if (pipe(fds) == -1) {
			print_error("error: fatal", NULL, STDERR_FILENO);
			exit(1);
		}
		if (next_cmd(argv, &next, "|"))
			argv[next] = NULL;
		pid  = fork();
		if (pid < 0) {
			print_error("error: fatal", NULL, STDERR_FILENO);
			exit(1);
		}
		if (pid == 0) {
			if (pre != 1) {
				dup2(pre, STDIN_FILENO);
				close(pre);
			}
			dup2(fds[1], STDOUT_FILENO);
			close(fds[1]);
			execve(argv[0], argv, envp);
			print_error("error: cannot execute ", argv[0], STDERR_FILENO);
			exit(1);
		}
		else {
			waitpid(pid, NULL, 0);
			if (pre != -1)
				close(pre);
			close(fds[1]);
			pre = fds[0];
			argv += next + 1;
			npipes--;
		}
		
	}
	
	if (pre != -1) {
		dup2(pre, STDIN_FILENO);
		close(pre);
	}
	if (argv[0] != NULL) {
		execve(argv[0], argv, envp);
		print_error("error: cannot execute ", argv[0], STDERR_FILENO);
		exit(1);
	}
}

int main(int argc, char **argv, char **envp)
{
	if (argc == 1)
		return (0);
	
	int index = 1;
	int last = 0;
	pid_t   pid;
	argv++;
	skip_semis(&argv);
	while (index < argc)
	{
		if (next_cmd(argv, &last, ";"))
		{
			argv[last] = NULL;
			
			pid = fork();
			if (pid < 0) {
				print_error("error: fatal", NULL, STDERR_FILENO);
				exit(1);
			}
			if (pid == 0) {
				if (strcmp(argv[0], "cd") == 0)
					run_cd(argv);
				else 
					execute_cmds(argv, envp);
			}
			else {
				waitpid(pid, NULL, 0);
				index = last + 1;
				argv += last + 1;
				skip_semis(&argv);
			}			
		}
		else {
			index = argc;
			execute_cmds(argv, envp);
		}
	}
	return (0);
}
