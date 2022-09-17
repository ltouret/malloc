/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convert_func.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/16 18:01:13 by ltouret           #+#    #+#             */
/*   Updated: 2020/05/20 15:42:10 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		convert_hex(t_list *current)
{
	unsigned int	arg;

	arg = *(unsigned int*)((t_block*)current->content)->param;
	if ((((t_block*)current->content)->converted = utoa_base(arg,
					"0123456789abcdef")))
		return (1);
	return (-1);
}

int		convert_upper_hex(t_list *current)
{
	unsigned int	arg;

	arg = *(unsigned int*)((t_block*)current->content)->param;
	if ((((t_block*)current->content)->converted = utoa_base(arg,
					"0123456789ABCDEF")))
		return (1);
	return (-1);
}

int		convert_int(t_list *current)
{
	int				arg;

	arg = *(int*)((t_block*)current->content)->param;
	if ((((t_block*)current->content)->converted = ft_itoa(arg)))
		return (1);
	return (-1);
}

int		convert_uint(t_list *current)
{
	unsigned int	arg;

	arg = *(unsigned int*)((t_block*)current->content)->param;
	if ((((t_block*)current->content)->converted = utoa_base(arg,
					"0123456789")))
		return (1);
	return (-1);
}

int		convert_ptr(t_list *current)
{
	unsigned long	arg;
	char			*tmp;

	arg = (unsigned long)((t_block*)current->content)->param;
	if ((((t_block*)current->content)->converted = ultoa_base(arg,
					"0123456789abcdef")))
	{
		if ((tmp = ft_strjoin("0x", ((t_block*)current->content)->converted)))
		{
			free(((t_block*)current->content)->converted);
			((t_block*)current->content)->converted = tmp;
			return (1);
		}
	}
	return (-1);
}
