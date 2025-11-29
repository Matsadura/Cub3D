/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_textures_colors_utils2.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 13:57:44 by zzaoui            #+#    #+#             */
/*   Updated: 2025/11/01 13:57:46 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * prepare_trimmed_line - Trims whitespace from a raw line
 * @raw_line: The raw line to trim
 * @config: The main config structure
 * Returns: The trimmed line or NULL if empty.
 */
char	*prepare_trimmed_line(char *raw_line, t_config *config)
{
	char	*trimmed;

	trimmed = ft_strtrim(raw_line, "  ");
	if (trimmed == NULL)
		error_and_exit("Failed to allocate memory", config);
	if (trimmed[0] == '\0')
	{
		free(trimmed);
		return (NULL);
	}
	return (trimmed);
}

/**
 * split_config_line - Splits a trimmed line into tokens
 * @trimmed: The trimmed line to split
 * @config: The main config structure
 * Returns: An array of tokens.
 */
char	**split_config_line(char *trimmed, t_config *config)
{
	char	**split;

	split = ft_split(trimmed, ' ');
	if (split == NULL)
	{
		free(trimmed);
		error_and_exit("Failed to parse configuration line", config);
	}
	return (split);
}

/** 
 * validate_element_or_exit - Validates the element or exits on error
 * @split: The split line tokens
 * @trimmed: The trimmed line
 * @mask: The mask array to track assigned elements
 * @config: The main config structure
 */
void	validate_element_or_exit(char **split, char *trimmed, int *mask,
	t_config *config)
{
	if (split[0] == NULL || is_valid_element(split[0], mask) == FALSE)
	{
		free_array_and_ptr(split, trimmed);
		error_and_exit("Bad map position or wrong/duplicate element", config);
	}
}

/**
 * handle_assignment - Handles the assignment of texture paths or colors
 * @config: The main config structure
 * @split: The split line tokens
 * @trimmed: The trimmed line
 * @mask: The mask array to track assigned elements
 */
void	handle_assignment(t_config *config, char **split,
	char *trimmed, int *mask)
{
	if (assign_texture_path(config, split, mask) == FALSE
		&& assign_color_value(config, trimmed, mask) == FALSE)
	{
		free_array_and_ptr(split, trimmed);
		error_and_exit("Invalid color format", config);
	}
}
