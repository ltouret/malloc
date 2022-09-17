/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_string_new.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/29 17:36:58 by ltouret           #+#    #+#             */
/*   Updated: 2021/05/13 19:43:18 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_string	*ft_string_new(void)
{
	t_string	*new_string;

	new_string = malloc(sizeof(t_string));
	if (!new_string)
		return (NULL);
	new_string->str = NULL;
	new_string->len = 0;
	new_string->capacity = 0;
	return (new_string);
}
