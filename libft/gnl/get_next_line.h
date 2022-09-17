/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/17 14:24:48 by ltouret           #+#    #+#             */
/*   Updated: 2020/06/05 23:44:50 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdlib.h>
# include <unistd.h>
# include <stddef.h>
# include "../libft.h"

# define BU 10

char	*gnl_strjoin(char const *s1, char const *s2);
int		find_nl(char *s1);
char	*str_until_nl(char *buff);
char	*rest_of_str(char *after_nl);
int		read_buff(char *buff, char **line, int fd, char **after_nl);
int		get_next_line(int fd, char **line);
int		hand_return(char *buff, char **after_nl, int r_eof, char **line);

#endif
