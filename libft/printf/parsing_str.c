/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_str.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leet <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/15 19:31:08 by leet              #+#    #+#             */
/*   Updated: 2020/05/20 18:18:09 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		find_term_char(char *str)
{
	int		i;

	i = -1;
	while (str[++i])
		if (ft_find("cspdiuxX%", str[i]) != -1)
			return (i);
	return (-1);
}

char	*str_index_dup(char *str, int start, int end)
{
	char	*new_str;
	int		str_size;
	int		i;

	str_size = end - start;
	if (str_size < 0 || start > ft_strlen(str) || end > ft_strlen(str) ||
			start >= end)
		return (NULL);
	if ((new_str = malloc(sizeof(char) * (str_size + 1))))
	{
		i = 0;
		while (end > start)
			new_str[i++] = str[start++];
		new_str[i] = '\0';
		return (new_str);
	}
	return (NULL);
}

char	get_type(char *raw_block)
{
	int		len;
	char	c;

	len = ft_strlen(raw_block);
	c = raw_block[--len];
	return (c);
}

void	init_cont(t_list *current)
{
	((t_block*)current->content)->raw_block = NULL;
	((t_block*)current->content)->param = NULL;
	((t_block*)current->content)->wildcard[0] = NULL;
	((t_block*)current->content)->wildcard[1] = NULL;
	((t_block*)current->content)->converted = NULL;
}

int		parsing_str(char *str, t_list **lst)
{
	char	*tmp;
	t_list	*current;
	t_block	*block;

	while (*str)
	{
		if (*str == '%')
		{
			if (!(block = malloc(sizeof(t_block))))
				return (-1);
			if (!(current = ft_lstnew((void *)block)))
				return (-1);
			init_cont(current);
			tmp = str_index_dup(str, ft_find(str, '%'),
				find_term_char(str + 1) + 2);
			((t_block*)current->content)->raw_block = tmp;
			((t_block*)current->content)->type = get_type(tmp);
			ft_lstadd_back(lst, current);
			str = str + find_term_char(str + 1) + 1;
		}
		str++;
	}
	return (1);
}
