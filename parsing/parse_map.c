/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 18:54:52 by zzaoui            #+#    #+#             */
/*   Updated: 2025/10/29 18:54:53 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * init_map_w_h - Initializes the map width and height in the config
 * @config: The main config structure
 */
static void	init_map_w_h(t_config *config)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (config->map[i] != NULL)
	{
		if ((int)ft_strlen(config->map[i]) > j)
			j = ft_strlen(config->map[i]);
		i++;
	}
	config->map_width = j;
	config->map_height = i;
}

/**
 * parse_map_lines - Parses the map lines from the configuration
 * @config: The main config structure
 * Returns: 1 on success, otherwise exits on failure.
 */
int	parse_map_lines(t_config *config)
{
	int	i;

	init_map_w_h(config);
	i = 0;
	while (config->map[i] != NULL)
	{
		if (is_valid_map_line(config->map[i]) == FALSE)
			error_and_exit("Invalid char in map line", config);
		if (check_player_position(config) != 1)
			error_and_exit("Player pos not found in map or duplicate", config);
		if (is_void_inside(config) == TRUE)
			error_and_exit("Map have a void inside or player on edge", config);
		if (is_player_on_edge(config) == TRUE)
			error_and_exit("Player can't be on the edge of the map", config);
		if (is_map_wall_enclosed(config->map) == FALSE)
			error_and_exit("Map is not wall enclosed", config);
		i++;
	}
	return (TRUE);
}
