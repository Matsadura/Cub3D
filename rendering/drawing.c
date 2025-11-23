/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   drawing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:25:00 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/23 22:12:22 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

/**
 * render_2dmap - Renders the complete 2D mini-map tile by tile.
 * @data: The main data structure.
 */
void	render_2dmap(t_data *data)
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
	center_x = (int)(data->config.player_x * MINIMAP_SCALE);
	center_y = (int)(data->config.player_y * MINIMAP_SCALE);
	draw_circle(data, center_x, center_y);
}

/**
 * draw_direction - Draws a line representing the player's direction.
 * @data: The main data structure.
 */
void	draw_direction(t_data *data)
{
	if (data == NULL)
		return ;
	dda_algo(data);
}

/**
 * normalize_angle - Resets the angle to the range [0, 2*PI].
 * @angle: The angle to normalize.
 */
double  normalize_angle(double angle)
{
    angle = fmod(angle, 2 * PI);
    if (angle < 0)
        angle += 2 * PI;
    return (angle);
}

/**
 * draw_rays_2d - Renders the full field of view (FOV) rays on the 2D map.
 * @data: The main data structure.
 */
void    draw_rays_2d(t_data *data)
{
    int     i;
    double  ray_angle;
	double	line_len;
    
    if (data == NULL)
        return ;
    ray_angle = data->config.player_angle - (FOV / 2);
    i = 0;
    while (i < SCREEN_WIDTH)
    {
        ray_angle = normalize_angle(ray_angle);
		line_len = cast_ray(ray_angle, data);
		data->coord.ray_end_x = (data->config.player_x + cos(ray_angle) * line_len) * MINIMAP_SCALE;
		data->coord.ray_end_y = (data->config.player_y + sin(ray_angle) * line_len) * MINIMAP_SCALE;
		draw_wall(data, line_len, i);
		dda_algo(data);
        ray_angle += FOV / SCREEN_WIDTH;
        i++;
    }
}
