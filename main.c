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
	(void)av;
	if (ac != 2)
		return (EXIT_FAILURE);
	if (does_file_exist(av[1]) == TRUE)
		printf("exists\n");
	else
		printf("does not exist\n");
	return (EXIT_SUCCESS);
}
