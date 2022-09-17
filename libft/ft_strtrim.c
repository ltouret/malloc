/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/10 17:06:16 by ltouret           #+#    #+#             */
/*   Updated: 2021/09/05 00:09:33 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	find_set(char const *s1, char const *set, int opt)
{
	int		i;
	int		o;

	i = 0;
	if (opt == 0)
		o = 0;
	else
		o = ft_strlen(s1) - 1;
	while (set[i] && o >= 0 && s1[o])
	{
		if (set[i] == s1[o])
		{
			if (opt == 0)
				o++;
			else
				o--;
			i = -1;
		}
		i++;
	}
	return (o);
}

int	no_neg(int i)
{
	if (0 > i)
		i = 0;
	return (i);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char		*str;
	int			i;
	int			last;
	int			start;

	if (!s1 || !set)
		return (NULL);
	i = 0;
	last = find_set(s1, set, 0);
	start = find_set(s1, set, 1);
	str = (char *)malloc((no_neg(start - last) + 2) * sizeof(char));
	if (str)
	{
		while (start >= last)
		{
			str[i] = s1[last];
			i++;
			last++;
		}
		str[i] = '\0';
	}
	return (str);
}
