/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_textures_colors_utils.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 14:11:20 by zzaoui            #+#    #+#             */
/*   Updated: 2025/10/28 14:11:21 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * all_elements_found - Checks if all required elements have been found
 * @mask: The mask array to track assigned elements
 * Returns: 1 if all elements are found, otherwise 0.
 */
int	all_elements_found(int *mask)
{
	int	i;

	i = 0;
	while (i < 6)
	{
		if (mask[i] == 0)
			return (FALSE);
		i++;
	}
	return (TRUE);
}

/**
 * is_valid_element - Checks if the line corresponds to a valid element
 * @line: The current line to check
 * @mask: The mask array to track assigned elements
 * Returns: 1 if the element is valid and not yet assigned, otherwise 0.
 */
int	is_valid_element(char *line, int *mask)
{
	int		is_valid;

	is_valid = FALSE;
	if ((ft_strcmp(line, "NO") == 0 && mask[0] == 0)
		|| (ft_strcmp(line, "SO") == 0 && mask[1] == 0)
		|| (ft_strcmp(line, "WE") == 0 && mask[2] == 0)
		|| (ft_strcmp(line, "EA") == 0 && mask[3] == 0)
		|| (ft_strcmp(line, "F") == 0 && mask[4] == 0)
		|| (ft_strcmp(line, "C") == 0 && mask[5] == 0))
		is_valid = TRUE;
	return (is_valid);
}

/**
 * check_texture_paths - Validates the texture file extensions
 * @config: The main config structure
 */
void	check_texture_paths(t_config *config)
{
	if (is_file_ext(config->no_path, ".xpm") == FALSE
		|| is_file_ext(config->so_path, ".xpm") == FALSE
		|| is_file_ext(config->we_path, ".xpm") == FALSE
		|| is_file_ext(config->ea_path, ".xpm") == FALSE)
	{
		error_and_exit("One or more texture paths have invalid file extensions", config);
	}
}

/**
 * validate_color_component - Validates and assigns a color component
 * @value: The string value of the color component
 * @component: Pointer to store the validated color component
 * Returns: 1 if valid, otherwise 0.
 */
int	validate_color_component(char *value, int *component)
{
    char	*trimmed;
    int		number;
    int		is_valid;

    is_valid = FALSE;
    trimmed = ft_strtrim(value, " ");
    if (trimmed != NULL && ft_isnumber(trimmed) == TRUE)
    {
        number = ft_atoi(trimmed);
        if (number >= 0 && number <= 255)
        {
            *component = number;
            is_valid = TRUE;
        }
    }
    free(trimmed);
    return (is_valid);
}
