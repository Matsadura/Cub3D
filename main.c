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

#include <stdio.h>
#include <stdlib.h>
#include "libft.h"

/**
 * Main entry point of the program
 * Returns: 0 on success otherwise 1.
 */
int	main(int ac, char **av)
{
	(void)av;
	if (ac != 2)
		return (EXIT_FAILURE);
	ft_printf("%d", ac);
	return (EXIT_SUCCESS);
}
