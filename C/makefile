
FLAGS = -lpthread
#OBJS  = threadpool.c main.c
#HEAD  = threadpool.h
OBJS  = main.c
NAME  = main

$(NAME): $(OBJS) $(HEAD)
	gcc $(FLAGS) -o $(NAME) $(OBJS)

debug: $(OBJS) $(HEAD)
	gcc $(FLAGS) -g -o $(NAME) $(OBJS)