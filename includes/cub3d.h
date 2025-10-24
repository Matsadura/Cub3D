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

/* Parsing functions */

int	is_file_ext(char *filepath, char *ext);
int	does_file_exist(char *file_path);

#endif /* CUB3D_H  */
