/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_map_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 19:35:48 by zzaoui            #+#    #+#             */
/*   Updated: 2025/10/29 19:35:49 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * is_valid_map_line - Checks if a map line contains only valid characters
 * @line: The map line to check
 * Returns: 1 if the line is valid, otherwise 0.
 */
int	is_valid_map_line(char *line)
{
	int	i;

	i = 0;
	while (line[i] != '\0')
	{
		if (line[i] != '0' && line[i] != '1' && line[i] != ' '
			&& line[i] != 'N' && line[i] != 'S'
			&& line[i] != 'E' && line[i] != 'W')
			return (FALSE);
		i++;
	}
	return (TRUE);
}

/**
 * check_player_position - Checks for exactly one player starting position
 * @config: The main config structure
 * Returns: the number of player positions found.
 */
int	check_player_position(t_config *config)
{
	int	i;
	int	j;
	int	player_count;

	player_count = 0;
	i = 0;
	while (config->map[i] != NULL)
	{
		j = 0;
		while (config->map[i][j] != '\0')
		{
			if (config->map[i][j] == 'N' || config->map[i][j] == 'S' ||
				config->map[i][j] == 'E' || config->map[i][j] == 'W')
			{
				config->player_x = j;
				config->player_y = i;
				config->player_camera = config->map[i][j];
				player_count++;
			}
			j++;
		}
		i++;
	}
	return (player_count);
}
