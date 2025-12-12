/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 15:33:33 by claghrab          #+#    #+#             */
/*   Updated: 2025/11/30 15:33:35 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_BONUS_H
# define CUB3D_BONUS_H

# include "../minilibx-linux/mlx.h"
# include "libft.h"
# include <math.h>
# include <stdio.h>
# include <stdlib.h>

/* Macros */

# define TRUE 1
# define FALSE 0
# define STDERR 2
# define RED 0xFF0000
# define BLACK 0x000000
# define BLUE 0x0000FF
# define WHITE 0xFFFFFF
# define BLUE 0x0000FF
# define BROWN 0x8B4513
# define TILE_SIZE 64
# define ROTATION_SPEED 0.01
# define WALK_SPEED 0.7
# define COLLISION_PADDING 15
# define FORWARD 119
# define BACKWARD 115
# define LEFT 97
# define RIGHT 100
# define ESC 65307
# define LEFT_ARROW 65361
# define RIGHT_ARROW 65363
# define LINE_LEN 100
# define PI 3.141592653
# define SCREEN_WIDTH 1900
# define SCREEN_HEIGHT 1200
# define FOV 1.047197551
# define MINIMAP_SCALE 2
# define MINIMAP_LINE_LEN 50
# define X_START_POINT 32
# define Y_START_POINT 70.588235294
# define MINIMAP_SIZE 300
# define MINIMAP_RADIUS 150
# define MINIMAP_XCENTER 182
# define MINIMAP_YCENTER 220.588235294
# define NO 0
# define SO 1
# define WE 2
# define EA 3

/* Main config structure */

typedef struct s_config
{
	char		**tmp_lines;
	char		*no_path;
	char		*so_path;
	char		*we_path;
	char		*ea_path;
	char		**map;
	char		player_camera;
	int			floor_color[3];
	int			ceiling_color[3];
	int			fd;
	int			map_width;
	int			map_height;
	double		player_x;
	double		player_y;
	double		player_angle;
	double		delta_x;
	double		delta_y;
}				t_config;

/* Main window structure */

typedef struct s_win
{
	void		*mlx_ptr;
	void		*win_ptr;
}				t_win;

/* Main image structure */

typedef struct s_img
{
	void		*img;
	char		*addr;
	int			bpp;
	int			line_length;
	int			endian;
}				t_img;

typedef struct s_tex
{
	void		*img;
	char		*addr;
	int			w;
	int			h;
	int			bpp;
	int			line_length;
	int			endian;
}				t_tex;

/* Main data structure */

typedef struct s_coord
{
	double		ray_end_x;
	double		ray_end_y;
	double		x_inter;
	double		y_inter;
	double		x_step;
	double		y_step;
	int			x_map;
	int			y_map;

	double		ray_angle;
	double		h_hit_x;
	double		h_hit_y;
	double		v_hit_x;
	double		v_hit_y;
	double		hit_x;
	double		hit_y;
	int			hit_vertical;
}				t_coord;

typedef struct s_mvmnt
{
	double		turn_direc;
	double		walk_direc;
	double		rotation_direc;
}				t_mvmnt;

typedef struct s_data
{
	t_win		win;
	t_config	config;
	t_img		img;
	t_mvmnt		mvmnt;
	t_coord		coord;
	t_tex		tex[4];
}				t_data;

typedef struct s_wallstrip
{
    int			start;
    int			end;
    double		wall_height;
    double		wall_top;
    double		step;
    double		tex_pos;
    int			tex_x;
}	t_wallstrip;

/* Parsing functions */

int				is_file_ext(char *filepath, char *ext);
int				open_file(char *file_name);
char			**read_file(int fd, t_config *config);
int				parse_textures_colors(t_config *config);
int				parse_map_lines(t_config *config);

/* Parsing Utils functions */

int				all_elements_found(int *mask);
int				is_valid_element(char *line, int *mask);
int				validate_color_component(char *value, int *component);
int				is_valid_map_line(char *line);
int				check_player_position(t_config *config);
int				is_void_inside(t_config *config);
int				is_player_on_edge(t_config *config);
int				is_map_wall_enclosed(char **map);
char			**split_lines_preserve_empty(const char *str);
void			validate_map_block(char **lines, t_config *config);
int				line_is_empty(const char *line);
char			get_map_char(char **map, int row, int col);

/* Utils functions */

int				arraylen(char **array);
void			free_array(char **array);
void			print_array(char **array);
void			free_array_and_ptr(char **array, char *ptr);
void			error_and_exit(const char *message, t_config *config);
void			check_texture_paths(t_config *config);
void			handle_assignment(t_config *config, char **split, char *trimmed,
					int *mask);
int				assign_texture_path(t_config *config, char **line, int *mask);
int				assign_color_value(t_config *config, char *line, int *mask);
char			*prepare_trimmed_line(char *raw_line, t_config *config);
char			**split_config_line(char *trimmed, t_config *config);
void			validate_element_or_exit(char **split, char *trimmed, int *mask,
					t_config *config);

/* Rendering functions */

int				rgb_to_hex(int *color);
int				game_loop(t_data *data);
int				on_destroy(t_data *data);
int				is_facing_up(double angle);
int				is_facing_down(double angle);
int				is_facing_left(double angle);
int				is_facing_right(double angle);
int				handle_key(int keycode, t_data *data);
int				handle_mouse(int x, int y, t_data *data);
int				handle_key_release(int keycode, t_data *data);
void			window(t_data *data);
void			dda_algo(t_data *data);
void			data_init(t_data *data);
void			setup_image(t_data *data);
void			walk_player(t_data *data);
void			turn_player(t_data *data);
void			setup_window(t_data *data);
void			draw_rays_2d(t_data *data);
void			minimap_base(t_data *data);
void			render_player(t_data *data);
void			rotate_player(t_data *data);
void			render_minimap(t_data *data);
void			init_player_direction(t_data *data);
void			draw_wall(t_data *data, double ray, int x);
void			validate_move_y(t_data *data, double new_y);
void			validate_move_x(t_data *data, double new_x);
void			set_h_coord(t_data *data, double ray_angle);
void			set_v_coord(t_data *data, double ray_angle);
void			draw_floor(t_data *data, int wall_end, int x);
void			cleanup_and_exit(t_config *config, t_data *data);
void			render_minimap_helper(t_data *data, int x, int y);
void			draw_ceiling(t_data *data, int wall_start, int x);
void			my_mlx_pixel_put(t_data *data, int x, int y, int color);
void			add_padding(double target, double player_coord,
					double *padding);
double			calc_distance(t_data *data);
double			normalize_angle(double angle);
double			cast_ray(double ray_angle, t_data *data);
double			v_intersection(double ray_angle, t_data *data);
double			h_intersection(double ray_angle, t_data *data);

/* Textures functions */
void			textures_init(t_data *data);
int				texture_get_pixel(t_tex *t, int x, int y);
t_tex			*choose_wall_texture(t_data *data, double ray_angle);
double			fix_singular_angles(double ray_angle);
double			set_ray_hit(t_data *data, double h_dis, double v_dis);
int				wall_tex_x(t_data *data, t_tex *tex);
void			wall_compute(double ray, t_wallstrip *w);
int				clamp_int(int v, int min, int max);
double			clamp_ray(double ray);





#endif /* CUB3D_BONUS_H  */
