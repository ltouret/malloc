/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_string_sub.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/29 17:40:34 by ltouret           #+#    #+#             */
/*   Updated: 2021/05/13 20:24:34 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_string	*ft_string_sub(t_string *string, size_t start, size_t len)
{
	t_string	*out;

	if (!string)
		return (NULL);
	out = ft_string_new();
	if (!out)
		return (NULL);
	out->str = malloc(sizeof(char) * (len + 1));
	if (!out->str)
		return (NULL);
	ft_strlcpy(out->str, string->str + start, len + 1);
	out->len = len;
	out->capacity = len + 1;
	return (out);
}

t_string	*ft_string_dup(char *string)
{
	t_string	*new;

	new = ft_string_new();
	if (!string || !new)
		return (NULL);
	if (!ft_string_push(new, string))
		return (NULL);
	return (new);
}
