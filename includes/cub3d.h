/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 15:27:32 by zzaoui            #+#    #+#             */
/*   Updated: 2025/10/22 15:29:24 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "libft.h"

/* Macros */

# define TRUE 1
# define FALSE 0
# define STDERR 2

/* Main config structure */

typedef struct s_config
{
	char	**tmp_lines;
	char	*no_path;
	char	*so_path;
	char	*we_path;
	char	*ea_path;
	int		floor_color[3];
	int		ceiling_color[3];
	char	**map;
	int		map_width;
	int		map_height;
	int		player_x;
	int		player_y;
	char	player_camera;
	int		fd;
}	t_config;

/* Parsing functions */

int		is_file_ext(char *filepath, char *ext);
// int		does_file_exist(char *file_path);
int		open_file(char *file_name);
char	**read_file(int fd, t_config *config);
int		parse_textures_colors(t_config *config);
int		parse_map_lines(t_config *config);

/* Parsing Utils functions */

int		all_elements_found(int *mask);
int		is_valid_element(char *line, int *mask);
int		validate_color_component(char *value, int *component);
int		is_valid_map_line(char *line);
int		check_player_position(t_config *config);
int		is_void_inside(t_config *config);
int		is_player_on_edge(t_config *config);
int		is_map_wall_enclosed(char **map);
char	**split_lines_preserve_empty(const char *str);
void	validate_map_block(char **lines, t_config *config);
int		line_is_empty(const char *line);
char	get_map_char(char **map, int row, int col);

/* Utils functions */

int		arraylen(char **array);
void	free_array(char **array);
void	print_array(char **array);
void	free_array_and_ptr(char **array, char *ptr);
void	error_and_exit(const char *message, t_config *config);
void	check_texture_paths(t_config *config);
void	handle_assignment(t_config *config, char **split,
			char *trimmed, int *mask);
int		assign_texture_path(t_config *config, char **line, int *mask);
int		assign_color_value(t_config *config, char *line, int *mask);
char	*prepare_trimmed_line(char *raw_line, t_config *config);
char	**split_config_line(char *trimmed, t_config *config);
void	validate_element_or_exit(char **split, char *trimmed, int *mask,
			t_config *config);

#endif /* CUB3D_H  */
