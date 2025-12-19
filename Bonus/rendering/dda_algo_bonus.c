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

#include "cub3d_bonus.h"

/**
 * dda_algo - Draws a line for the minimap FOV using the DDA algorithm.
 * @data: The main data structure.
 */
void	dda_algo(t_data *data)
{
	double (dx), (dy), (step);
	double (x_inc), (y_inc), (x), (y);
	int (i);
	dx = data->coord.ray_end_x - MINIMAP_XCENTER;
	dy = data->coord.ray_end_y - MINIMAP_YCENTER;
	if (fabs(dx) > fabs(dy))
		step = fabs(dx);
	else
		step = fabs(dy);
	x_inc = dx / step;
	y_inc = dy / step;
	i = 0;
	x = MINIMAP_XCENTER;
	y = MINIMAP_YCENTER;
	while (i < step)
	{
		x = x + x_inc;
		y = y + y_inc;
		my_mlx_pixel_put(data, x, y, BLUE);
		i++;
	}
}
