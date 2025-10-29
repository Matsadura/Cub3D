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
 * parse_map_lines - Parses the map lines from the configuration
 * @config: The main config structure
 * Returns: 1 on success, otherwise exits on failure.
 */
int	parse_map_lines(t_config *config)
{
	int	i;

	i = 0;
	while (config->map[i] != NULL)
	{
		if (is_valid_map_line(config->map[i]) == FALSE)
			error_and_exit("Invalid char in map line", config);
		i++;
	}
	return (TRUE);
}
