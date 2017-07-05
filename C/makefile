
FLAGS = -pthread -lm
OBJS  = queue.c threadpool.c main.c
HEAD  = queue.h threadpool.h main.h
NAME  = main

$(NAME): $(OBJS) $(HEAD)
	gcc $(OBJS) -o $(NAME) $(FLAGS)

debug: $(OBJS) $(HEAD)
	gcc $(FLAGS) -g -o $(NAME) $(OBJS)