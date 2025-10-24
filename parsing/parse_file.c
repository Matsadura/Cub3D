/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_file.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 15:46:16 by zzaoui            #+#    #+#             */
/*   Updated: 2025/10/22 15:46:19 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * is_file_ext - Checks if the file is a .cub
 * @filepath: The path of the file map to check
 * @ext: The file extention 
 * Returns: True if the extention matches, otherwise False.
 */
int	is_file_ext(char *filepath, char *ext)
{
	size_t	len_f;
	size_t	len_e;

	if (filepath == NULL || ext == NULL)
		return (FALSE);
	len_f = ft_strlen(filepath);
	len_e = ft_strlen(ext);
	if (len_f <= len_e)
		return (FALSE);
	if (ft_strcmp(filepath + len_f - len_e, ext) != 0)
		return (FALSE);
	return (TRUE);
}

/**
 * does_file_exist - Checks if the file exists
 * @filepath: The path of the file to check
 * Returns: True if it exists, otherwise False.
 */
int	does_file_exist(char *filepath)
{
	int	fd;

	fd = open(filepath, O_RDONLY);
	if (fd < 0)
		return (FALSE);
	close(fd);
	return (TRUE);
}
