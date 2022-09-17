/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/10 14:44:48 by ltouret           #+#    #+#             */
/*   Updated: 2021/09/04 23:55:26 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	int		i;
	int		o;

	i = 0;
	o = 0;
	if (!s1 || !s2)
		return (NULL);
	str = (char *)malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (str)
	{
		while (s1[i])
		{
			str[i] = s1[i];
			i++;
		}
		while (s2[o])
		{
			str[i] = s2[o];
			i++;
			o++;
		}
		str[i] = '\0';
	}
	return (str);
}
