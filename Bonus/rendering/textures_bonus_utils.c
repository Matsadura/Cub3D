/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   textures_bonus_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 00:53:50 by zzaoui            #+#    #+#             */
/*   Updated: 2025/12/13 00:53:52 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

void	init_texture_paths(char ***paths)
{
	static char	*path_list[19] = {
		"./textures/gun01.xpm",
		"./textures/gun02.xpm",
		"./textures/gun03.xpm",
		"./textures/gun04.xpm",
		"./textures/gun05.xpm",
		"./textures/gun06.xpm",
		"./textures/gun07.xpm",
		"./textures/gun08.xpm",
		"./textures/gun09.xpm",
		"./textures/gun10.xpm",
		"./textures/gun11.xpm",
		"./textures/gun12.xpm",
		"./textures/gun13.xpm",
		"./textures/gun14.xpm",
		"./textures/gun15.xpm",
		"./textures/gun16.xpm",
		"./textures/gun17.xpm",
		"./textures/gun18.xpm",
		"./textures/gun19.xpm"
	};

	*paths = path_list;
}

/**
 * draw_gun_texture - Draws the gun texture at the bottom center of the screen.
 * @data: The main data structure.
 */
void	draw_gun_texture(t_data *data)
{
	t_tex		*tex;
	static int	i = 4;

	int (x), (y), (screen_x), (screen_y), (key), (color);
	usleep(16000);
	if (i < 4 || i > 22)
		i = 4;
	tex = &data->tex[i++];
	if (!tex || !tex->addr || tex->w <= 0 || tex->h <= 0)
		return ;
	screen_x = (SCREEN_WIDTH - tex->w) / 2;
	screen_y = SCREEN_HEIGHT - tex->h;
	key = texture_get_pixel(tex, 0, 0) & 0x00FFFFFF;
	y = -1;
	while (++y < tex->h)
	{
		x = -1;
		while (++x < tex->w)
		{
			color = texture_get_pixel(tex, x, y);
			if (((color & 0x00FFFFFF) != key) && (color != 0))
				my_mlx_pixel_put(data, screen_x + x, screen_y + y, color);
		}
	}
}
