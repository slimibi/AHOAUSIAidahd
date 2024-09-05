##
## Made by PABLOleBG
##

SRC		=	src/main.c	\
			src/request.c	\
			src/client.c

NAME	=	server

OBJ		=	$(SRC:.c=.o)

 CFLAGS	=	-I include/

all:		$(NAME)

$(NAME):	$(OBJ)
# make -C lib/my
# make clean -C lib/my
		gcc -o $(NAME) $(OBJ) $(CFLAGS)

clean:
		rm -rf *~
		rm -f $(OBJ)
		rm -rf src/*~

fclean:		clean
# make fclean -C lib/my
		rm -f $(NAME)

re:		fclean all

.PHONY:		all clean fclean
