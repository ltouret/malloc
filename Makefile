ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_${HOSTTYPE}.so
SLINK = libft_malloc.so

SRCS = malloc.c realloc.c shared.c free.c show_alloc_mem.c

OBJS = ${SRCS:.c=.o}

CC		= cc
RM		= rm -f
LN		= ln -s
CP		= cp

CFLAGS = -Wall -Wextra -Werror -fPIC

.c.o:
		${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

$(NAME): ${OBJS}
		${CC} -shared -o ${NAME} ${OBJS}
		${LN} ${NAME} ${SLINK}

all:	${NAME}

clean:
		${RM} ${OBJS}

fclean:	clean
		${RM} ${NAME} ${SLINK}

re:		fclean all

install:
		${CP} libft_malloc_*.so /usr/lib/
		${LN} /usr/lib/libft_malloc_*.so /usr/lib/${SLINK}

.PHONY : all clean fclean re
