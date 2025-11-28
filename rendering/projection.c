/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   projection.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 15:34:58 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/28 15:35:04 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * draw_wall - Calculates wall height and draws a vertical strip for the 3D view.
 * @data: The main data structure.
 * @ray: The corrected distance to the wall.
 * @x: The current screen column (x-coordinate) being rendered.
 */
void	draw_wall(t_data *data, double ray, int x)
{
	int (y), (start), (end);
	double (dist_proj), (wall_height);
	if (data == NULL)
		return ;
	dist_proj = (SCREEN_WIDTH / 2) / tan(FOV / 2);
	wall_height = (TILE_SIZE / ray) * dist_proj;
	if (ray < 0.00001)
		ray = 0.00001;
	start = (SCREEN_HEIGHT / 2) - (wall_height / 2);
	if (start < 0)
		start = 0;
	end = (SCREEN_HEIGHT / 2) + (wall_height / 2);
	if (end >= SCREEN_HEIGHT)
		end = SCREEN_HEIGHT - 1;
	y = start;
	while (y < end)
	{
		my_mlx_pixel_put(data, x, y, RED);
		y++;
	}
	draw_ceiling(data, start, x);
	draw_floor(data, end, x);
}

/**
 * draw_ceiling - Fills the pixels above the wall strip with ceiling color.
 * @data: The main data structure.
 * @wall_start: The y-coordinate where the top of the wall begins.
 * @x: The current screen column.
 */
void	draw_ceiling(t_data *data, int wall_start, int x)
{
	int	start;

	if (data == NULL)
		return ;
	start = 0;
	while (start < wall_start)
	{
		my_mlx_pixel_put(data, x, start, BLUE);
		start++;
	}
}

/**
 * draw_floor - Fills the pixels below the wall strip with floor color.
 * @data: The main data structure.
 * @wall_end: The y-coordinate where the bottom of the wall ends.
 * @x: The current screen column.
 */
void	draw_floor(t_data *data, int wall_end, int x)
{
	if (data == NULL)
		return ;
	while (wall_end < SCREEN_HEIGHT)
	{
		my_mlx_pixel_put(data, x, wall_end, BROWN);
		wall_end++;
	}
}
