/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting_utils2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 02:56:34 by zzaoui            #+#    #+#             */
/*   Updated: 2025/12/12 02:56:36 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * fix_singular_angles - Adjusts ray angles that are exactly
 * horizontal or vertical to avoid computational issues.
 * @ray_angle: The original ray angle in radians.
 * Return: The adjusted ray angle.
 */
double	fix_singular_angles(double ray_angle)
{
	const double	eps = 0.000001;

	if (fabs(ray_angle) < eps || fabs(ray_angle - PI) < eps
		|| fabs(ray_angle - (PI / 2)) < eps
		|| fabs(ray_angle - (3 * PI / 2)) < eps)
		ray_angle += eps;
	return (ray_angle);
}

/**
 * set_ray_hit - Determines which wall intersection (horizontal or vertical)
 * is closer and sets the hit coordinates accordingly.
 * @data: The main data structure.
 * @h_dis: The distance to the horizontal wall intersection.
 * @v_dis: The distance to the vertical wall intersection.
 * Return: The distance to the closest wall intersection.
 */
double	set_ray_hit(t_data *data, double h_dis, double v_dis)
{
	if (h_dis < v_dis)
	{
		data->coord.hit_vertical = 0;
		data->coord.hit_x = data->coord.h_hit_x;
		data->coord.hit_y = data->coord.h_hit_y;
		return (h_dis);
	}
	data->coord.hit_vertical = 1;
	data->coord.hit_x = data->coord.v_hit_x;
	data->coord.hit_y = data->coord.v_hit_y;
	return (v_dis);
}
