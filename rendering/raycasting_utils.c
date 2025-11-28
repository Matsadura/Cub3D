/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 13:11:02 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/21 13:32:01 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * calc_distance - Calculates the Euclidean distance between
 * player and wall hit.
 * @data: The main data structure containing player and
 * intersection coordinates.
 */
double	calc_distance(t_data *data)
{
	double	dist;

	if (data == NULL)
		return (FALSE);
	dist = sqrt(pow(data->coord.x_inter - data->config.player_x, 2)
			+ pow(data->coord.y_inter - data->config.player_y, 2));
	return (dist);
}

/**
 * is_facing_down - Checks if the ray angle is pointing downwards (0 to PI).
 * @angle: The current ray angle in radians.
 */
int	is_facing_down(double angle)
{
	return (angle > 0 && angle < PI);
}

/**
 * is_facing_up - Checks if the ray angle is pointing upwards.
 * @angle: The current ray angle in radians.
 */
int	is_facing_up(double angle)
{
	return (!is_facing_down(angle));
}

/**
 * is_facing_left - Checks if the ray angle is pointing left (PI/2 to 3PI/2).
 * @angle: The current ray angle in radians.
 */
int	is_facing_left(double angle)
{
	return (angle > (PI / 2) && angle < (3 * PI / 2));
}

/**
 * is_facing_right - Checks if the ray angle is pointing right.
 * @angle: The current ray angle in radians.
 */
int	is_facing_right(double angle)
{
	return (!is_facing_left(angle));
}
