/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/15 10:53:44 by ltouret           #+#    #+#             */
/*   Updated: 2021/09/05 00:10:25 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char				*str;
	unsigned int		i;

	i = 0;
	if (!s || !f)
		return (NULL);
	str = (char*)malloc((ft_strlen(s) + 1) * sizeof(char));
	if (str)
	{
		while (s[i])
		{
			str[i] = (*f)(i, s[i]);
			i++;
		}
		str[i] = '\0';
	}
	return (str);
}
