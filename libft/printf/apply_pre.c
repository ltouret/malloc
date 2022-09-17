/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_pre.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/16 22:36:51 by ltouret           #+#    #+#             */
/*   Updated: 2020/05/24 22:22:40 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		handle_minus_pre(t_block *block, int *minus)
{
	char	*tmp;

	*minus = 0;
	if (block->converted[0] == '-')
	{
		if (!(tmp = ft_strdup(&block->converted[1])))
			return (-1);
		free(block->converted);
		block->converted = tmp;
		*minus = 1;
	}
	return (1);
}

int		int_pre_special(t_block *block, int precision, int minus)
{
	char *tmp;

	if (precision == 0 && block->type != 'p' && *((int *)block->param) == 0)
	{
		block->converted[0] = '\0';
		return (1);
	}
	if (minus)
	{
		if (!(tmp = ft_strjoin("-", block->converted)))
			return (-1);
		free(block->converted);
		block->converted = tmp;
	}
	if (block->type == 'p' && precision == 0 && (long)block->param == 0)
	{
		free(block->converted);
		block->converted = ft_strdup("0x");
	}
	return (1);
}

int		int_pre(t_block *block, int precision)
{
	char	*tmp;
	char	*pre_str;
	int		len;
	int		minus;

	if (handle_minus_pre(block, &minus) == -1)
		return (-1);
	len = ft_strlen(block->converted);
	if (precision > len)
	{
		if (!(pre_str = malloc(sizeof(char) * precision - len + 1)))
			return (-1);
		ft_memset(pre_str, '0', precision - len);
		pre_str[precision - len] = '\0';
		if (!(tmp = ft_strjoin(pre_str, block->converted)))
			return (-1);
		free(pre_str);
		free(block->converted);
		block->converted = tmp;
	}
	return (int_pre_special(block, precision, minus));
}

int		str_pre(t_block *block, int precision)
{
	int		len;
	char	*tmp;

	len = ft_strlen(block->converted);
	if (block->param == NULL && precision < 6)
	{
		if (!(tmp = ft_strdup("")))
			return (-1);
		free(block->converted);
		block->converted = tmp;
	}
	else if (len > precision && block->type == 's' && precision >= 0)
	{
		if (!(tmp = malloc(sizeof(char) * (precision + 1))))
			return (-1);
		ft_strlcpy(tmp, block->converted, precision + 1);
		free(block->converted);
		block->converted = tmp;
	}
	return (1);
}

int		apply_pre(t_list *current)
{
	int			i;
	int			pre;
	t_block		*block;

	i = 0;
	block = ((t_block*)current->content);
	while (block->raw_block[i] && block->raw_block[i] != '.')
		i++;
	if (block->raw_block[i] == '\0')
		return (1);
	i++;
	pre = ft_atoi((&(block->raw_block[i])));
	if (ft_find("cdiuxXp", block->type) != -1)
		if (int_pre(block, pre) == -1)
			return (-1);
	if (ft_find("s", block->type) != -1)
		if (str_pre(block, pre) == -1)
			return (-1);
	return (1);
}
