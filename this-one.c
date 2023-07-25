#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
/**
 * execute_external_command - Executes a command not built into the shell.
 * @argv: an array of arguments passed to the function
 * This function creates a child process and attempts to execute the command
 * specified in the argv array. If successful, the child process will run the
 * command and then terminate. The parent process will wait for the child to
 * complete execution before terminating.
 * Return: 0 on success, -1 on failure.
 */


int execute_external_command(char **argv)
{
	pid_t pid;
	int status = 0, i = 0;
	char *envp[] = { NULL };

	pid = fork();

	if (pid == 0)
	{
		/* This part will handle the child process */
		execve(argv[0], argv, envp);
		perror("execve error");
		while (argv[i] != NULL)
		{
			free(argv[i]);
			i++;
		}
		free(argv);
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		/* The fork failed */
		perror("fork error");
		while (argv[i] != NULL)
		{
			free(argv[i]);
			i++;
		}
		free(argv);
		return (-1);
	}
	else
	{
		/* This is the parent process */
		waitpid(pid, &status, 0);
	}
	return (0);
}


/**
 * main - Entry point for the shell program.
 * @argc: The number of arguments passed to the program
 * @argv: An array of strings containing the arguments
 * This function contains the main loop for the shell program. It prints a
 * prompt message, reads input from the user, and determines whether the
 * command entered is an internal or external command. Internal commands are
 * executed by calling the execute_internal_command function, and external
 * commands are executed by calling the execute_external_command function.
 * Return: 0 on success.
 **/

int main(int argc, char **argv)
{
	char *lineptr = NULL;
	char *lptr_dup = NULL;
	size_t n = 0;
	ssize_t inputL = 0;
	const char *delim = " \t\r\n\a";
	int numTok = 0;
	char *token = NULL;
	int i, s = 0;

	(void)argc;

	while (1)
	{
		argv = NULL;

		write(1, "prompt$ ", strlen("prompt$ "));
		inputL = getline(&lineptr, &n, stdin);

		if (inputL == -1)
		{
			free(lineptr);
			return (-1);
		}
		/* allocate space for a copy of the lineptr */
		lptr_dup = malloc(sizeof(char) * (size_t)inputL + 1);
		if (lptr_dup == NULL)
		{
			perror("memory allocation error");
			free(lineptr);
			return (-1);
		}
		/* copy lineptr to lptr_dup */
		strcpy(lptr_dup, lineptr);
		/* calculate the total number of tokens */
		token = strtok(lineptr, delim);

		while (token != NULL)
		{
			numTok++;
			token = strtok(NULL, delim);
		}
		numTok++;
		/* Allocate space to hold the array of strings */
		argv = malloc(sizeof(char *) * numTok);
		if (argv == NULL)
		{
			free(lineptr);
			free(lptr_dup);
			perror("oops, memory allocation error");
			return (-1);
		}
		/* Store each token in the array argv  */
		token = strtok(lptr_dup, delim);
		s = strlen(token);
		for (i = 0; token != NULL; i++)
		{
			argv[i] = malloc(sizeof(char) * (s + 1));
			if (argv[i] == NULL)
			{
				free(lineptr);
				free(lptr_dup);
				free(argv);
				perror("memory allocation error");
				return (-1);
			}
			strcpy(argv[i], token);
			token = strtok(NULL, delim);
		}
		argv[i] = NULL;
		/* Determine if the command is internal or external */

		if (strcmp(argv[0], "ls") == 0)
		{
			argv[0] = "/bin/ls";
			execute_external_command(argv);
		}
		else if (strcmp(argv[0], "exit") == 0)
		{
			free(lineptr);
			free(token);
			free(lptr_dup);
			for (; i > 0; i--)
				free(argv[i]);
			free(argv);
			break;
		}
		else
		{
			execute_external_command(argv);
		}
		for (; i > 0; i--)
			free(argv[i]);
		free(argv);
		free(token);
	}
	return (0);
}