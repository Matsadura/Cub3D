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
	config->no_path = NULL;
	config->so_path = NULL;
	config->we_path = NULL;
	config->ea_path = NULL;
	config->floor_color[0] = -1;
	config->floor_color[1] = -1;
	config->floor_color[2] = -1;
	config->ceiling_color[0] = -1;
	config->ceiling_color[1] = -1;
	config->ceiling_color[2] = -1;
	config->map = NULL;
	config->player_x = -1;
	config->player_y = -1;
	config->player_camera = '\0';
	// config->fd = -1;
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
	if (config.fd < 0)
		return (EXIT_FAILURE);
	config_init(&config);
	config.tmp_lines = read_file(config.fd);
	if (config.tmp_lines == NULL)
	{
		ft_dprintf(2, "Error: Empty file\n");
		if (config.fd >= 0)
			close(config.fd);
		return (EXIT_FAILURE);
	}

	parse_textures_colors(&config);

	/* TMP PRINTS */
	printf("NO Texture Path: %s\n", config.no_path);
	printf("SO Texture Path: %s\n", config.so_path);
	printf("WE Texture Path: %s\n", config.we_path);
	printf("EA Texture Path: %s\n", config.ea_path);
	printf("Floor Color: R=%d, G=%d, B=%d\n", config.floor_color[0],
		config.floor_color[1], config.floor_color[2]);
	printf("Ceiling Color: R=%d, G=%d, B=%d\n", config.ceiling_color[0],
		config.ceiling_color[1], config.ceiling_color[2]);
	/* END TMP PRINTS */

	
	/* Free all */
	free_array(config.tmp_lines);
	free(config.no_path);
	free(config.so_path);
	free(config.we_path);
	free(config.ea_path);
	free_array(config.map);
	close(config.fd);
	return (EXIT_SUCCESS);
}
