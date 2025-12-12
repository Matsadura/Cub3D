/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_up.c                                            :+:      :+:    :+:*/
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 16:46:17 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/08 16:46:18 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/**
 * cleanup_and_exit - Frees all allocated memory and exits the program cleanly.
 * @config: The config struct containing data to be freed.
 * @data: The main data struct containing MLX components to be destroyed.
 */
void	cleanup_and_exit(t_config *config, t_data *data)
{
	free_array(config->tmp_lines);
	free(config->no_path);
	free(config->so_path);
	free(config->we_path);
	free(config->ea_path);
	if (data->tex[NO].img != NULL && data->win.mlx_ptr != NULL)
		mlx_destroy_image(data->win.mlx_ptr, data->tex[NO].img);
	if (data->tex[SO].img != NULL && data->win.mlx_ptr != NULL)
		mlx_destroy_image(data->win.mlx_ptr, data->tex[SO].img);
	if (data->tex[WE].img != NULL && data->win.mlx_ptr != NULL)
		mlx_destroy_image(data->win.mlx_ptr, data->tex[WE].img);
	if (data->tex[EA].img != NULL && data->win.mlx_ptr != NULL)
		mlx_destroy_image(data->win.mlx_ptr, data->tex[EA].img);
	if (data->win.mlx_ptr != NULL && data->img.img != NULL)
		mlx_destroy_image(data->win.mlx_ptr, data->img.img);
	if (data->win.mlx_ptr != NULL && data->win.win_ptr != NULL)
		mlx_destroy_window(data->win.mlx_ptr, data->win.win_ptr);
	if (data->win.mlx_ptr != NULL)
		mlx_destroy_display(data->win.mlx_ptr);
	if (data->win.mlx_ptr != NULL)
		free(data->win.mlx_ptr);
	if (config->fd >= 0)
		close(config->fd);
	exit(EXIT_SUCCESS);
}
