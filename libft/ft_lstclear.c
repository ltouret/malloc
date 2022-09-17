/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/22 18:57:18 by ltouret           #+#    #+#             */
/*   Updated: 2021/09/04 23:44:07 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void*))
{
	t_list	*temp;
	t_list	*erase;

	temp = *lst;
	if (!lst || !del)
		return ;
	if (*lst == NULL)
	{
		*lst = NULL;
		return ;
	}
	while (temp)
	{
		erase = temp;
		(*del)(temp->content);
		temp = temp->next;
		free(erase);
	}
	*lst = NULL;
}
