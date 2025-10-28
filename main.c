/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 12:42:35 by zzaoui            #+#    #+#             */
/*   Updated: 2025/10/22 12:42:40 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * Main entry point of the program
 * Returns: 0 on success otherwise 1.
 */
int	main(int ac, char **av)
{
	int	fd;
	char	**file_lines;

	if (ac != 2)
		return (EXIT_FAILURE);
	fd = open_file(av[1]);
	if (fd < 0)
		return (EXIT_FAILURE);
	file_lines = read_file(fd);
	int i = 0;
	while (file_lines[i] != NULL)
	{
		ft_printf("%s\n", file_lines[i]);
		i++;
	}
	i = 0;
	while (file_lines[i] != NULL)
	{
		free(file_lines[i]);
		i++;
	}
	free(file_lines);
	close(fd);
	return (EXIT_SUCCESS);
}
