
.SUFFIXES:
.SUFFIXES: .c .o

NAME = malloc

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
