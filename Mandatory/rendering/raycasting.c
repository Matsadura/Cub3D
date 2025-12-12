/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 14:36:56 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/21 13:38:01 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * set_h_coord - Calculates the starting coordinates and step
 * size for horizontal checks.
 * @data: The main data structure.
 * @ray_angle: The angle of the current ray being cast.
 */
void	set_h_coord(t_data *data, double ray_angle)
{
	if (data == NULL)
		return ;
	data->coord.y_inter = floor(data->config.player_y / TILE_SIZE) * TILE_SIZE;
	data->coord.y_inter += (is_facing_down(ray_angle) * TILE_SIZE);
	data->coord.x_inter = data->config.player_x
		+(data->coord.y_inter - data->config.player_y) / tan(ray_angle);
	data->coord.y_step = TILE_SIZE;
	if (is_facing_up(ray_angle))
		data->coord.y_step *= -1;
	data->coord.x_step = data->coord.y_step / tan(ray_angle);
	if ((is_facing_left(ray_angle) && data->coord.x_step > 0)
		|| (is_facing_right(ray_angle) && data->coord.x_step < 0))
		data->coord.x_step *= -1;
}

/**
 * h_intersection - Finds the distance to the nearest
 * horizontal wall intersection.
 * @ray_angle: The angle of the current ray.
 * @data: The main data structure.
 */
double	h_intersection(double ray_angle, t_data *data)
{
	double	check_y;

	if (data == NULL)
		return (FALSE);
	set_h_coord(data, ray_angle);
	check_y = data->coord.y_inter;
	if (is_facing_up(ray_angle))
		check_y -= 1;
	data->coord.y_map = (int)check_y / TILE_SIZE;
	data->coord.x_map = (int)data->coord.x_inter / TILE_SIZE;
	while (data->coord.x_map >= 0 && data->coord.x_map < data->config.map_width
		&& data->coord.y_map >= 0 && data->coord.y_map < data->config.map_height
		&& data->config.map[data->coord.y_map][data->coord.x_map] != '1')
	{
		check_y += data->coord.y_step;
		data->coord.y_inter += data->coord.y_step;
		data->coord.x_inter += data->coord.x_step;
		data->coord.y_map = (int)check_y / TILE_SIZE;
		data->coord.x_map = (int)data->coord.x_inter / TILE_SIZE;
	}
	data->coord.h_hit_x = data->coord.x_inter;
	data->coord.h_hit_y = data->coord.y_inter;
	return (calc_distance(data));
}

/**
 * set_v_coord - Calculates the starting coordinates and step
 * size for vertical checks.
 * @data: The main data structure.
 * @ray_angle: The angle of the current ray being cast.
 */
void	set_v_coord(t_data *data, double ray_angle)
{
	if (data == NULL)
		return ;
	data->coord.x_inter = floor(data->config.player_x / TILE_SIZE) * TILE_SIZE;
	data->coord.x_inter += is_facing_right(ray_angle) * TILE_SIZE;
	data->coord.y_inter = data->config.player_y
		+ (data->coord.x_inter - data->config.player_x) * tan(ray_angle);
	data->coord.x_step = TILE_SIZE;
	if (is_facing_left(ray_angle))
		data->coord.x_step *= -1;
	data->coord.y_step = data->coord.x_step * tan(ray_angle);
	if ((is_facing_up(ray_angle) && data->coord.y_step > 0)
		|| (is_facing_down(ray_angle) && data->coord.y_step < 0))
		data->coord.y_step *= -1;
}

/**
 * v_intersection - Finds the distance to the nearest vertical wall intersection.
 * @ray_angle: The angle of the current ray.
 * @data: The main data structure.
 */
double	v_intersection(double ray_angle, t_data *data)
{
	double	check_x;

	if (data == NULL)
		return (FALSE);
	set_v_coord(data, ray_angle);
	check_x = data->coord.x_inter;
	if (is_facing_left(ray_angle))
		check_x -= 1;
	data->coord.y_map = (int)data->coord.y_inter / TILE_SIZE;
	data->coord.x_map = (int)check_x / TILE_SIZE;
	while (data->coord.x_map >= 0 && data->coord.x_map < data->config.map_width
		&& data->coord.y_map >= 0 && data->coord.y_map < data->config.map_height
		&& data->config.map[data->coord.y_map][data->coord.x_map] != '1')
	{
		data->coord.y_inter += data->coord.y_step;
		data->coord.x_inter += data->coord.x_step;
		check_x += data->coord.x_step;
		data->coord.y_map = (int)data->coord.y_inter / TILE_SIZE;
		data->coord.x_map = (int)check_x / TILE_SIZE;
	}
	data->coord.v_hit_x = data->coord.x_inter;
	data->coord.v_hit_y = data->coord.y_inter;
	return (calc_distance(data));
}

/**
 * cast_ray - Calculates the exact distance to the closest wall for a single ray.
 * @ray_angle: The angle of the ray being cast.
 * @data: The main data structure.
 */
double	cast_ray(double ray_angle, t_data *data)
{
	double	h_dis;
	double	v_dis;

	if (data == NULL)
		return (FALSE);
	ray_angle = fix_singular_angles(ray_angle);
	h_dis = h_intersection(ray_angle, data);
	v_dis = v_intersection(ray_angle, data);
	return (set_ray_hit(data, h_dis, v_dis));
}
