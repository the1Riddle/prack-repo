/**
char **tokenize_input(char *cmd)
{
	char *token = NULL;
	char *delim = " \t\r\n\a";
	int number_tokens = 0;
	int i, s = 0;
	char **argv;

	token = _strtok(cmd, delim);
	while (token != NULL)
	{
		number_tokens++;
		token = _strtok(NULL, delim);
	}

	argv = malloc(sizeof(char *) * (number_tokens + 1));
	if (argv == NULL)
	{
		return (NULL);
	}

	token = _strtok(cmd, delim);
	for (i = 0; token != NULL; i++)
	{
		s = strlen(token);
		argv[i] = malloc(sizeof(char) * (s + 1));
		if (argv[i] == NULL)
		{
			for (; i > 0; i--)
			{
				free(argv[i - 1]);
				free(argv);
				return (NULL);
			}
		}
		strcpy(argv[i], token);
		token = _strtok(NULL, delim);
	}
	argv[i] = NULL;

	return (argv);
}**/