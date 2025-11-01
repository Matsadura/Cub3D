#include "cub3d.h"

static void	free_lines(char **lines, size_t count)
{
	while (count > 0)
	{
		free(lines[count - 1]);
		count--;
	}
	free(lines);
}

static size_t	count_lines(const char *str)
{
	size_t	count;
	size_t	i;

	if (str == NULL || str[0] == '\0')
		return (0);
	count = 0;
	i = 0;
	while (str[i] != '\0')
	{
		count++;
		while (str[i] != '\0' && str[i] != '\n')
			i++;
		if (str[i] == '\n')
			i++;
	}
	return (count);
}

char	**split_lines_preserve_empty(const char *str)
{
	char		**lines;
	size_t		line_count;
	size_t		i;
	size_t		start;
	size_t		len;

	line_count = count_lines(str);
	lines = (char **) malloc((line_count + 1) * sizeof(char *));
	if (lines == NULL)
		return (NULL);
	if (line_count == 0)
	{
		lines[0] = NULL;
		return (lines);
	}
	i = 0;
	start = 0;
	while (str[start] != '\0')
	{
		len = 0;
		while (str[start + len] != '\0' && str[start + len] != '\n')
			len++;
		lines[i] = ft_substr(str, start, len);
		if (lines[i] == NULL)
		{
			free_lines(lines, i);
			return (NULL);
		}
		i++;
		start += len;
		if (str[start] == '\n')
			start++;
	}
	lines[i] = NULL;
	return (lines);
}
