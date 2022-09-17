/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/10 18:35:45 by ltouret           #+#    #+#             */
/*   Updated: 2021/09/05 11:18:09 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	find_nl(char *s1)
{
	int		i;

	i = -1;
	while (s1[++i])
		if (s1[i] == '\n')
			return (i);
	return (-1);
}

char	*str_until_nl(char *buff)
{
	int		i;
	int		counter;
	char	*str;

	i = -1;
	counter = 0;
	while (buff[++i] != '\n' && buff[i])
		counter++;
	str = (char *)(malloc(sizeof(char) * (counter + 1)));
	if (str)
	{
		i = -1;
		counter = 0;
		while (buff[++i] != '\n' && buff[i])
			str[counter++] = buff[i];
		str[counter] = '\0';
		return (str);
	}
	return (NULL);
}

char	*rest_of_str(char *after_nl)
{
	char	*str;
	int		nl;
	int		o;

	nl = find_nl(after_nl);
	if (nl == -1)
	{
		free(after_nl);
		return (ft_substr("", 0, 0));
	}
	str = (char *)(malloc(sizeof(char) * (ft_strlen(after_nl) - (nl + 1) + 1)));
	if (!(str))
		return (NULL);
	o = 0;
	while (after_nl[++nl])
		str[o++] = after_nl[nl];
	str[o] = '\0';
	free(after_nl);
	return (str);
}

int	read_buff(char *buff, char **line, int fd, char **after_nl)
{
	int		nl;
	int		r_eof;

	nl = find_nl(*after_nl);
	r_eof = BU;
	while (nl == -1 && r_eof == BU)
	{
		r_eof = read(fd, buff, BU);
		if (r_eof != -1)
			buff[r_eof] = '\0';
		nl = find_nl(buff);
		*after_nl = gnl_strjoin(*after_nl, ft_substr(buff, 0, ft_strlen(buff)));
	}
	if (r_eof != BU && find_nl(*after_nl) == -1)
		return (hand_return(buff, after_nl, r_eof, line));
	*line = str_until_nl(*after_nl);
	*after_nl = rest_of_str(*after_nl);
	free(buff);
	return (1);
}

int	get_next_line(int fd, char **line)
{
	static char	*after_nl = NULL;
	char		*buff;
	int			rtrn_val;
	int			w;

	rtrn_val = 1;
	w = -1;
	if (!after_nl)
	{
		after_nl = malloc(sizeof(char) * 1);
		if (!(after_nl))
			return (-1);
		after_nl[0] = '\0';
	}
	buff = malloc(sizeof(char) * (BU + 1));
	if (!(buff) || !line || BU <= 0 || fd < 0)
		return (-1);
	while (++w < BU + 1)
		buff[w] = 0;
	rtrn_val = read_buff(buff, line, fd, &after_nl);
	return (rtrn_val);
}
/*
**#include <stdio.h>
**#include <fcntl.h>
**
**int       main(int argc, char **argv)
**{
**  int     fd[argc];
**  int     ret[argc];
**  char    *str;
**  int     are_all_files_read;
**  int     i;
**
**  are_all_files_read = 0;
**  i = 0;
**  if (argc == 1)
**  {
**      while (ret[0])
**      {
**          fd[0] = open("/dev/stdin", O_RDONLY);
**          ret[0] = get_next_line(fd[0], &str);
**          printf("%d-%s\n", ret[0], str);
**          free(str);
**      }
**  }
**  while (i < argc - 1)
**  {
**      fd[i] = open(argv[i + 1], O_RDONLY);
**      i++;
**  }
**  i = 0;
**  while (!are_all_files_read)
**  {
**      while (i < argc - 1)
**      {
**          ret[i] = get_next_line(fd[i], &str);
**          printf("%d-%s\n", ret[i], str);
**          free(str);
**          i++;
**      }
**      are_all_files_read = 1;
**      while (i)
**      {
**          i--;
**          if (ret[i])
**              are_all_files_read = 0;
**      }
**  }
**  while (1)
**      ;
**  return (0);
**}
*/
