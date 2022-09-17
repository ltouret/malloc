/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/17 14:31:08 by ltouret           #+#    #+#             */
/*   Updated: 2021/09/05 11:20:14 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*gnl_strjoin(char const *s1, char const *s2)
{
	char	*str;
	int		i;
	int		o;

	if (!s1 || !s2)
		return (NULL);
	str = (char *)malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (str)
	{
		i = -1;
		o = 0;
		while (s1[++i])
			str[i] = s1[i];
		while (s2[o])
			str[i++] = s2[o++];
		str[i] = '\0';
	}
	free((void *)s1);
	free((void *)s2);
	return (str);
}

int	hand_return(char *buff, char **after_nl, int r_eof, char **line)
{
	*line = str_until_nl(*after_nl);
	free(buff);
	free(*after_nl);
	*after_nl = NULL;
	buff = NULL;
	if (r_eof == -1)
		return (-1);
	if (**line == '\0' || find_nl(*line) == -1)
		return (0);
	return (1);
}
