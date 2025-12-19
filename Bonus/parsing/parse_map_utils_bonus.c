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

#include "cub3d_bonus.h"

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

/**
 * is_void_inside - Checks if there are voids inside the map
 * @config: The main config structure
 * Returns: 1 if voids are found inside, otherwise 0.
 */
int	is_void_inside(t_config *config)
{
	int		i;
	int		j;
	char	cell;

	i = -1;
	while (config->map[++i] != NULL)
	{
		j = -1;
		while (config->map[i][++j] != '\0')
		{
			cell = config->map[i][j];
			if (cell == '0' || cell == 'N'
				|| cell == 'S' || cell == 'E'
				|| cell == 'W')
			{
				if (get_map_char(config->map, i - 1, j) == ' '
					|| get_map_char(config->map, i + 1, j) == ' '
					|| get_map_char(config->map, i, j - 1) == ' '
					|| get_map_char(config->map, i, j + 1) == ' ')
					return (TRUE);
			}
		}
	}
	return (FALSE);
}

/**
 * is_map_wall_enclosed - Checks if the map is enclosed by walls
 * @config: The main config structure
 * Returns: 1 if the map is wall-enclosed, otherwise 0.
 */
int	is_map_wall_enclosed(char **map)
{
	int	i;
	int	j;

	i = 0;
	while (map[i] != NULL)
	{
		j = 0;
		while (map[i][j] != '\0')
		{
			if (map[i][j] == '0' || map[i][j] == 'N'
				|| map[i][j] == 'S' || map[i][j] == 'E'
				|| map[i][j] == 'W')
			{
				if (i == 0 || j == 0
					|| map[i + 1] == NULL || map[i - 1] == NULL
					|| map[i - 1][j] == '\0' || map[i + 1][j] == '\0'
					|| map[i][j - 1] == '\0' || map[i][j + 1] == '\0')
					return (FALSE);
			}
			j++;
		}
		i++;
	}
	return (TRUE);
}

/**
 * is_player_on_edge - Checks if the player is positioned on the map edge
 * @config: The main config structure
 * Returns: 1 if the player is on the edge, otherwise 0.
 */
int	is_player_on_edge(t_config *config)
{
	if (config->player_x == 0 || config->player_y == 0
		|| config->player_x == config->map_width - 1
		|| config->player_y == config->map_height - 1)
		return (TRUE);
	return (FALSE);
}
