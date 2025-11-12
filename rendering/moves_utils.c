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

#include "cub3d.h"

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
