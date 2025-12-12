/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   projection_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 01:34:21 by zzaoui            #+#    #+#             */
/*   Updated: 2025/12/12 01:34:22 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/**
 * wall_compute - Computes wall strip parameters for rendering.
 * @ray: The corrected distance to the wall.
 * @w: Pointer to the wall strip structure to populate.
 */
void	wall_compute(double ray, t_wallstrip *w)
{
	double	dist_proj;

	ray = clamp_ray(ray);
	dist_proj = (SCREEN_WIDTH / 2) / tan(FOV / 2);
	w->wall_height = (TILE_SIZE / ray) * dist_proj;
	w->wall_top = (SCREEN_HEIGHT / 2) - (w->wall_height / 2);
	w->start = (int)w->wall_top;
	if (w->start < 0)
		w->start = 0;
	w->end = (SCREEN_HEIGHT / 2) + (w->wall_height / 2);
	if (w->end >= SCREEN_HEIGHT)
		w->end = SCREEN_HEIGHT - 1;
}

/**
 * wall_hit_ratio - Calculates the horizontal position of the wall hit.
 * @data: The main data structure.
 * Return: The ratio (0.0 to 1.0) of the wall hit position.
 */
double	wall_hit_ratio(t_data *data)
{
	double	wallx01;

	if (data->coord.hit_vertical)
		wallx01 = fmod(data->coord.hit_y, TILE_SIZE) / (double)TILE_SIZE;
	else
		wallx01 = fmod(data->coord.hit_x, TILE_SIZE) / (double)TILE_SIZE;
	if (wallx01 < 0)
		wallx01 += 1.0;
	return (wallx01);
}

/**
 * wall_mirror_tex_x - Adjusts texture x-coordinate
 * 		for mirroring based on wall side.
 * @data: The main data structure.
 * @tex: Pointer to the texture structure.
 * @tex_x: The original texture x-coordinate.
 * Return: The adjusted texture x-coordinate.
 */
int	wall_mirror_tex_x(t_data *data, t_tex *tex, int tex_x)
{
	if (data->coord.hit_vertical && is_facing_right(data->coord.ray_angle))
		return (tex->w - tex_x - 1);
	if (!data->coord.hit_vertical && is_facing_up(data->coord.ray_angle))
		return (tex->w - tex_x - 1);
	return (tex_x);
}

/**
 * wall_tex_x - Calculates the texture x-coordinate for the wall hit.
 * @data: The main data structure.
 * @tex: Pointer to the texture structure.
 * Return: The texture x-coordinate.
 */
int	wall_tex_x(t_data *data, t_tex *tex)
{
	double	wallx01;
	int		tex_x;

	wallx01 = wall_hit_ratio(data);
	tex_x = (int)(wallx01 * (double)tex->w);
	tex_x = clamp_int(tex_x, 0, tex->w - 1);
	return (wall_mirror_tex_x(data, tex, tex_x));
}
