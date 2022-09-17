/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convert.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/15 19:18:14 by ltouret           #+#    #+#             */
/*   Updated: 2020/05/15 19:18:19 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		convert_thi(t_list *lst, char type)
{
	if (type == 'x')
	{
		if (convert_hex(lst) == -1)
			return (-1);
	}
	else if (type == 'X')
	{
		if (convert_upper_hex(lst) == -1)
			return (-1);
	}
	return (1);
}

int		convert_sec(t_list *lst, char type)
{
	if (type == 'd' || type == 'i')
	{
		if (convert_int(lst) == -1)
			return (-1);
	}
	else if (type == 'u')
	{
		if (convert_uint(lst) == -1)
			return (-1);
	}
	else if (type == 's')
	{
		if (convert_str(lst) == -1)
			return (-1);
	}
	else if (convert_thi(lst, type) == -1)
		return (-1);
	return (1);
}

int		convert_fir(t_list *lst)
{
	char type;

	while (lst)
	{
		type = ((t_block*)lst->content)->type;
		if (type == 'c')
		{
			if (convert_c(lst) == -1)
				return (-1);
		}
		else if (type == 'p')
		{
			if (convert_ptr(lst) == -1)
				return (-1);
		}
		else if (convert_sec(lst, type) == -1)
			return (-1);
		lst = lst->next;
	}
	return (1);
}
