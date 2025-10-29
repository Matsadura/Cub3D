/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extras.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 16:24:05 by zzaoui            #+#    #+#             */
/*   Updated: 2025/10/28 16:24:08 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * free_array_and_ptr - Frees a NULL-terminated array of strings and a pointer
 * @array: The array to free
 * @ptr: The pointer to free
 */
void	free_array_and_ptr(char **array, char *ptr)
{
	free_array(array);
	free(ptr);
}

/** 
 * error_and_exit - Prints an error message, frees resources, and exits
 * @message: The error message to print
 * @config: The main config structure to free resources from
 */
void	error_and_exit(const char *message, t_config *config)
{
	ft_dprintf(2, "Error\n%s\n", message);
	free_array(config->tmp_lines);
	free(config->no_path);
	free(config->so_path);
	free(config->we_path);
	free(config->ea_path);
	close(config->fd);
	exit(EXIT_FAILURE);
}