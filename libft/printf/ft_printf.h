/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouret <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/28 17:25:36 by ltouret           #+#    #+#             */
/*   Updated: 2020/06/05 23:23:07 by ltouret          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stddef.h>
# include <stdlib.h>
# include <unistd.h>
# include <limits.h>
# include <stdarg.h>
# include "../libft.h"

typedef struct		s_block
{
	char	*raw_block;
	int		*wildcard[2];
	char	*converted;
	void	*param;
	char	type;
}					t_block;
void				free_block(void *content);
int					get_pre(t_block *block);
int					wid_len(t_block *block);
void				handle_minus_wid(t_block *block, int zero);
int					apply_order(t_block *block, int minus, char *pre_str);
int					int_wid(t_block *block, int wid, int minus, int zero);
int					str_wid(t_block *block, int wid, int minus);
int					apply_wid(t_list *current, int zero, int minus);
int					handle_minus_pre(t_block *block, int *minus);
int					int_pre_special(t_block *block, int precision, int minus);
int					int_pre(t_block *block, int precision);
int					str_pre(t_block *block, int precision);
int					apply_pre(t_list *current);
int					replace_wldcard(t_list *current, int i, int wild_index);
int					check_wldcards(t_list *current);
void				get_flags(t_list *current, int *zero, int *minus);
int					apply_mod(t_list *lst);
int					find_term_char(char *str);
char				*str_index_dup(char *str, int start, int end);
char				get_type(char *raw_block);
void				init_cont(t_list *current);
int					parsing_str(char *fmt, t_list **lst);
int					ft_printf(const char *str, ...);
int					nbr_len(unsigned long nbr, int base_len);
char				*utoa_base(unsigned int nbr, const char *base);
char				*ultoa_base(unsigned long nbr, const char *base);
int					get_pointer(t_list *current, void *pointer);
int					get_int(t_list *current, int nbr);
int					get_wildcard(t_list *current, va_list args);
int					get_percent(t_list *current);
int					get_param(t_list *lst, va_list args);
int					convert_fir(t_list *lst);
int					convert_sec(t_list *lst, char type);
int					convert_thi(t_list *lst, char type);
int					convert_hex(t_list *current);
int					convert_upper_hex(t_list *current);
int					convert_int(t_list *current);
int					convert_uint(t_list *current);
int					convert_c(t_list *current);
int					convert_ptr(t_list *current);
int					convert_str(t_list *current);

#endif
