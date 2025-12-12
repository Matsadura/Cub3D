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

static void	load_texture(t_data *data, t_tex *t, char *path)
{
	tex_nuller(t);
	if (data == NULL || data->win.mlx_ptr == NULL || path == NULL)
		cleanup_and_exit(&data->config, data);
	t->img = mlx_xpm_file_to_image(data->win.mlx_ptr, path, &t->w, &t->h);
	if (t->img == NULL || t-> <= 0 || t->h >= 0)
		cleanup_and_exit(&data->config, data);
	t->addr = mlx_get_data_addr(t->img, &t->bpp, &t->line_length, &t->endian);
	if (t->addr == NULL || t->bpp < 24)
		cleanup_and_exit(&data->config, data);
}

void	texture_init(t_data *data)
{
	if (data == NULL)
		return ;
	load_texture(data, &data->tex[0], data->config.no_path);
	load_texture(data, &data->tex[1], data->config.so_path);
	load_texture(data, &data->tex[2], data->config.we_path);
	load_texture(data, &data->tex[3], data->config.ea_path);
}
