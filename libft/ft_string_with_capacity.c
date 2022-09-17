/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_string_with_capacity.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/29 17:41:05 by ltouret           #+#    #+#             */
/*   Updated: 2021/03/29 17:41:16 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

t_string	*ft_string_with_capacity(size_t capacity)
{
	t_string	*new_string;

	new_string = malloc(sizeof(t_string));
	if (!new_string)
		return (NULL);
	new_string->str = malloc(sizeof(char) * capacity);
	if (!new_string->str)
		return (NULL);
	new_string->len = 0;
	new_string->capacity = capacity;
	return (new_string);
}
