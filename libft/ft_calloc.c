/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/01 23:43:28 by ltouret           #+#    #+#             */
/*   Updated: 2021/09/04 23:39:55 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t count, size_t size)
{
	char			*dst;
	unsigned int	total;
	unsigned int	i;

	if ((count * size) == 0)
		total = 1;
	else
		total = count * size;
	dst = malloc(total);
	if (!dst)
		return (NULL);
	i = 0;
	while (total--)
		dst[i++] = 0;
	return ((void *)dst);
}
