/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/18 10:37:38 by ltouret           #+#    #+#             */
/*   Updated: 2021/09/04 23:52:37 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	nbr_len(unsigned long nbr, int base_len)
{
	int		i;

	i = 1;
	while (nbr != 0)
	{
		nbr /= base_len;
		i++;
	}
	return (i);
}

char	*ft_itoa_base(int nbr, const char *base)
{
	char	*str;
	int		base_len;
	int		i;

	base_len = ft_strlen(base);
	i = nbr_len(nbr, base_len);
	if (base_len < 2)
		return (NULL);
	str = malloc(sizeof(char) * (i + 1));
	if (!str)
		return (NULL);
	str[i--] = '\0';
	str[i--] = base[(nbr % base_len)];
	nbr /= base_len;
	while (nbr != 0)
	{
		str[i--] = base[(nbr % base_len)];
		nbr /= base_len;
	}
	return (str);
}
