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
	char	**splited_line;

	splited_line = ft_split(line, ' ');

	int	i;
	while (splited_line[i] != NULL)
	{
		printf("splited_line[%d]: '%s'\n", i, splited_line[i]);
		i++;
	}

	if ((ft_strcmp(splited_line[0], "NO") == 0 && mask[0] == 0)
		|| (ft_strcmp(splited_line[0], "SO") == 0 && mask[1] == 0)
		|| (ft_strcmp(splited_line[0], "WE") == 0 && mask[2] == 0)
		|| (ft_strcmp(splited_line[0], "EA") == 0 && mask[3] == 0)
		|| (ft_strcmp(splited_line[0], "F") == 0 && mask[4] == 0)
		|| (ft_strcmp(splited_line[0], "C") == 0 && mask[5] == 0))
	{
		free_array(splited_line);
		return (TRUE);
	}
	free_array(splited_line);
	return (FALSE);
}
