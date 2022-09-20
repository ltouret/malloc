# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ltouret <ltouret@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/19 22:51:45 by ltouret           #+#    #+#              #
#    Updated: 2022/09/19 23:11:02 by ltouret          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

.SUFFIXES:
.SUFFIXES: .c .o

NAME = malloc

#NAME = libft_malloc_$(HOSTTYPE).so
#LIB_NAME = libft_malloc.so

SRCS = main.c

OBJS = ${SRCS:.c=.o}

CC		= cc
RM		= rm -f

CFLAGS = -Wall -Wextra -Werror

FSAN = -fsanitize=address

LIBFT_FLAGS	= -Llibft -lft

INCLUDES	= -I libft

.c.o:
		${CC} ${INCLUDES} ${CFLAGS} -c $< -o ${<:.c=.o}

$(NAME): ${OBJS}
		make -C libft 
		${CC} ${OBJS} ${LIBFT_FLAGS} -o ${NAME}

all:	${NAME}

clean:
		make -C libft clean
		${RM} ${OBJS}

fclean:	clean
		rm -f libft/libft.a
		${RM} ${NAME}

re:		fclean all

test:	all
		@./malloc

strace:	all
		@strace ./malloc

.PHONY: all clean fclean re test strace
