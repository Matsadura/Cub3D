/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   style.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 16:00:52 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/30 16:00:55 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/**
 * rgb_to_hex - Converts RGB color components to a single integer.
 * @color: An array containing the red, green, and blue components.
 * Returns: The combined integer color value.
 */
int	rgb_to_hex(int *color)
{
	return ((color[0] << 16) | (color[1] << 8) | color[2]);
}
