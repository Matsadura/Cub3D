/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   drawing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:25:07 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/08 17:25:09 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * my_mlx_pixel_put - Safely draws a pixel to the in-memory image buffer.
 * @data: The main data structur.
 * @x: The x-coordinate (horizontal) of the pixel.
 * @y: The y-coordinate (vertical) of the pixel.
 * @color: The integer value of the color to draw (e.g., 0x00RRGGBB).
 */
void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || y < 0 || x >= data->config.map_width * 64
		|| y >= data->config.map_height * 64)
		return ;
	dst = data->img.addr + (y * data->img.line_length + x * (data->img.bpp
				/ 8));
	*(unsigned int *)dst = color;
}

/**
 * choose_color - Chooses a color based on the map tile character.
 * @map: The map of the game.
 * @x: The row index to check.
 * @y: The column index to check.
 */
int	choose_color(char **map, int x, int y)
{
	if (map == NULL || *map == NULL)
		return (FALSE);
	if (map[y][x] == '1')
		return (RED);
	else if (map[y][x] == '0' || map[y][x] == 'S' || map[y][x] == 'N'
			|| map[y][x] == 'W' || map[y][x] == 'E')
		return (WHITE);
	else
		return (BLACK);
}

/**
 * color_pixels - Fills an entire map grid tile with a single color.
 * @data: The main data structur.
 * @color: The integer color to fill the tile with.
 * @x: The map grid's x-coordinate (row).
 * @y: The map grid's y-coordinate (column).
 */
void	color_pixels(t_data *data, int color, int x, int y)
{
	int	x_start;
	int	y_start;

	if (data == NULL)
		return ;
	y_start = y * TILE_SIZE;
	while (y_start < (y + 1) * TILE_SIZE)
	{
		x_start = x * TILE_SIZE;
		while (x_start < (x + 1) * TILE_SIZE)
		{
			my_mlx_pixel_put(data, x_start, y_start, color);
			x_start++;
		}
		y_start++;
	}
}

/**
 * draw_circle - Draws a filled black circle on the image buffer.
 * @data: The main data structure.
 * @center_x: The x-coordinate of the circle's center.
 * @center_y: The y-coordinate of the circle's center.
 */
void	draw_circle(t_data *data, int center_x, int center_y)
{
	int	radius;
	int	x;
	int	y;

	if (data == NULL)
		return ;
	radius = 15;
	y = center_y - radius;
	while (y < center_y + radius)
	{
		x = center_x - radius;
		while (x < center_x + radius)
		{
			if ((x - center_x) * (x - center_x) + (y - center_y) * (y
					- center_y) < radius * radius)
				my_mlx_pixel_put(data, x, y, BLACK);
			x++;
		}
		y++;
	}
}
