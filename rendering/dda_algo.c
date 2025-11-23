/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dda_algo.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:57:45 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/23 15:00:59 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

/**
 * dda_algo - Draws a line from the player's position using the DDA algorithm.
 * @data: The main data structure.
 */
void	dda_algo(t_data *data)
{
	double (dx), (dy), (step);
	double (x_inc), (y_inc), (x), (y);
	int (i);
	dx = data->coord.ray_end_x - data->config.player_x * MINIMAP_SCALE;
	dy = data->coord.ray_end_y - data->config.player_y * MINIMAP_SCALE;
	if (fabs(dx) > fabs(dy))
		step = fabs(dx);
	else
		step = fabs(dy);
	x_inc = dx / step;
	y_inc = dy / step;
	i = 0;
	x = data->config.player_x * MINIMAP_SCALE;
	y = data->config.player_y * MINIMAP_SCALE;
	while (i < step)
	{
		x = x + x_inc;
		y = y + y_inc;
		my_mlx_pixel_put(data, x, y, BLACK);
		i++;
	}
}

/**
 * get_coord - Calculates the end-point coordinates for
 * the player's direction line.
 * @data: The main data structure.
 */
// void	get_coord(t_data *data)
// {
// 	if (data == NULL)
// 		return ;
// 	data->coord.x_l = (data->config.delta_x * LINE_LEN) + data->config.player_x;
// 	data->coord.y_l = (data->config.delta_y * LINE_LEN) + data->config.player_y;
// }
