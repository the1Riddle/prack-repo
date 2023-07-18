#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

int cmd_execution(char *cmd);

/**
 * _getline - read line of text from the user's input and use it in a program
 * @cmd: double pointer
 * @stream: the file to read from
 *
 * Return: -1 on failure or when it's an invalid arg.
 */
char *_getline(char **cmd, FILE *stream)
{
    static char buffer[BUFFER_SIZE];
    static int index = 0;
    static int bytesRead = 0;
    static int currentBuff = BUFFER_SIZE;
    char c;
    char *newCmd;
    int Lsize = 0;

    if (*cmd == NULL)
    {
        *cmd = malloc(currentBuff * sizeof(char));

        if (*cmd == NULL)
        {
            perror("Memory allocation error");
            exit(EXIT_FAILURE);
        }
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
            (*cmd)[Lsize] = '\0';
            break;
        }

        if (Lsize + 1 == currentBuff)
        {
            currentBuff *= 2;
            newCmd = malloc(currentBuff * sizeof(char));
            if (newCmd == NULL)
            {
                perror("Memory allocation error");
                exit(EXIT_FAILURE);
            }
            _memcpy(newCmd, *cmd, Lsize);
            free(*cmd);
            *cmd = newCmd;
        }
        (*cmd)[Lsize] = c;
        Lsize++;
    }

    return (*cmd);
}
/**
 * cmd_execution - executs commands specified
 * @cmd: a character pointer of the commands
 *
 * Description: this func executes a command specified user in cmd,
 * The function replaces the current process with new ones specified
 * and arguments.the parent waits for child process to finish
 * but when an error is encountered it exits with 1.
 *
 * Return: -1 when forking fails and the status of chiled process
 */
int cmd_execution(char *cmd)
{
    pid_t pid = fork();
    int status;
    char *args[2];

    args[0] = cmd;
    args[1] = NULL;

    if (pid < 0)
    {
        return -1;
    }
    else if (pid == 0)
    {
        if (execve(cmd, args, NULL) < 0)
        {
            write(2, "Failed to execute.\n", strlen("Failed to execute.\n"));
            exit(1);
        }
    }
    else
    {
        waitpid(pid, &status, 0);
        return status;
    }
    return 0;
}
/**
 * clear_scrn - inbuilt to clear screen
 */
void clear_scrn()
{
	write(STDOUT_FILENO, "\033[2J\033[H", 7);
}
/**
 * exit_shell - exit inbuild
 */
void exit_shell(int status)
{
	exit(status);
}
/**
 * main - the entry point
 *
 * Description: The function reads inputs from the user.
 * The function uses a boolean which is combined by the isatty func in the...
 * while loop to check if the stdout is not in the terminal to make the...
 * prompt$ apear only ones.
 * the func also checks whether their is no input by the user where it will...
 * continue to display the prompt again.
 * it also checks the input by the user and when it finds a space at the end...
 * then it removes it to '\0'.
 *
 * Return: 0 for (success).
 */
int main(void)
{
    char *cmd = NULL;
    bool F = false;
    size_t length;
    int e_Status = 0;

    while (1 && !F)
    {
        if (isatty(STDIN_FILENO) == 0)
            F = true;
        if (!F)
            write(1, "prompt$ ", strlen("prompt$ "));
        if (_getline(&cmd, stdin) == NULL)
        {
            write(1, "\n", 1);
            break;
        }

        length = strlen(cmd);
        if (length > 0 && cmd[length - 1] == '\n')
        {
            cmd[length - 1] = '\0';
            length--;
        }
        if (length == 0)
            continue;
        
        if (_strcmp(cmd, "clear") == 0)/* if clear is typed then */
        	clear_scrn(); /* will clear the screen terminal */
        if (_strcmp(cmd, "env") == 0)/*prints curent env */
        {
            printEnv();
        }

        if (_strncmp(cmd, "exit", 4) == 0)
        {
        	if (strlen(cmd) > 4)
        	{
        		e_Status = atoi(cmd + 5);
        	}
        	write(2, "Exiting...\n", strlen("Exiting...\n"));
        	exit_shell(e_Status);
        }
        else if (cmd_execution(cmd) < 0)
        {
            write(2, "Failed to execute.\n", strlen("Failed to execute.\n"));
        }
    }

    free(cmd);
    return 0;
}

