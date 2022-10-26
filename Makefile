SRCS	=	Libft/libft.a \
				main.c

NAME	=	gmake

FLAGS	=	-Wall -Wextra -Werror

lib	:
	@make -C Libft

GCC	=	gcc

OBJS	=	${SRCS:.c=.o}

.c.o:
	${GCC} ${FLAGS} -c $< -o ${<:.c=.o}

all: ${NAME}

${NAME}: ${OBJS}lib
	${GCC} ${FLAGS} ${OBJS} -o ${NAME}

clean:
		rm -rf ${OBJS}

fclean: clean
		rm -rf ${NAME}

re:	fclean all

