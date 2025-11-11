#include "cub3d.h"

int	handle_key(int keycode, t_data *data)
{
	if (data == NULL)
		return (FALSE);
	if (keycode == LEFT_ARROW)
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

int	game_loop(t_data *data)
{
	if (data == NULL)
		return (FALSE);
	rotate_player(data);
	walk_player(data);
	turn_player(data);
    render_2Dmap(data);
    draw_direction(data);
    draw_player(data);
    mlx_put_image_to_window(data->win.mlx_ptr, data->win.win_ptr, data->img.img, 0, 0);
    return (0);
}

void	rotate_player(t_data *data)
{
	if (data == NULL)
		return ;
	if (data->mvmnt.rotation_direc != 0)
    {
        data->config.player_angle += data->mvmnt.rotation_direc * ROTATION_SPEED;
        data->config.delta_x = cos(data->config.player_angle);
        data->config.delta_y = sin(data->config.player_angle);
    }
}

void	walk_player(t_data *data)
{
	double	(tmp_x), (tmp_y), (x_padding), (y_padding);
	int		(map_check_x), (map_check_y);
	if (data == NULL)
		return ;
	if (data->mvmnt.walk_direc != 0)
	{
		tmp_x = (data->config.delta_x * (WALK_SPEED * data->mvmnt.walk_direc)) + data->config.player_x;
		tmp_y = (data->config.delta_y * (WALK_SPEED * data->mvmnt.walk_direc)) + data->config.player_y;
		add_padding(tmp_x, data->config.player_x, &x_padding);
		map_check_x = (int)(tmp_x + x_padding) / TILE_SIZE;
		map_check_y = (int)data->config.player_y / TILE_SIZE;
		if (data->config.map[map_check_y][map_check_x] != '1' &&
        	data->config.map[map_check_y][map_check_x] != ' ')
			data->config.player_x = tmp_x;
		add_padding(tmp_y, data->config.player_y, &y_padding);
		map_check_x = (int)data->config.player_x / TILE_SIZE;
		map_check_y = (int)(tmp_y + y_padding) / TILE_SIZE;
		if (data->config.map[map_check_y][map_check_x] != '1' &&
        	data->config.map[map_check_y][map_check_x] != ' ')
			data->config.player_y = tmp_y;
	}
}

void	turn_player(t_data *data)
{
	double	(tmp_x), (tmp_y), (x_padding), (y_padding), (teta_x), (teta_y);
	int		(map_check_x), (map_check_y);
	if (data == NULL)
		return ;
	if (data->mvmnt.turn_direc != 0)
	{
		teta_x = cos(data->config.player_angle + (PI / 2));
		teta_y = sin(data->config.player_angle + (PI / 2));
		tmp_x = (teta_x * (WALK_SPEED * data->mvmnt.turn_direc)) + data->config.player_x;
		tmp_y = (teta_y * (WALK_SPEED * data->mvmnt.turn_direc)) + data->config.player_y;
		add_padding(tmp_x, data->config.player_x, &x_padding);
		map_check_x = (int)(tmp_x + x_padding) / TILE_SIZE;
		map_check_y = (int)data->config.player_y / TILE_SIZE;
		if (data->config.map[map_check_y][map_check_x] != '1' &&
        	data->config.map[map_check_y][map_check_x] != ' ')
			data->config.player_x = tmp_x;
		add_padding(tmp_x, data->config.player_y, &y_padding);
		map_check_x = (int)data->config.player_x / TILE_SIZE;
		map_check_y = (int)(tmp_y + y_padding) / TILE_SIZE;
		if (data->config.map[map_check_y][map_check_x] != '1' &&
        	data->config.map[map_check_y][map_check_x] != ' ')
			data->config.player_y = tmp_y;
	}
}

void	add_padding(double target, double player_coord, double *padding)
{
	if (target > player_coord)
		*padding = COLLISION_PADDING;
	else
		*padding = -COLLISION_PADDING;
}
