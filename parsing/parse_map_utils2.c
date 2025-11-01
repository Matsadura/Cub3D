/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_map_utils2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 13:34:06 by zzaoui            #+#    #+#             */
/*   Updated: 2025/11/01 13:34:09 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	int		empty_after_start;
	int		i;

	map_started = FALSE;
	empty_after_start = FALSE;
	i = 0;
	while (lines[i] != NULL)
	{
		if (line_is_empty(lines[i]) == TRUE)
		{
			if (map_started == TRUE)
				empty_after_start = TRUE;
		}
		else
		{
			if (empty_after_start == TRUE)
				error_and_exit("Map contains empty line", config);
			map_started = TRUE;
		}
		i++;
	}
	if (map_started == FALSE)
		error_and_exit("Map is missing", config);
}
