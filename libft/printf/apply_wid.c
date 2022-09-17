/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_wid.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/19 16:08:01 by ltouret           #+#    #+#             */
/*   Updated: 2020/05/25 09:15:23 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		str_wid(t_block *block, int wid, int minus)
{
	char	*pre_str;
	char	*tmp;
	int		len;

	len = ft_strlen(block->converted);
	if (wid > len)
	{
		if (!(pre_str = malloc(sizeof(char) * (wid - len + 1))))
			return (-1);
		ft_memset(pre_str, ' ', wid - len);
		pre_str[wid - len] = '\0';
		if (minus)
		{
			if (!(tmp = ft_strjoin(block->converted, pre_str)))
				return (-1);
		}
		else if (!(tmp = ft_strjoin(pre_str, block->converted)))
			return (-1);
		free(pre_str);
		free(block->converted);
		block->converted = tmp;
	}
	return (1);
}

int		apply_wid(t_list *current, int zero, int minus)
{
	int			wid;
	int			i;
	t_block		*block;

	i = 0;
	block = ((t_block*)current->content);
	while (block->raw_block[i] &&
		ft_find("%-0", block->raw_block[i]) != -1)
		i++;
	wid = ft_atoi(&(block->raw_block[i]));
	if (ft_find("cdiuxXp%", block->type) != -1)
		if (int_wid(block, wid, minus, zero) == -1)
			return (-1);
	if (ft_find("s", block->type) != -1)
		if (str_wid(block, wid, minus) == -1)
			return (-1);
	return (1);
}
