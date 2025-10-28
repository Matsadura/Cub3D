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
