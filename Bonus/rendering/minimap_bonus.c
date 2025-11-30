/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 16:20:05 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/30 16:20:07 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/**
 * minimap_base - Draws the black circular background/border for the minimap.
 * @data: The main data structure.
 */
void	minimap_base(t_data *data)
{
	double	x;
	double	y;

	if (data == NULL)
		return ;
	y = Y_START_POINT - 3;
	while (y < Y_START_POINT + MINIMAP_SIZE + 3)
	{
		x = X_START_POINT - 3;
		while (x < X_START_POINT + MINIMAP_SIZE + 3)
		{
			if (pow((x - MINIMAP_XCENTER), 2) + pow((y - MINIMAP_YCENTER), 2)
				<= pow((MINIMAP_RADIUS + 3), 2))
				my_mlx_pixel_put(data, x, y, BLACK);
			x++;
		}
		y++;
	}
}

/**
 * render_minimap_helper - Determines and draws the
 * color of a single minimap pixel.
 * @data: The main data structure.
 * @x: The screen x-coordinate of the pixel.
 * @y: The screen y-coordinate of the pixel.
 */
void	render_minimap_helper(t_data *data, int x, int y)
{
	int (x_map), (y_map), (color);
	if (data == NULL)
		return ;
	if (pow((x - MINIMAP_XCENTER), 2) + pow((y - MINIMAP_YCENTER), 2)
		<= pow(MINIMAP_RADIUS, 2))
	{
		x_map = (data->config.player_x + ((x - MINIMAP_XCENTER)
					* MINIMAP_SCALE)) / TILE_SIZE;
		y_map = (data->config.player_y + ((y - MINIMAP_YCENTER)
					* MINIMAP_SCALE)) / TILE_SIZE;
		color = WHITE;
		if (y_map >= 0 && y_map < data->config.map_height
			&& x_map >= 0 && x_map < data->config.map_width)
			if (data->config.map[y_map][x_map] == '1')
				color = BLACK;
		my_mlx_pixel_put(data, x, y, color);
	}
}

/**
 * render_minimap - Iterates through the minimap region to draw the map content.
 * @data: The main data structure.
 */
void	render_minimap(t_data *data)
{
	double (x), (y);
	if (data == NULL)
		return ;
	y = Y_START_POINT;
	while (y < Y_START_POINT + MINIMAP_SIZE)
	{
		x = X_START_POINT;
		while (x < X_START_POINT + MINIMAP_SIZE)
		{
			render_minimap_helper(data, x, y);
			x++;
		}
		y++;
	}
}

/**
 * render_player - Draws the player indicator (red circle)
 * in the center of the minimap.
 * @data: The main data structure.
 */
void	render_player(t_data *data)
{
	double	x;
	double	y;
	double	x_center;
	double	y_center;
	int		radius;

	if (data == NULL)
		return ;
	radius = 10;
	y_center = Y_START_POINT + (MINIMAP_SIZE / 2);
	x_center = X_START_POINT + (MINIMAP_SIZE / 2);
	y = y_center - radius;
	while (y < y_center + radius)
	{
		x = x_center - radius;
		while (x < x_center + radius)
		{
			if (pow((x - x_center), 2) + pow((y - y_center), 2)
				< pow(radius, 2))
				my_mlx_pixel_put(data, x, y, RED);
			x++;
		}
		y++;
	}
}
