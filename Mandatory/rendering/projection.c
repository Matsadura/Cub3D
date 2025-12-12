/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   projection.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 15:34:58 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/28 15:35:04 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * draw_wall_fallback - Draws a fallback wall strip in red if texture is missing.
 * @data: The main data structure.
 * @x: The current screen column.
 * @start: The starting y-coordinate of the wall strip.
 * @end: The ending y-coordinate of the wall strip.
 */
static void	draw_wall_fallback(t_data *data, int x, int start, int end)
{
    int	y;

    y = start;
    while (y < end)
        my_mlx_pixel_put(data, x, y++, RED);
}

/**
 * draw_wall_textured - Draws a textured wall strip.
 * @data: The main data structure.
 * @tex: Pointer to the texture structure.
 * @x: The current screen column.
 * @w: Pointer to the wall strip structure containing drawing parameters.
 */
static void	draw_wall_textured(t_data *data, t_tex *tex, int x, t_wallstrip *w)
{
    int	y;
    int	tex_y;

    w->step = (double)tex->h / w->wall_height;
    w->tex_pos = (w->start - w->wall_top) * w->step;
    y = w->start;
    while (y < w->end)
    {
        tex_y = clamp_int((int)w->tex_pos, 0, tex->h - 1);
        my_mlx_pixel_put(data, x, y,
            texture_get_pixel(tex, w->tex_x, tex_y));
        w->tex_pos += w->step;
        y++;
    }
}

/**
 * draw_wall - Calculates wall height and draws a vertical strip for the 3D view.
 * @data: The main data structure.
 * @ray: The corrected distance to the wall.
 * @x: The current screen column (x-coordinate) being rendered.
 */
void	draw_wall(t_data *data, double ray, int x)
{
    t_wallstrip	w;
    t_tex		*tex;

    if (data == NULL)
        return ;
    wall_compute(ray, &w);
    draw_ceiling(data, w.start, x);
    tex = choose_wall_texture(data, data->coord.ray_angle);
    if (!tex || !tex->addr || tex->w <= 0 || tex->h <= 0)
        draw_wall_fallback(data, x, w.start, w.end);
    else
    {
        w.tex_x = wall_tex_x(data, tex);
        draw_wall_textured(data, tex, x, &w);
    }
    draw_floor(data, w.end, x);
}

/**
 * draw_ceiling - Fills the pixels above the wall strip with ceiling color.
 * @data: The main data structure.
 * @wall_start: The y-coordinate where the top of the wall begins.
 * @x: The current screen column.
 */
void	draw_ceiling(t_data *data, int wall_start, int x)
{
	int	start;

	if (data == NULL)
		return ;
	start = 0;
	while (start < wall_start)
	{
		my_mlx_pixel_put(data, x, start,
			rgb_to_hex(data->config.ceiling_color));
		start++;
	}
}

/**
 * draw_floor - Fills the pixels below the wall strip with floor color.
 * @data: The main data structure.
 * @wall_end: The y-coordinate where the bottom of the wall ends.
 * @x: The current screen column.
 */
void	draw_floor(t_data *data, int wall_end, int x)
{
	if (data == NULL)
		return ;
	while (wall_end < SCREEN_HEIGHT)
	{
		my_mlx_pixel_put(data, x, wall_end,
			rgb_to_hex(data->config.floor_color));
		wall_end++;
	}
}
