#include "cub3d.h"

/**
 * line_is_empty - Checks if a line is empty or contains only whitespace
 * @line: The line to check
 * Returns: 1 if the line is empty, otherwise 0.
 */
int	line_is_empty(const char *line)
{
	int		i;

	if (line == NULL)
		return (TRUE);
	i = 0;
	while (line[i] != '\0')
	{
		if (line[i] != ' ' && line[i] != '\t')
			return (FALSE);
		i++;
	}
	return (TRUE);
}

/**
 * validate_map_block - Validates that the map block has no empty lines
 * @lines: The array of lines from the configuration file
 * @config: The main config structure
 */
void	validate_map_block(char **lines, t_config *config)
{
	int		map_started;
	int		i;

	map_started = FALSE;
	i = -1;
	while (lines[++i] != NULL)
	{
		if (line_is_empty(lines[i]) == FALSE)
		{
			map_started = TRUE;
			i++;
			continue ;
		}
		if (map_started == FALSE)
		{
			i++;
			continue ;
		}
		while (lines[i] != NULL && line_is_empty(lines[i]) == TRUE)
			i++;
		if (lines[i] != NULL)
			error_and_exit("Map contains empty line", config);
		break ;
	}
	if (map_started == FALSE)
		error_and_exit("Map is missing", config);
}
