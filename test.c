#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024
extern char **environ;

/* Function prototypes */
char *_getline(FILE *stream);
char **parse_cmd(char *cmd);
int cmd_execution(char **cmd);
void free_cmd(char **cmd);
void clear_scrn(void);
void exit_shell(int status);

int main(void)
{
	char *cmd = NULL;
	size_t length;
	int e_Status = 0;
	bool is_tty = isatty(STDIN_FILENO);

	while (1)
	{
		if (is_tty)
			write(STDOUT_FILENO, "prompt$ ", strlen("prompt$ "));

		cmd = _getline(stdin);
		if (cmd == NULL)
		{
			if (is_tty)
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		length = strlen(cmd);
		if (length > 0 && cmd[length - 1] == '\n')
		{
			cmd[length - 1] = '\0';
			length--;
		}
		if (length == 0)
		{
			free(cmd);
			continue;
		}

		if (strcmp(cmd, "clear") == 0)
		{
			clear_scrn();
		}
		else if (strcmp(cmd, "env") == 0)
		{
			char **env = environ;
			while (*env != NULL)
			{
				write(STDOUT_FILENO, *env, strlen(*env));
				write(STDOUT_FILENO, "\n", 1);
				env++;
			}
		}
		else if (strncmp(cmd, "exit", 4) == 0)
		{
			if (strlen(cmd) > 4)
			{
				e_Status = atoi(cmd + 5);
			}
			exit_shell(e_Status);
			free(cmd);
			break;
		}
		else
		{
			char **args = parse_cmd(cmd);
			if (args)
			{
				int status = cmd_execution(args);
				free_cmd(args);
				if (status < 0)
				{
					write(STDERR_FILENO, "Failed to execute.\n", strlen("Failed to execute.\n"));
				}
			}
		}

		free(cmd);
	}

	return 0;
}

char *_getline(FILE *stream)
{
	static char buffer[BUFFER_SIZE];
	static int index = 0, bytesRead = 0;
	char c, *newCmd;
	int Lsize = 0, currentBuff = BUFFER_SIZE;

	char *cmd = malloc(currentBuff * sizeof(char));
	if (cmd == NULL)
	{
		perror("Memory allocation error");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		if (index == bytesRead)
		{
			bytesRead = read(fileno(stream), buffer, BUFFER_SIZE);
			if (bytesRead == -1)
			{
				perror("Error from read");
				exit(EXIT_FAILURE);
			}
			if (bytesRead == 0)
				break;
			index = 0;
		}
		c = buffer[index++];
		if (c == '\n')
		{
			cmd[Lsize] = '\0';
			break;
		}
		if (Lsize + 1 == currentBuff)
		{
			currentBuff *= 2;
			newCmd = realloc(cmd, currentBuff * sizeof(char));
			if (newCmd == NULL)
			{
				free(cmd);
				perror("Memory allocation error");
				exit(EXIT_FAILURE);
			}
			cmd = newCmd;
		}
		cmd[Lsize] = c;
		Lsize++;
	}
	return cmd;
}

char **parse_cmd(char *cmd)
{
	char **args = NULL;
	char *token = strtok(cmd, " \t\n");
	int arg_count = 0;

	while (token != NULL)
	{
		args = realloc(args, (arg_count + 1) * sizeof(char *));
		if (args == NULL)
		{
			perror("Memory allocation error");
			exit(EXIT_FAILURE);
		}
		args[arg_count] = strdup(token);
		if (args[arg_count] == NULL)
		{
			perror("Memory allocation error");
			exit(EXIT_FAILURE);
		}
		arg_count++;
		token = strtok(NULL, " \t\n");
	}

	args = realloc(args, (arg_count + 1) * sizeof(char *));
	if (args == NULL)
	{
		perror("Memory allocation error");
		exit(EXIT_FAILURE);
	}
	args[arg_count] = NULL;

	return args;
}

int cmd_execution(char **cmd)
{
	pid_t pid;
	int status = 0;

	pid = fork();
	if (pid < 0)
	{
		perror("fork error");
		return -1;
	}
	else if (pid == 0)
	{
		/* This part will handle the child process */
		execvp(cmd[0], cmd);
		perror("Failed to execute");
		exit(EXIT_FAILURE);
	}
	else
	{
		waitpid(pid, &status, 0);
	}
	return status;
}

void free_cmd(char **cmd)
{
	char **ptr = cmd;
	while (*ptr != NULL)
	{
		free(*ptr);
		ptr++;
	}
	free(cmd);
}

void clear_scrn(void)
{
	write(STDOUT_FILENO, "\033[2J\033[H", 7);
}

void exit_shell(int status)
{
	write(STDERR_FILENO, "Exiting...\n", strlen("Exiting...\n"));
	exit(status);
}