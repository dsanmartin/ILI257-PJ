
FLAGS = -lpthread
OBJS  = queue.c threadpool.c main.c
HEAD  = queue.h threadpool.h
NAME  = main

$(NAME): $(OBJS) $(HEAD)
	gcc $(FLAGS) -o $(NAME) $(OBJS)

debug: $(OBJS) $(HEAD)
	gcc $(FLAGS) -g -o $(NAME) $(OBJS)