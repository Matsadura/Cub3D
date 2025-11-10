#include "cub3d.h"

int	handle_key(int keycode, t_data *data)
{
	if (data == NULL)
		return (FALSE);
	if (keycode == LEFT_ARROW)
		data->mvmnt.rotation_direc = -1;
	else if (keycode == RIGHT_ARROW)
		data->mvmnt.rotation_direc = 1;
	return (TRUE);
}

int	handle_key_release(int keycode, t_data *data)
{
	if (data == NULL)
		return (FALSE);
	if (keycode == LEFT_ARROW || keycode == RIGHT_ARROW)
        data->mvmnt.rotation_direc = 0;
	return (TRUE);
}

int	game_loop(t_data *data)
{
	if (data == NULL)
		return (FALSE);
	if (data->mvmnt.rotation_direc != 0)
    {
        data->config.player_angle += data->mvmnt.rotation_direc * MOVE_SPEED;
        data->config.delta_x = cos(data->config.player_angle);
        data->config.delta_y = sin(data->config.player_angle);
    }
    render_2Dmap(data);
    draw_direction(data);
    draw_player(data);
    mlx_put_image_to_window(data->win.mlx_ptr, data->win.win_ptr, data->img.img, 0, 0);
    return (0);
}
