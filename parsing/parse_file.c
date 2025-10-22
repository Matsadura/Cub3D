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
 * is_cub_ext - Checks if the file is a .cub
 * @filepath: The path of the file map to check
 * Returns: True if .cub, otherwise False.
 */
int	is_cub_ext(char *filepath)
{
	char	*tmp;

	if (ft_strlen(filepath) < 4)
		return (FALSE);
	tmp = filepath;
	while (*(tmp + 4) != '\0')
		tmp++;
	if (ft_strcmp(".cub", tmp) != 0)
		return (FALSE);
	return (TRUE);
}
