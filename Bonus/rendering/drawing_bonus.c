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

#include "cub3d_bonus.h"

/**
 * normalize_angle - Resets the angle to the range [0, 2*PI].
 * @angle: The angle to normalize.
 */
double	normalize_angle(double angle)
{
	angle = fmod(angle, 2 * PI);
	if (angle < 0)
		angle += 2 * PI;
	return (angle);
}

/**
 * draw_minimap_fov - Draws the visual FOV cone (blue rays) on the minimap.
 * @data: The main data structure.
 */
void	draw_minimap_fov(t_data *data)
{
	int		i;
	double	ray_angle;

	if (data == NULL)
		return ;
	ray_angle = data->config.player_angle - (FOV / 2);
	i = 0;
	while (i <= 90)
	{
		ray_angle = normalize_angle(ray_angle);
		data->coord.ray_end_x = MINIMAP_XCENTER
			+ cos(ray_angle) * MINIMAP_LINE_LEN;
		data->coord.ray_end_y = MINIMAP_YCENTER
			+ sin(ray_angle) * MINIMAP_LINE_LEN;
		dda_algo(data);
		ray_angle += FOV / 90;
		i++;
	}
}

/**
 * draw_rays_2d - Main render loop: draws 3D walls and overlays the minimap.
 * @data: The main data structure.
 *
 * Description: Casts rays across the screen width. Applies cosine correction
 * to fix fisheye distortion, calls draw_wall() for the 3D view, and finally
 * renders the minimap components on top.
 */
void	draw_rays_2d(t_data *data)
{
	int		i;
	double	ray_angle;
	double	line_len;

	if (data == NULL)
		return ;
	ray_angle = data->config.player_angle - (FOV / 2);
	i = 0;
	while (i < SCREEN_WIDTH)
	{
		ray_angle = normalize_angle(ray_angle);
		data->coord.ray_angle = ray_angle;
		line_len = cast_ray(ray_angle, data);
		line_len = line_len * cos(ray_angle - data->config.player_angle);
		draw_wall(data, line_len, i);
		ray_angle += FOV / SCREEN_WIDTH;
		i++;
	}
	minimap_base(data);
	render_minimap(data);
	draw_minimap_fov(data);
	render_player(data);
}
