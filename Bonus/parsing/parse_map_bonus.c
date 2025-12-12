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

#include "cub3d_bonus.h"

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
 * space_line - Creates a line filled with spaces of given length
 * @length: The length of the line
 * Returns: Pointer to the newly created line, or NULL on failure.
 */
static char	*space_line(int length)
{
	char	*line;
	int		i;

	line = (char *)malloc(sizeof(char) * (length + 1));
	if (line == NULL)
		return (NULL);
	i = 0;
	while (i < length)
	{
		line[i] = ' ';
		i++;
	}
	line[i] = '\0';
	return (line);
}

/**
 * rectangulate_map - Makes all map lines the same length by padding with spaces
 * @config: The main config structure
 */
static void	rectangulate_map(t_config *config)
{
	int		i;
	int		line_length;
	int		space_length;
	char	*spaces;
	char	*joined;

	i = 0;
	while (config->map[i] != NULL)
	{
		line_length = ft_strlen(config->map[i]);
		if (line_length < config->map_width)
		{
			space_length = config->map_width - line_length;
			spaces = space_line(space_length);
			if (spaces == NULL)
				error_and_exit("", config);
			joined = ft_strjoin(config->map[i], spaces);
			free(spaces);
			if (joined == NULL)
				error_and_exit("", config);
			free(config->map[i]);
			config->map[i] = joined;
		}
		i++;
	}
}

/**
 * space_to_wall - Converts all spaces in the map to walls ('1')
 * @config: The main config structure
 */
static void	space_to_cell(t_config *config)
{
	int	i;
	int	j;

	i = 0;
	while (i < config->map_height)
	{
		j = 0;
		while (j < config->map_width)
		{
			if (config->map[i][j] == ' ')
				config->map[i][j] = '0';
			j++;
		}
		i++;
	}
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
	rectangulate_map(config);
	space_to_cell(config);
	return (TRUE);
}
