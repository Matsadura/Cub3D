/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   drawing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:25:00 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/08 17:25:02 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * render_2Dmap - Renders the complete 2D mini-map tile by tile.
 * @data: The main data structure.
 */
void    render_2Dmap(t_data *data)
{
	int	x_weight;
	int	y_height;

	if (data == NULL)
		return ;
	y_height = 0;
	while (y_height < data->config.map_height)
	{
		x_weight = 0;
		while (x_weight < data->config.map_width)
		{
			color_pixels(data, choose_color(data->config.map,
            x_weight, y_height), x_weight, y_height);
			x_weight++;
		}
		y_height++;
	}
}

/**
 * draw_player - Draws the player's position on the 2D map as a circle.
 * @data: The main data structure.
 */
void	draw_player(t_data *data)
{
	int	center_x;
	int	center_y;

	if (data == NULL)
		return ;
	center_x = (data->config.player_x);
	center_y = (data->config.player_y);
	draw_circle(data, center_x, center_y);
	return ;
}

/**
 * draw_direction - Draws a line representing the player's direction.
 * @data: The main data structure.
 */
void	draw_direction(t_data *data)
{
	if (data == NULL)
		return ;
	get_coord(data);
	dda_algo(data);
}
