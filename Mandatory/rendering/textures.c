/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   textures.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 01:19:21 by zzaoui            #+#    #+#             */
/*   Updated: 2025/12/12 01:19:23 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * tex_nuller - Initializes a texture structure to default NULL/zero values.
 * @t: Pointer to the texture structure to initialize.
 */
static void	tex_nuller(t_tex *t)
{
	if (t == NULL)
		return ;
	t->img = NULL;
	t->addr = NULL;
	t->w = 0;
	t->h = 0;
	t->bpp = 0;
	t->line_length = 0;
	t->endian = 0;
}

/**
 * load_texture - Loads a texture from an XPM file into a texture structure.
 * @data: The main data structure containing the MLX pointer.
 * @t: Pointer to the texture structure to load the texture into.
 */
static void	load_texture(t_data *data, t_tex *t, char *path)
{
	tex_nuller(t);
	if (data == NULL || data->win.mlx_ptr == NULL || path == NULL)
		cleanup_and_exit(&data->config, data);
	t->img = mlx_xpm_file_to_image(data->win.mlx_ptr, path, &t->w, &t->h);
	if (t->img == NULL || t->w <= 0 || t->h <= 0)
		cleanup_and_exit(&data->config, data);
	t->addr = mlx_get_data_addr(t->img, &t->bpp, &t->line_length, &t->endian);
	if (t->addr == NULL || t->bpp < 24)
		cleanup_and_exit(&data->config, data);
}

/**
 * texture_init - Initializes all textures used in the game.
 * @data: The main data structure containing texture paths and MLX pointer.
 */
void	textures_init(t_data *data)
{
	if (data == NULL)
		return ;
	load_texture(data, &data->tex[NO], data->config.no_path);
	load_texture(data, &data->tex[SO], data->config.so_path);
	load_texture(data, &data->tex[WE], data->config.we_path);
	load_texture(data, &data->tex[EA], data->config.ea_path);
}

/**
 * texture_get_pixel - Retrieves the color of a pixel from a texture.
 * @t: Pointer to the texture structure.
 * @x: The x-coordinate of the pixel within the texture.
 * @y: The y-coordinate of the pixel within the texture.
 * Return: The color of the pixel as an integer.
 */
int	texture_get_pixel(t_tex *t, int x, int y)
{
	char	*px;

	if (t == NULL || t->addr == NULL || x < 0 || y < 0
		|| x >= t->w || y >= t->h)
		return (RED);
	px = t->addr + (y * t->line_length + x * (t->bpp / 8));
	return (*(int *)px);
}

/**
 * choose_wall_texture - Selects the appropriate wall texture based on ray angle and hit side.
 * @data: The main data structure.
 * @ray_angle: The angle of the ray being cast.
 * Return: Pointer to the selected texture structure.
 */
t_tex	*choose_wall_texture(t_data *data, double ray_angle)
{
	if (data->coord.hit_vertical == 1)
	{
		if (is_facing_right(ray_angle))
			return (&data->tex[WE]);
		return (&data->tex[EA]);
	}
	else
	{
		if (is_facing_down(ray_angle))
			return (&data->tex[NO]);
		return (&data->tex[SO]);	
	}
}
