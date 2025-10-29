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

#include "cub3d.h"

/**
 * assign_texture_path - Checks and assigns texture paths
 * @config: The main config structure
 * @line: The current line to check
 * @mask: The mask array to track assigned textures
 * Returns: 1 if a texture was assigned, otherwise 0.
 */
static int	assign_texture_path(t_config *config, char **line, int *mask)
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
static int	assign_color_value(t_config *config, char *line, int *mask)
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
 * parse_textures - Parses the texture paths from the config lines
 * @config: The main config structure
 * Returns: 0 on success, otherwise an error code.
 */
int	parse_textures_colors(t_config *config)
{
	static int	mask[6] = {0, 0, 0, 0, 0, 0};
	int			i;
	char		*line;
	char		**splited_line;

	i = -1;
	while (config->tmp_lines[++i] != NULL)
	{
		if (all_elements_found(mask) == TRUE)
			break ;
		line = ft_strtrim(config->tmp_lines[i], "  ");
		splited_line = ft_split(line, ' ');
		if (is_valid_element(splited_line[0], mask) == FALSE)
		{
			free_array_and_ptr(splited_line, line);
			error_and_exit("Invalid or duplicate element", config);
		}
		if (assign_texture_path(config, splited_line, mask) == FALSE)
		{
			if (assign_color_value(config, line, mask) == FALSE)
			{
				free_array_and_ptr(splited_line, line);
				error_and_exit("Invalid color format", config);
			}
		}
		free_array_and_ptr(splited_line, line);
	}
	if (all_elements_found(mask) == FALSE)
		error_and_exit("Missing required elements", config);
	check_texture_paths(config);
	return (TRUE);
}
