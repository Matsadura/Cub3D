/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   projection_utils2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 02:38:15 by zzaoui            #+#    #+#             */
/*   Updated: 2025/12/12 02:38:17 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * clamp_int - Clamps an integer value between a minimum and maximum.
 * @v: The integer value to clamp.
 * @min: The minimum allowable value.
 * @max: The maximum allowable value.
 * Return: The clamped integer value.
 */
int	clamp_int(int v, int min, int max)
{
	if (v < min)
		return (min);
	if (v > max)
		return (max);
	return (v);
}

/**
 * clamp_ray - Ensures the ray distance is not too close to zero.
 * @ray: The ray distance to clamp.
 * Return: The clamped ray distance.
 */
double	clamp_ray(double ray)
{
	if (ray < 0.00001)
		return (0.00001);
	return (ray);
}
