/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arrays.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zzaoui <zzaoui@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 13:31:29 by zzaoui            #+#    #+#             */
/*   Updated: 2025/10/28 13:31:31 by zzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * arrlen - Calculates the length of a NULL-terminated array of strings
 * @array: The array to measure
 * Returns: The number of strings in the array
 */
int	arraylen(char **array)
{
	int	count;

	count = 0;
	if (array == NULL)
		return (0);
	while (array[count] != NULL)
		count++;
	return (count);
}

/**
 * free_array - Frees a NULL-terminated array of strings
 * @array: The array to free
 */
void	free_array(char **array)
{
	int	i;

	if (array == NULL)
		return ;
	i = 0;
	while (array[i] != NULL)
	{
		free(array[i]);
		i++;
	}
	free(array);
}
