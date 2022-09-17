/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_param.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leet <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/15 19:07:56 by leet              #+#    #+#             */
/*   Updated: 2020/05/19 20:56:56 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		get_wildcard(t_list *current, va_list args)
{
	char	*block;
	int		i;

	i = 0;
	block = ((t_block*)current->content)->raw_block;
	while (*block)
	{
		if (*block == '*')
		{
			if (!(((t_block*)current->content)->wildcard[i] =
				malloc(sizeof(int))))
				return (-1);
			*((t_block*)current->content)->wildcard[i] = va_arg(args, int);
			i++;
		}
		block++;
	}
	return (1);
}

int		get_pointer(t_list *current, void *pointer)
{
	((t_block*)current->content)->param = pointer;
	return (1);
}

int		get_int(t_list *current, int nbr)
{
	int *value;

	if ((value = malloc(sizeof(int))))
	{
		*value = nbr;
		((t_block*)current->content)->param = value;
		return (1);
	}
	return (-1);
}

int		get_percent(t_list *current)
{
	if ((((t_block*)current->content)->converted = ft_strdup("%")))
		return (1);
	return (-1);
}

int		get_param(t_list *lst, va_list args)
{
	char type;

	while (lst)
	{
		type = (((t_block*)lst->content)->type);
		if (get_wildcard(lst, args) == -1)
			return (-1);
		if (ft_find("cdiuxX", type) != -1)
		{
			if (get_int(lst, va_arg(args, int)) == -1)
				return (-1);
		}
		else if (ft_find("sp", type) != -1)
		{
			if (get_pointer(lst, va_arg(args, void *)) == -1)
				return (-1);
		}
		else if (ft_find("%", type != -1))
		{
			if (get_percent(lst) == -1)
				return (-1);
		}
		lst = lst->next;
	}
	return (1);
}
