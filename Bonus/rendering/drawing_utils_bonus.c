/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   drawing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 17:25:07 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/23 14:43:01 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/**
 * my_mlx_pixel_put - Safely draws a pixel to the in-memory image buffer.
 * @data: The main data structure.
 * @x: The x-coordinate (horizontal) of the pixel.
 * @y: The y-coordinate (vertical) of the pixel.
 * @color: The integer value of the color to draw (e.g., 0x00RRGGBB).
 */
void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
		return ;
	dst = data->img.addr + (y * data->img.line_length + x * (data->img.bpp
				/ 8));
	*(unsigned int *)dst = color;
}
