/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlast.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/22 16:06:34 by ltouret           #+#    #+#             */
/*   Updated: 2021/09/04 23:44:35 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstlast(t_list *lst)
{
	t_list	*temp;
	int		i;

	temp = lst;
	i = 0;
	if (lst == NULL)
		return (NULL);
	while (temp)
	{
		temp = temp->next;
		i++;
	}
	while (i-- - 1)
		lst = lst->next;
	return (lst);
}
