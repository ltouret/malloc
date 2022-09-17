/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_string_destroy.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/29 17:36:28 by ltouret           #+#    #+#             */
/*   Updated: 2021/05/13 21:20:32 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_string_destroy(t_string *string)
{
	if (!string)
		return ;
	free(string->str);
	free(string);
}

void	ft_string_clear(t_string *string)
{
	if (!string)
		return ;
	ft_bzero(string->str, string->capacity);
	string->len = 0;
}
