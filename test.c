#include "shell.h"
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
int cmd_execution(char **cmd)
{
	pid_t pid;
	int status = 0, i = 0;

	pid = fork();

	if (pid == 0)
	{
		/* This part will handle the child process */
		execve(cmd[0], cmd, NULL);
		perror("Failed to execute");
		while (cmd[i] != NULL)
		{
			free(cmd[i]);
			i++;
		}
		free(cmd);
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		/* The fork failed */
		perror("fork error");
		while (cmd[i] != NULL)
		{
			free(cmd[i]);
			i++;
		}
		free(cmd);
		return (-1);
	}
	else
	{
		waitpid(pid, &status, 0);
	}
	return (status);
}

/**
 * exe_with_args - will handle cmds with aguments
 * @cmd: commands
 *
 * Return: the exe status
 */
int exe_with_args(char *cmd)
{
	char **args = NULL;
	int arg_count = 0;
	char *token = _strtok(cmd, " \t\n");
	int execution_status, i;

	while (token != NULL)
	{
		args = realloc(args, (arg_count + 1) * sizeof(char *));
		args[arg_count] = _strdup(token);
		arg_count++;
		token = _strtok(NULL, " \t\n");
	}
	args = realloc(args, (arg_count + 1) * sizeof(char *));
	args[arg_count] = NULL;

	execution_status = cmd_execution(args);

	for (i = 0; i < arg_count; i++)
	{
		free(args[i]);
	}
	free(args);

	if (execution_status < 0)
	{
		write(2, "Failed to execute.\n", strlen("Failed to execute.\n"));
	}

	return (execution_status);
}

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
	static int index, bytesRead, currentBuff = BUFFER_SIZE;
	char c, *newCmd;
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
 * _strtok - short desc
 * @str: what it does
 * @delim: what it does
 *
 * Return: what is returned
 */
char *_strtok(char *str, const char *delim)
{
	static char *buffer;
	size_t token_start = _strspn(buffer, delim);
	size_t token_end = _strcspn(buffer + token_start, delim);

	if (str != NULL)
	{
		buffer = str;
	}
	if (buffer == NULL || *buffer == '\0')
	{
		return (NULL);
	}
	if (buffer[token_start] == '\0')
	{
		buffer = buffer + token_start;
		return (NULL);
	}
	if (buffer[token_start + token_end] != '\0')
	{
		buffer[token_start + token_end] = '\0';
		buffer = buffer + token_start + token_end + 1;
	}
	else
	{
		buffer = buffer + token_start + token_end;
	}
	return (buffer - token_end - token_start);
}

#include "shell.h"
/**
* _getenv - gets the the environment variable value.
* @name: the name of environment var.
*
* Return: NULL if the variable is not found or a pointer to the value
* of the environment var.
*/
char *_getenv(const char *name)
{
	int nameL = _strlen(name);
	int x;

	for (x = 0; environ[x] != NULL; x++)
	{
/* if the value matchs and has = */
		if (_strncmp(environ[x], name, nameL) == 0 && environ[x][nameL] == '=')
		{
			return (&(environ[x][nameL + 1]));
		}
	}
	return (NULL);
}
/**
* printEnv - prints the environment vars.
*
* Return: nothing becouse its a void func
*/
void printEnv(void)
{
	char **env;
	size_t len;

/* iterates through env vars. */
	for (env = environ; *env != NULL; env++)
	{
		len = _strlen(*env);
		write(STDOUT_FILENO, *env, len);
		write(STDOUT_FILENO, "\n", 1);
	}
}
/**
* _setenv - chenges or adds environment variable
* @name: the env var name
* @value: the asigned value to the env var
* @overwrite: used to indicate whether to over write or not
*
* Return: 0 for success and -1 on failure
*/
int _setenv(const char *name, const char *value, int overwrite)
{
	int nameL = _strlen(name);
	int valueL = _strlen(value);
	int x;
	char *newEnt;

	if (name == NULL || value == NULL || _strchr(name, '=') != NULL)
	{
		return (-1);/* this means an error ocured during input */
	}
	for (x = 0; environ[x] != NULL; x++)
	{/*check if the env var exists */
		if (_strncmp(environ[x], name, nameL) == 0 && environ[x][nameL] == '=')
		{
			if (overwrite)/* if it exists then will overwrite it */
			{
				newEnt = malloc(nameL + valueL + 2);
				if (newEnt == NULL)
				{
					return (-1);
				}
				_strcat(newEnt, name);
				_strcat(newEnt, "=");
				_strcat(newEnt, value);
				free(environ[x]);
				environ[x] = newEnt;
			}
			return (0);
		}
	}
	newEnt = malloc(nameL + valueL + 2);
	if (newEnt == NULL)
	{/* and if the env var is not found then a new one is created */
		return (-1);
	}
	_strcat(newEnt, name);
	_strcat(newEnt, "=");
	_strcat(newEnt, value);
	environ[x] = newEnt;
	environ[x + 1] = NULL;
	return (0);
}
/**
* _unsetenv - unsets the environment
* @var: environment var
*
* Return: usually 0 for success
*/
int _unsetenv(char *var)
{
	if (unsetenv(var) != 0)
	{
		perror("unsetenv error");
		return (-1);
	}

	return (0);
}

/**
 * _strspn - The entry point
 * @s: consists entirely of bytes in accept
 * @accept: the initial segment of s
 *
 * Description: the function that gets the length of a prefix substring
 * Return: usually 0
 */
unsigned int _strspn(char *s, const char *accept)
{
	unsigned int y = 0;
	int z = 0;
	int accept_len = 0;

	while (accept[accept_len])
	{
		accept_len++;
	}
	while (*s)
	{
		z = 0;
		while (accept['r'])
		{
			if (*s == accept[z])
			{
				y++;
				break;
			}
			else if (z == accept_len - 1)
			{
				return (y);
			}
			z++;
		}
		s++;
	}
	return (y);
}

