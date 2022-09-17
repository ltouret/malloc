# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/10/20 13:00:20 by ltouret           #+#    #+#              #
#    Updated: 2020/10/27 23:25:42 by ltouret          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a

SRCS = ft_isdigit.c ft_memset.c ft_strjoin.c ft_strtrim.c ft_isprint.c\
ft_putchar_fd.c ft_strlcat.c ft_substr.c ft_atoi.c ft_atol.c ft_itoa.c\
ft_itoa_base.c ft_putendl_fd.c ft_strlcpy.c ft_tolower.c ft_bzero.c\
ft_memccpy.c ft_putnbr_fd.c ft_strlen.c ft_toupper.c ft_calloc.c ft_memchr.c\
ft_putstr_fd.c ft_strmapi.c ft_isalnum.c ft_memcmp.c ft_split.c ft_strncmp.c\
ft_strcmp.c ft_isalpha.c ft_memcpy.c ft_strchr.c ft_strnstr.c ft_isascii.c\
ft_memmove.c ft_strdup.c ft_strrchr.c ft_find.c\
\
gnl/get_next_line.c gnl/get_next_line_utils.c\
\
printf/ft_printf.c printf/utoa.c printf/convert_func.c printf/convert_func2.c\
printf/convert.c printf/get_param.c printf/parsing_str.c printf/apply_mod.c\
printf/apply_pre.c printf/apply_wid.c printf/apply_wid_int.c\
printf/free_block.c\
\
ft_lstadd_back.c ft_lstadd_front.c ft_lstclear.c ft_lstdelone.c\
ft_lstlast.c ft_lstmap.c ft_lstnew.c ft_lstsize.c

OBJS = ${SRCS:.c=.o}

CC		= gcc
RM		= rm -f

CFLAGS = -Wall -Wextra -Werror

.c.o:
		${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

$(NAME): ${OBJS}
		ar rcs ${NAME} ${OBJS}

all:	${NAME}

clean:
		${RM} ${OBJS}

fclean:	clean
		${RM} ${NAME}

re:		fclean all

.PHONY: all clean fclean re
