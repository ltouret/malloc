/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/21 07:23:38 by ltouret           #+#    #+#             */
/*   Updated: 2019/11/05 11:58:34 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	void	*original_dst;

	if (!dst && !src)
		return (NULL);
	original_dst = dst;
	while (n--)
		*((char *)dst++) = *((char *)src++);
	return (original_dst);
}
/*
**#include <string.h>
**
**int		main(void)
**{
**	write(1, "ft_memcpy\n", 10);
**	ft_memcpy(NULL, NULL, 0);
**	write(1, "memcpy\n", 7);
**	memcpy(NULL, NULL, 0);
**}
*/
