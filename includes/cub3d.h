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
	int		player_x;
	int		player_y;
	char	player_camera;
}	t_config;

/* Parsing functions */

int		is_file_ext(char *filepath, char *ext);
// int		does_file_exist(char *file_path);
int		open_file(char *file_name);
char	**read_file(int fd);
int		parse_textures_colors(t_config *config);

/* Parsing Utils functions */

int		all_elements_found(int *mask);
int		is_valid_element(char *line, int *mask);

/* Utils functions */

int		arraylen(char **array);
void	free_array(char **array);

#endif /* CUB3D_H  */
