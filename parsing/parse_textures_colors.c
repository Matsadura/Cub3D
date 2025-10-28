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
		printf("NO texture path assigned: %s\n", config->no_path);
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
 * create_color_from_string - Parses color values from a string
 * @config: The main config structure
 * @color_string: The string containing color values
 */
static void	create_color_from_string(t_config *config, char *color_string)
{
	char	**color_values;
	int		i;
	char	*trimmed_string;

	color_values = ft_split(color_string, ',');
	printf("Color string: '%s'\n", color_string);

	print_array(color_values);

	if (color_values != NULL && arraylen(color_values) == 3)
	{
		printf("Parsing color from string: '%s'\n", color_string);
		i = 0;
		while (i < 3)
		{
			trimmed_string = ft_strtrim(color_values[i], " ");

			printf("Color value %d: '%s'\n", i, trimmed_string);

			if (ft_isnumber(trimmed_string) == TRUE)
			{
				if (ft_atoi(trimmed_string) >= 0
					&& ft_atoi(trimmed_string) <= 255)
					config->floor_color[i] = ft_atoi(trimmed_string);
			}
			else
				config->floor_color[i] = -1; /* FLAG TO PRINT ERROR MESSAGE */
			i++;
			free(trimmed_string);
		}
	}
	free_array(color_values);
}

/**
 * assign_color_value - Checks and assigns floor/ceiling colors
 * @config: The main config structure
 * @line: The current line to check
 * @mask: The mask array to track assigned colors
 */
static void	assign_color_value(t_config *config, char **line, int *mask)
{
	if (ft_strcmp(line[0], "F") == 0 && mask[4] == 0)
	{
		create_color_from_string(config, line[1]);
		mask[4] = 1;
	}
	else if (ft_strcmp(line[0], "C") == 0 && mask[5] == 0)
	{
		create_color_from_string(config, line[1]);
		mask[5] = 1;
	}
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

	i = 0;
	while (config->tmp_lines[i] != NULL)
	{
		if (all_elements_found(mask) == TRUE)
			break ;
		line = ft_strtrim(config->tmp_lines[i], "  ");
		splited_line = ft_split(line, ' ');

		if (is_valid_element(splited_line[0], mask) == FALSE)
		{
			printf(" IAM HERE\n");
			free(line);
			free_array(splited_line);
			break ;
		}
		printf(" IAM HERE 2\n");
		if (assign_texture_path(config, splited_line, mask) == FALSE)
			assign_color_value(config, splited_line, mask);
		free_array(splited_line);
		free(line);
		i++;
	}
	printf("I am here 3\n");
	if (all_elements_found(mask) == FALSE)
		return (ft_dprintf(2, "Error: Missing required elements\n"), FALSE);
	return (TRUE);
}
