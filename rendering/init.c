/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 15:42:02 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/13 16:19:20 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

/**
 * data_init - Initializes player position, direction, and movement state.
 * @data: The main data structure.
 */
void	data_init(t_data *data)
{
	data->config.player_x = (data->config.player_x + 0.5) * 64;
	data->config.player_y = (data->config.player_y + 0.5) * 64;
	data->mvmnt.rotation_direc = 0;
	data->mvmnt.turn_direc = 0;
	data->mvmnt.walk_direc = 0;
	init_player_direction(data);
}

/**
 * init_player_direction - Sets the player's initial angle and direction vector.
 * @data: The main data structure
 */
void	init_player_direction(t_data *data)
{
	if (data->config.player_camera == 'E')
		data->config.player_angle = 0;
	else if (data->config.player_camera == 'S')
		data->config.player_angle = PI / 2;
	else if (data->config.player_camera == 'W')
		data->config.player_angle = PI;
	else if (data->config.player_camera == 'N')
		data->config.player_angle = (PI / 2) * 3;
	data->config.delta_x = cos(data->config.player_angle);
	data->config.delta_y = sin(data->config.player_angle);
	data->config.plane_x = -sin(data->config.player_angle) * 0.66;
	data->config.plane_y = cos(data->config.player_angle) * 0.66;
}
