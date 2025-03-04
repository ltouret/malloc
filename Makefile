ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_${HOSTTYPE}.so
SLINK = libft_malloc.so
EXEC_TEST = test_malloc

SRCS = src/malloc.c src/realloc.c src/shared.c src/free.c src/show_alloc_mem.c

OBJS = ${SRCS:.c=.o}

CC		= cc
RM		= rm -f
LN		= ln -s
CP		= cp

CFLAGS = -Wall -Wextra -Werror -Iinclude -fPIC

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
		${RM} ${EXEC_TEST}

re:		fclean all

install:
		${CP} libft_malloc_*.so /usr/lib/
		${LN} /usr/lib/libft_malloc_*.so /usr/lib/${SLINK}

test:   all
		${CC} ${CFLAGS} -l ft_malloc -L. main.c -o ${EXEC_TEST}
		LD_LIBRARY_PATH=. ./${EXEC_TEST}

.PHONY : all clean fclean re install test
