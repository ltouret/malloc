/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memccpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/21 07:23:38 by ltouret           #+#    #+#             */
/*   Updated: 2021/09/04 23:48:42 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memccpy(void *dst, void *src, int c, size_t n)
{
	size_t			i;
	unsigned char	*dst_c;
	unsigned char	*src_c;

	src_c = (unsigned char *)src;
	dst_c = (unsigned char *)dst;
	i = 0;
	while (i < n && src_c[i] != (unsigned char)c)
	{
		dst_c[i] = src_c[i];
		i++;
	}
	if (i == n)
		return (NULL);
	else
	{
		dst_c[i] = src_c[i];
		i++;
		return (&dst_c[i]);
	}
}
/*
**#include <stdio.h>
**#include <string.h>
**
**int		main(void)
**{
**	char	src[100] = "test1 test2 testC";
**	char	dst[100] = "aaaaaaaaaa";
**	char	src_ft[100] = "test1 test2 testC";
**	char	dst_ft[100] = "aaaaaaaaaa";;
**
**	printf("ft_memccpy : %s\nmemccpy : %s\n",
**			(char *)ft_memccpy(dst_ft, src_ft, 'C', 17),
**			(char *)memccpy(dst, src, 'C', 17));
**	printf("dst : %s\nsrc :%s\ndst_ft : %s\nsrc_ft : %s", dst, src,
**			dst_ft, src_ft);
**	return (0);
**}
*/
