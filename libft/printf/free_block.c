/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_block.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 20:40:32 by ltouret           #+#    #+#             */
/*   Updated: 2020/05/24 20:42:20 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

void	free_block(void *content)
{
	t_block		*block;

	block = content;
	if (block->raw_block)
		free(block->raw_block);
	if (block->param && block->type != 's' && block->type != 'p')
		free(block->param);
	if (block->wildcard[0])
		free(block->wildcard[0]);
	if (block->wildcard[1])
		free(block->wildcard[1]);
	if (block->converted)
		free(block->converted);
	free(block);
}
