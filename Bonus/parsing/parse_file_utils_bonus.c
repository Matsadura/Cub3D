/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_file_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 13:34:16 by zzaoui            #+#    #+#             */
/*   Updated: 2025/11/01 13:34:19 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/**
 * free_lines - Frees an array of lines
 * @lines: The array of lines to free
 * @count: The number of lines in the array
 */
static void	free_lines(char **lines, size_t count)
{
	while (count > 0)
	{
		free(lines[count - 1]);
		count--;
	}
	free(lines);
}

/** 
 * count_lines - Counts the number of lines in a string
 * @str: The input string
 * Returns: The number of lines.
 */
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

/** 
 * alloc_lines - Allocates an array for lines
 * @count: The number of lines to allocate
 * Returns: The allocated array or NULL on failure.
 */
static char	**alloc_lines(size_t count)
{
	char	**lines;

	lines = (char **) malloc((count + 1) * sizeof(char *));
	if (lines == NULL)
		return (NULL);
	if (count == 0)
		lines[0] = NULL;
	return (lines);
}

/** 
 * fill_lines - Fills an array with lines from a string
 * @str: The input string
 * @lines: The array to fill with lines
 * Returns: 1 on success, otherwise 0.
 */
static int	fill_lines(const char *str, char **lines)
{
	size_t	start;
	size_t	index;
	size_t	len;

	start = 0;
	index = 0;
	while (str[start] != '\0')
	{
		len = 0;
		while (str[start + len] != '\0' && str[start + len] != '\n')
			len++;
		lines[index] = ft_substr(str, start, len);
		if (lines[index] == NULL)
		{
			free_lines(lines, index);
			return (FALSE);
		}
		index++;
		start += len;
		if (str[start] == '\n')
			start++;
	}
	lines[index] = NULL;
	return (TRUE);
}

/** 
 * split_lines_preserve_empty - Splits a string into lines
 * 		preserving empty lines
 * @str: The input string
 * Returns: An array of lines, or NULL on failure.
 */
char	**split_lines_preserve_empty(const char *str)
{
	char		**lines;
	size_t		line_count;

	line_count = count_lines(str);
	lines = alloc_lines(line_count);
	if (lines == NULL)
		return (NULL);
	if (line_count == 0)
		return (lines);
	if (fill_lines(str, lines) == FALSE)
		return (NULL);
	return (lines);
}
