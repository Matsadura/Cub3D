/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 15:41:52 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/08 15:41:55 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * window - Main application function.
 * @data: The main data structure
 */
void	window(t_data *data)
{
    //data_init(data);
	setup_window(data);
	setup_image(data);
    render_2Dmap(data);
	draw_direction(data);
	draw_player(data);
	mlx_put_image_to_window(data->win.mlx_ptr, data->win.win_ptr, data->img.img, 0, 0);
	mlx_loop(data->win.mlx_ptr);
}

/**
 * setup_window - Initializes the core MLX connection and creates a new window.
 * @data: The main data structure
 */
void    setup_window(t_data *data)
{
    if (data == NULL)
        return ;
    data->win.mlx_ptr = mlx_init();
	if (data->win.mlx_ptr == NULL)
		error_and_exit("Mlx failure", &data->config);
	data->win.win_ptr = mlx_new_window(data->win.mlx_ptr, data->config.map_width * 64,
						data->config.map_height * 64, "CUB3D");
	if (data->win.win_ptr == NULL)
	    error_and_exit("Mlx failure", &data->config);
}

/**
 * setup_image - Creates a new MLX image buffer and gets its data address.
 * @data: The main data structure
 */
void    setup_image(t_data *data)
{
    if (data == NULL)
        return ;
    data->img.img = mlx_new_image(data->win.mlx_ptr, data->config.map_width * 64,
						data->config.map_height * 64);
	data->img.addr = mlx_get_data_addr(data->img.img, &data->img.bpp,
						&data->img.line_length, &data->img.endian);
}