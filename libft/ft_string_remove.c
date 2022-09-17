/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_string_remove.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/29 17:40:09 by ltouret           #+#    #+#             */
/*   Updated: 2021/05/06 14:12:48 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_string	*ft_string_remove(t_string *string, int start, int end)
{
	t_string	*out;

	out = ft_string_with_capacity(end - start + 1);
	if (!out)
		return (NULL);
	ft_strlcpy(out->str, &string->str[start], end - start + 1);
	out->len = end - start;
	ft_strlcpy(&string->str[start], &string->str[end], string->capacity);
	string->len -= out->len;
	return (out);
}

char		ft_string_pop(t_string *string)
{
	char		c;

	c = *(string->str + string->len - 1);
	string->str[string->len - 1] = '\0';
	string->len--;
	return (c);
}
