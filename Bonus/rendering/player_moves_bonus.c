/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_moves.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 16:26:35 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/23 21:01:56 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/**
 * game_loop - The main game loop, updates player state and renders a new frame.
 * @data: The main data structure.
 */
int	game_loop(t_data *data)
{
	if (data == NULL)
		return (FALSE);
	rotate_player(data);
	walk_player(data);
	turn_player(data);
	draw_rays_2d(data);
	mlx_put_image_to_window(data->win.mlx_ptr, data->win.win_ptr, data->img.img,
		0, 0);
	return (0);
}

/**
 * rotate_player - Updates the player's view angle based on rotation input.
 * @data: The main data structure.
 */
void	rotate_player(t_data *data)
{
	if (data == NULL)
		return ;
	if (data->mvmnt.rotation_direc != 0)
	{
		data->config.player_angle += data->mvmnt.rotation_direc
			* ROTATION_SPEED;
		data->config.delta_x = cos(data->config.player_angle);
		data->config.delta_y = sin(data->config.player_angle);
	}
}

/**
 * walk_player - Moves the player forward or backward with collision detection.
 * @data: The main data structure.
 */
void	walk_player(t_data *data)
{
	double (tmp_x), (tmp_y);
	if (data == NULL)
		return ;
	if (data->mvmnt.walk_direc != 0)
	{
		tmp_x = (data->config.delta_x * (WALK_SPEED * data->mvmnt.walk_direc))
			+ data->config.player_x;
		validate_move_x(data, tmp_x);
		tmp_y = (data->config.delta_y * (WALK_SPEED * data->mvmnt.walk_direc))
			+ data->config.player_y;
		validate_move_y(data, tmp_y);
	}
}

/**
 * turn_player - Moves the player left or right with collision detection.
 * @data: The main data structure.
 */
void	turn_player(t_data *data)
{
	double (tmp_x), (tmp_y), (teta_x), (teta_y);
	if (data == NULL)
		return ;
	if (data->mvmnt.turn_direc != 0)
	{
		teta_x = cos(data->config.player_angle + (PI / 2));
		teta_y = sin(data->config.player_angle + (PI / 2));
		tmp_x = (teta_x * (WALK_SPEED * data->mvmnt.turn_direc))
			+ data->config.player_x;
		validate_move_x(data, tmp_x);
		tmp_y = (teta_y * (WALK_SPEED * data->mvmnt.turn_direc))
			+ data->config.player_y;
		validate_move_y(data, tmp_y);
	}
}

/**
 * handle_mouse - Rotates the player view based on horizontal mouse movement.
 * @x: The current x-coordinate of the mouse cursor.
 * @y: The current y-coordinate of the mouse cursor (unused).
 * @data: The main data structure.
 */
int	handle_mouse(int x, int y, t_data *data)
{
	static int	old_x = -1;

	if (data == NULL)
		return (FALSE);
	(void)y;
	if (old_x == -1)
	{
		old_x = x;
		return (TRUE);
	}
	if (x > old_x)
		data->config.player_angle += ROTATION_SPEED;
	else if (x < old_x)
		data->config.player_angle -= ROTATION_SPEED;
	data->config.delta_x = cos(data->config.player_angle);
	data->config.delta_y = sin(data->config.player_angle);
	old_x = x;
	return (TRUE);
}
