/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/12 18:13:42 by ltouret           #+#    #+#             */
/*   Updated: 2021/09/04 23:49:32 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_revputstr_fd(char *s, int fd, int len)
{
	int		i;

	i = len - 1;
	while (i > 0)
	{
		ft_putchar_fd(s[i], fd);
		i--;
	}
}

int	ft_len_nbr(int nb)
{
	int		i;

	i = (0 >= nb) ? 1 : 0;
	while (nb != 0)
	{
		nb /= 10;
		i++;
	}
	return (i);
}

void	ft_putnbr_fd(int n, int fd)
{
	long	temp;
	char	num[ft_len_nbr(n) + 1];
	int		i;

	temp = (long)(n);
	i = 1;
	num[0] = 0;
	if (0 > temp)
		temp = -temp;
	else if (temp == 0)
	{
		num[i] = '0';
		ft_revputstr_fd(num, fd, ft_len_nbr(n) + 1);
		return ;
	}
	while (temp != 0)
	{
		num[i] = temp % 10 + 48;
		temp /= 10;
		i++;
	}
	if (0 > n)
		num[i++] = '-';
	ft_revputstr_fd(num, fd, ft_len_nbr(n) + 1);
}