/**
 * _strchr - The function that locates a character in a string.
 * @s: is a string
 * @c: the character to be allocated by a pointer on its
 * first occurrence
 *
 * Return: a pointer to the first occurrence of the character c in the string s
 * or NULL if the character is not found
 */
char *_strchr(const char *s, char c)
{
	while (*s != '\0')
	{
		if (*s == c)
		{
			return ((char*)s);
		}
		s++;
	}
	return (NULL);
}

/**
 * clear_scrn - inbuilt to clear screen
 */
void clear_scrn(void)
{
	write(STDOUT_FILENO, "\033[2J\033[H", 7);
}
/**
 * exit_shell - exit inbuild
 * @status: the stats
 */
void exit_shell(int status)
{
	write(2, "Exiting...\n", strlen("Exiting...\n"));
	exit(status);
}

/**
 * _strdup - returns pointer to new mem alloc space which contains copy
 * @strtodup: string to be duplicated
 *
 * Return: a pointer to the new duplicated string
 */
char *_strdup(char *strtodup)
{
	char *copy;

	int len, i;

	if (strtodup == 0)
		return (NULL);

	for (len = 0; strtodup[len]; len++)
		;
	copy = malloc((len + 1) * sizeof(char));

	for (i = 0; i <= len; i++)
		copy[i] = strtodup[i];

	return (copy);
}

/**
 * _strlen - returns the length of a string
 * @s: a pointer to a string
 *
 * Return: length
 */
int _strlen(const char *s)
{
	int lngth = 0;

	if (*s)
	{
		lngth++;
		lngth += _strlen(s + 1);
	}

	return (lngth);
}

/**
 * _strcspn - determine the length of the prefix substring
 * @str1: first string
 * @str2: second string
 *
 * Return: count
 */
size_t _strcspn(const char *str1, const char *str2)
{
	const char *p1, *p2;
	size_t count = 0;

	for (p1 = str1; *p1 != '\0'; p1++)
	{
		for (p2 = str2; *p2 != '\0'; p2++)
		{
			if (*p1 == *p2)
				return (count);
		}
		count++;
	}

	return (count);
}

/**
 * _strcmp - The compare two string values
 * @s1: input value
 * @s2: input value
 *
 * Return: s1[i] - s2[i]
 */
int _strcmp(char *s1, char *s2)
{
	int i;

	i = 0;
	while (s1[i] != '\0' && s2[i] != '\0')
	{
		if (s1[i] != s2[i])
		{
			return (s1[i] - s2[i]);
		}
		i++;
	}
	return (0);
}

/**
 * _strncmp - compares two strings
 * @str1: a pointer to the first str
 * @str2: a pointer to the second str to compare the first one with
 * @n: number of chers to be compared
 *
 * Return: 0 for success
 */
int _strncmp(const char *str1, const char *str2, size_t n)
{
	size_t a;

	for (a = 0; a < n; a++)
	{
		if (str1[a] != str2[a])
		{
			return (str1[a] - str2[a]);
		}

		if (str1[a] == '\0')
		{
			return (0);
		}
	}
	return (0);
}
/**
 * _atoi - a func that converts a string to integer
 * @str: string to convert
 *
 * Return: the conversion
 */
int _atoi(char *str)
{
	int conv = 0, sign = 0, x = 0;

	if (str[0] == '-')
	{
		sign = (-1);
		x++;
	}
	for (; str[x] != '\0'; x++)
	{
		if (str[x] < '0' || str[x] > '9')
		{
			break;
		}
		conv = conv * 10 + (str[x] - '0');
	}

	return (sign * conv);
}
/**
 * _memcpy - a function that copies memory area
 * @dest: The memory area to be copied to
 * @src: The memory area to be copied from
 * @n: The number of bytes
 *
 * Description: The function copies n bytes
 * from memory area src to memory area dest
 * Return: a pointer to dest
 */
char *_memcpy(char *dest, char *src, unsigned int n)
{
	unsigned int a = 0;

	while (a < n)
	{
		(dest[a] = src[a]);
		a++;
	}
	return (dest);
}

/**
 * _strcat - concatenates two strings
 * @dest: input value
 * @src: input value
 *
 * Return: void
 */
char *_strcat(char *dest, const char *src)
{
	int i;
	int j;

	i = 0;
	while (dest[i] != '\0')
	{
		i++;
	}
	j = 0;
	while (src[j] != '\0')
	{
		dest[i] = src[j];
		i++;
		j++;
	}

	dest[i] = '\0';
	return (dest);
}

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
			write(1, "prompt$ ", _strlen("prompt$ "));
		if (_getline(&cmd, stdin) == NULL)
		{
			write(1, "\n", 1);
			break;
		}
		length = _strlen(cmd);
		if (length > 0 && cmd[length - 1] == '\n')
		{
			cmd[length - 1] = '\0';
			length--;
		}
		if (length == 0)
			continue;/* the following part will handle Internal Commands */
		if (_strcmp(cmd, "clear") == 0)/* if clear is typed then */
		{
			clear_scrn();/* will clear the screen terminal */
			continue;
		}
		else if (_strcmp(cmd, "env") == 0)/*prints curent env */
		{
			printEnv();
			continue;
		}
		else if (_strncmp(cmd, "exit", 4) == 0)
		{
			if (_strlen(cmd) > 4)
			{
				e_Status = atoi(cmd + 5);
			}
			exit_shell(e_Status);
			break;
		}
		else
		{
			exe_with_args(cmd);
		}
	}
	free(cmd);
	return (0);
}
