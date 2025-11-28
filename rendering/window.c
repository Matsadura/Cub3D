/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 15:41:52 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/23 21:01:49 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * window - Main application function.
 * @data: The main data structure
 */
void	window(t_data *data)
{
	setup_window(data);
	setup_image(data);
	draw_rays_2d(data);
	mlx_put_image_to_window(data->win.mlx_ptr, data->win.win_ptr, data->img.img,
		0, 0);
	mlx_hook(data->win.win_ptr, 2, 1L << 0, handle_key, data);
	mlx_hook(data->win.win_ptr, 3, 1L << 1, handle_key_release, data);
	mlx_loop_hook(data->win.mlx_ptr, game_loop, data);
	mlx_loop(data->win.mlx_ptr);
}

/**
 * setup_window - Initializes the core MLX connection and creates a new window.
 * @data: The main data structure
 */
void	setup_window(t_data *data)
{
	if (data == NULL)
		return ;
	data->win.mlx_ptr = mlx_init();
	if (data->win.mlx_ptr == NULL)
		error_and_exit("Mlx failure", &data->config);
	data->win.win_ptr = mlx_new_window(data->win.mlx_ptr, SCREEN_WIDTH,
			SCREEN_HEIGHT, "CUB3D");
	if (data->win.win_ptr == NULL)
		error_and_exit("Mlx failure", &data->config);
}

/**
 * setup_image - Creates a new MLX image buffer and gets its data address.
 * @data: The main data structure
 */
void	setup_image(t_data *data)
{
	if (data == NULL)
		return ;
	data->img.img = mlx_new_image(data->win.mlx_ptr, SCREEN_WIDTH,
			SCREEN_HEIGHT);
	if (data->img.img == NULL)
		error_and_exit("Mlx failure", &data->config);
	data->img.addr = mlx_get_data_addr(data->img.img, &data->img.bpp,
			&data->img.line_length, &data->img.endian);
	if (data->img.addr == NULL)
		error_and_exit("Mlx failure", &data->config);
}
