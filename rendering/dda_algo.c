/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dda_algo.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:57:45 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/08 17:57:46 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * dda_algo - Draws a line from the player's position using the DDA algorithm.
 * @data: The main data structure.
 */
void	dda_algo(t_data *data)
{
	double	(dx), (dy), (step);
	double	(x_inc), (y_inc), (x), (y);
	int		i;

	dx = data->coord.x_l - data->config.player_x;
	dy = data->coord.y_l - data->config.player_y;
	if (fabs(dx) > fabs(dy))
		step = fabs(dx);
	else
		step = fabs(dy);
	x_inc = dx / step;
	y_inc = dy / step;
	i = 0;
	x = data->config.player_x;
	y = data->config.player_y;
	while (i < step)
	{
		x = x + x_inc;
		y = y + y_inc;
		my_mlx_pixel_put(data, x, y, RED);
		i++;
	}
}

/**
 * get_coord - Calculates the end-point coordinates for the player's direction line.
 * @data: The main data structure.
 */
void	get_coord(t_data *data)
{

	if (data == NULL)
		return ;
	data->coord.x_l = (data->config.delta_x * LINE_LEN) + data->config.player_x;
	data->coord.y_l = (data->config.delta_y * LINE_LEN) + data->config.player_y;
}
