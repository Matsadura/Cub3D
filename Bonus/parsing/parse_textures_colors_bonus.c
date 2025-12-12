/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_textures.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 13:03:04 by zzaoui            #+#    #+#             */
/*   Updated: 2025/10/28 13:03:08 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/**
 * assign_texture_path - Checks and assigns texture paths
 * @config: The main config structure
 * @line: The current line to check
 * @mask: The mask array to track assigned textures
 * Returns: 1 if a texture was assigned, otherwise 0.
 */
int	assign_texture_path(t_config *config, char **line, int *mask)
{
	if (ft_strcmp(line[0], "NO") == 0 && mask[0] == 0)
	{
		config->no_path = ft_strtrim(line[1], " ");
		mask[0] = 1;
		return (TRUE);
	}
	else if (ft_strcmp(line[0], "SO") == 0 && mask[1] == 0)
	{
		config->so_path = ft_strtrim(line[1], " ");
		mask[1] = 1;
		return (TRUE);
	}
	else if (ft_strcmp(line[0], "WE") == 0 && mask[2] == 0)
	{
		config->we_path = ft_strtrim(line[1], " ");
		mask[2] = 1;
		return (TRUE);
	}
	else if (ft_strcmp(line[0], "EA") == 0 && mask[3] == 0)
	{
		config->ea_path = ft_strtrim(line[1], " ");
		mask[3] = 1;
		return (TRUE);
	}
	return (FALSE);
}

/** 
 * create_color_from_str - Parses a color string and fills the color array
 * @color_str: The color string in the format "R,G,B"
 * @color_arr: The array to fill with the parsed color components
 * @config: The main config structure (for error handling)
 * Returns: 1 on success, otherwise 0.
 */
static int	create_color_from_str(char *color_str, int *color_arr)
{
	char	**color_values;
	int		i;
	int		error_flag;

	error_flag = FALSE;
	color_values = ft_split(color_str, ',');
	if (color_values == NULL || arraylen(color_values) != 3)
		error_flag = TRUE;
	i = 0;
	while (error_flag == FALSE && i < 3)
	{
		if (validate_color_component(color_values[i], &color_arr[i]) == FALSE)
			error_flag = TRUE;
		i++;
	}
	if (color_values != NULL)
		free_array(color_values);
	if (error_flag == TRUE)
		return (FALSE);
	return (TRUE);
}

/**
 * assign_color_value - Checks and assigns floor/ceiling colors
 * @config: The main config structure
 * @line: The current line to check
 * @mask: The mask array to track assigned colors
 */
int	assign_color_value(t_config *config, char *line, int *mask)
{
	if (ft_strncmp(line, "F ", 2) == 0 && mask[4] == 0)
	{
		if (create_color_from_str(line + 2, config->floor_color) == FALSE)
			return (FALSE);
		mask[4] = 1;
	}
	else if (ft_strncmp(line, "C ", 2) == 0 && mask[5] == 0)
	{
		if (create_color_from_str(line + 2, config->ceiling_color) == FALSE)
			return (FALSE);
		mask[5] = 1;
	}
	return (TRUE);
}

/**
 * process_line - Processes a single line for texture/color assignment
 * @config: The main config structure
 * @raw_line: The raw line to process
 * @mask: The mask array to track assigned elements
 */
static void	process_line(t_config *config, char *raw_line, int *mask)
{
	char	*trimmed;
	char	**split;

	trimmed = prepare_trimmed_line(raw_line, config);
	if (trimmed == NULL)
		return ;
	split = split_config_line(trimmed, config);
	validate_element_or_exit(split, trimmed, mask, config);
	handle_assignment(config, split, trimmed, mask);
	free_array_and_ptr(split, trimmed);
}

/**
 * parse_textures_colors - Parses texture paths and color values from config
 * @config: The main config structure
 * Returns: 1 on success, otherwise exits on failure.
 */
int	parse_textures_colors(t_config *config)
{
	int			mask[6];
	int			i;
	char		**map_start;

	ft_bzero(mask, sizeof(mask));
	i = 0;
	while (config->tmp_lines[i] != NULL && all_elements_found(mask) == FALSE)
	{
		process_line(config, config->tmp_lines[i], mask);
		i++;
	}
	if (all_elements_found(mask) == FALSE)
		error_and_exit("Missing required elements", config);
	check_texture_paths(config);
	while (config->tmp_lines[i] != NULL
		&& line_is_empty(config->tmp_lines[i]) == TRUE)
		i++;
	map_start = &config->tmp_lines[i];
	if (*map_start == NULL)
		error_and_exit("Map is missing", config);
	validate_map_block(map_start, config);
	config->map = map_start;
	return (TRUE);
}
