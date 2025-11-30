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

#include "cub3d.h"

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
 * draw_rays_2d - Renders the full field of view (FOV) rays on the 2D map.
 * @data: The main data structure.
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
		line_len = cast_ray(ray_angle, data);
		line_len = line_len * cos(ray_angle - data->config.player_angle);
		draw_wall(data, line_len, i);
		ray_angle += FOV / SCREEN_WIDTH;
		i++;
	}
}
