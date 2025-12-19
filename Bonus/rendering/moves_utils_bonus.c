/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   moves_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 16:24:58 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/12 16:25:02 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/**
 * add_padding - Sets a collision padding value based on player direction.
 * @target: The target coordinate (e.g., player's next move).
 * @player_coord: The player's current coordinate.
 * @padding: A pointer to the padding variable to be set.
 */
void	add_padding(double target, double player_coord, double *padding)
{
	if (target > player_coord)
		*padding = COLLISION_PADDING;
	else
		*padding = -COLLISION_PADDING;
}

/**
 * handle_key - Handles key press events for player movement and exit.
 * @keycode: The integer code of the key that was pressed.
 * @data: The main data structure.
 */
int	handle_key(int keycode, t_data *data)
{
	if (data == NULL)
		return (FALSE);
	if (keycode == ESC)
		cleanup_and_exit(&data->config, data);
	else if (keycode == LEFT_ARROW)
		data->mvmnt.rotation_direc = -1;
	else if (keycode == RIGHT_ARROW)
		data->mvmnt.rotation_direc = 1;
	else if (keycode == BACKWARD)
		data->mvmnt.walk_direc = -1;
	else if (keycode == FORWARD)
		data->mvmnt.walk_direc = 1;
	else if (keycode == LEFT)
		data->mvmnt.turn_direc = -1;
	else if (keycode == RIGHT)
		data->mvmnt.turn_direc = 1;
	return (TRUE);
}

/**
 * handle_key_release - Handles key release events to stop player movement.
 * @keycode: The integer code of the key that was released.
 * @data: The main data structure.
 */
int	handle_key_release(int keycode, t_data *data)
{
	if (data == NULL)
		return (FALSE);
	if (keycode == LEFT_ARROW || keycode == RIGHT_ARROW)
		data->mvmnt.rotation_direc = 0;
	if (keycode == BACKWARD || keycode == FORWARD)
		data->mvmnt.walk_direc = 0;
	if (keycode == LEFT || keycode == RIGHT)
		data->mvmnt.turn_direc = 0;
	return (TRUE);
}

/**
 * validate_move_x
	- Checks for collisions and updates the player's X coordinate.
 * @data: The main data structure.
 * @new_x: The potential new X coordinate to move to.
 */
void	validate_move_x(t_data *data, double new_x)
{
	double	padding;
	int		map_x;
	int		map_y;

	if (data == NULL)
		return ;
	add_padding(new_x, data->config.player_x, &padding);
	map_x = (int)(new_x + padding) / TILE_SIZE;
	map_y = (int)data->config.player_y / TILE_SIZE;
	if (map_x >= 0 && map_x < data->config.map_width && map_y >= 0
		&& map_y < data->config.map_height
		&& data->config.map[map_y][map_x] != '1'
		&& data->config.map[map_y][map_x] != ' ')
		data->config.player_x = new_x;
}

/**
 * validate_move_y
	- Checks for collisions and updates the player's Y coordinate.
 * @data: The main data structure.
 * @new_y: The potential new Y coordinate to move to.
 */
void	validate_move_y(t_data *data, double new_y)
{
	double	padding;
	int		map_x;
	int		map_y;

	if (data == NULL)
		return ;
	add_padding(new_y, data->config.player_y, &padding);
	map_x = (int)data->config.player_x / TILE_SIZE;
	map_y = (int)(new_y + padding) / TILE_SIZE;
	if (map_x >= 0 && map_x < data->config.map_width
		&& map_y >= 0 && map_y < data->config.map_height
		&& data->config.map[map_y][map_x] != '1'
		&& data->config.map[map_y][map_x] != ' ')
		data->config.player_y = new_y;
}
