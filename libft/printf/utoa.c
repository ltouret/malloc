/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utoa.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/16 18:16:22 by ltouret           #+#    #+#             */
/*   Updated: 2020/05/16 18:16:34 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		nbr_len(unsigned long nbr, int base_len)
{
	int		i;

	i = 1;
	while (nbr /= base_len)
		i++;
	return (i);
}

char	*utoa_base(unsigned int nbr, const char *base)
{
	char	*str;
	int		base_len;
	int		i;

	base_len = ft_strlen(base);
	i = nbr_len(nbr, base_len);
	if (base_len < 2)
		return (NULL);
	if (!(str = malloc(sizeof(char) * (i + 1))))
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

char	*ultoa_base(unsigned long nbr, const char *base)
{
	char	*str;
	int		base_len;
	int		i;

	base_len = ft_strlen(base);
	i = nbr_len(nbr, base_len);
	if (base_len < 2)
		return (NULL);
	if (!(str = malloc(sizeof(char) * (i + 1))))
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
