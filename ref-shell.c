#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

int prompt_execution(char *prompt);
char *_getline(char **prompt, FILE *stream)
{
    static char buffer[BUFFER_SIZE];
    static int index = 0;
    static int bytesRead = 0;
    static int myBuffer = BUFFER_SIZE;
    char c;
    char *newprompt;
    int the_size = 0;

    if (*prompt == NULL)
    {
        *prompt = malloc(myBuffer * sizeof(char));

        if (*prompt == NULL)
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
            (*prompt)[the_size] = '\0';
            break;
        }

        if (the_size + 1 == myBuffer)
        {
            myBuffer *= 2;
            newprompt = malloc(myBuffer * sizeof(char));
            if (newprompt == NULL)
            {
                perror("Memory allocation error");
                exit(EXIT_FAILURE);
            }
            memcpy(newprompt, *prompt, the_size);
            free(*prompt);
            *prompt = newprompt;
        }
        (*prompt)[the_size] = c;
        the_size++;
    }

    return (*prompt);
}
int prompt_execution(char *prompt)
{
    pid_t pid = fork();
    int status;
    char *args[2];

    args[0] = prompt;
    args[1] = NULL;

    if (pid < 0)
    {
        return -1;
    }
    else if (pid == 0)
    {
        if (execve(prompt, args, NULL) < 0)
        {
            write(2, "Failed to execute.\n", 20);
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
 * exit_shell - exit inbuild
 */
void exit_shell(int status)
{
	exit(status);
}
int main(void)
{
    char *prompt = NULL;
    bool F = false;
    size_t length;
    int StatusForExit = 0;

    while (1 && !F)
    {
        if (isatty(STDIN_FILENO) == 0)
            F = true;
        if (!F)
            write(1, "shell$$$ ", strlen("shell$$$ "));
        if (_getline(&prompt, stdin) == NULL)
        {
            write(1, "\n", 1);
            break;
        }

        length = strlen(prompt);
        if (length > 0 && prompt[length - 1] == '\n')
        {
            prompt[length - 1] = '\0';
            length--;
        }
        if (length == 0)
            continue;
        if (strncmp(prompt, "exit", 4) == 0)
        {
        	if (strlen(prompt) > 4)
        	{
        		StatusForExit = atoi(prompt + 5);
        	}
        	exit_shell(StatusForExit);
        }
        else if (prompt_execution(prompt) < 0)
        {
            write(2, "Failed to execute.\n", 20);
        }
    }

    free(prompt);
    return 0;
}

