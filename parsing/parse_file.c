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

// /**
//  * does_file_exist - Checks if the file exists
//  * @filepath: The path of the file to check
//  * Returns: True if it exists, otherwise False.
//  */
// int	does_file_exist(char *filepath)
// {
// 	int	fd;

// 	fd = open(filepath, O_RDONLY);
// 	if (fd < 0)
// 		return (FALSE);
// 	close(fd);
// 	return (TRUE);
// }

/**
 * open_map - Opens a file map
 * @file_name: The map's name
 * Return: The map's file descriptor
 */
int	open_file(char *file_name)
{
	int	fd;

	if (file_name == NULL)
		return (-1);
	if (is_file_ext(file_name, ".cub") == FALSE)
	{
		ft_dprintf(STDERR, "Error\n%s: is not a valid .cub file\n", file_name);
		exit(EXIT_FAILURE);
	}
	fd = open(file_name, O_RDONLY);
	if (fd < 0)
	{
		ft_dprintf(STDERR, "Error\n");
		perror(file_name);
		exit(EXIT_FAILURE);
	}
	return (fd);
}

/**
 * read_file - Reads the entire file and splits it into lines
 * @fd: The file descriptor
 * @config: The main config structure
 * Return: An array of strings representing the file lines
 */
char	**read_file(int fd, t_config *config)
{
	char	*line;
	char	*tmp;

	if (fd < 0)
		return (NULL);
	tmp = NULL;
	line = get_next_line(fd);
	while (line != NULL)
	{
		tmp = ft_strjoin_gnl(tmp, line);
		free(line);
		line = get_next_line(fd);
		if (tmp == NULL)
			return (NULL);
	}
	config->tmp_lines = ft_split(tmp, '\n');
	free(tmp);
	if (config->tmp_lines == NULL)
		error_and_exit("Failed to read configuration file", config);
	return (config->tmp_lines);
}
