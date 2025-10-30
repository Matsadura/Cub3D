/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 12:42:35 by zzaoui            #+#    #+#             */
/*   Updated: 2025/10/22 12:42:40 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	config_init(t_config *config)
{
	config->tmp_lines = NULL;
	config->no_path = NULL;
	config->so_path = NULL;
	config->we_path = NULL;
	config->ea_path = NULL;
	config->ceiling_color[0] = -1;
	config->ceiling_color[1] = -1;
	config->ceiling_color[2] = -1;
	config->floor_color[0] = -1;
	config->floor_color[1] = -1;
	config->floor_color[2] = -1;
	config->map = NULL;
	config->map_width = -1;
	config->map_height = -1;
	config->player_x = -1;
	config->player_y = -1;
	config->player_camera = '\0';
}

/**
 * 
 */
static void	cleanup(t_config *config)
{
	free_array(config->tmp_lines);
	free(config->no_path);
	free(config->so_path);
	free(config->we_path);
	free(config->ea_path);
	close(config->fd);
}

/**
 * Main entry point of the program
 * Returns: 0 on success otherwise 1.
 */
int	main(int ac, char **av)
{
	t_config	config;

	if (ac != 2)
		return (EXIT_FAILURE);
	config.fd = open_file(av[1]);
	config_init(&config);
	read_file(config.fd, &config);
	parse_textures_colors(&config);
	parse_map_lines(&config);
	print_array(config.map);
	cleanup(&config);
	return (EXIT_SUCCESS);
}
