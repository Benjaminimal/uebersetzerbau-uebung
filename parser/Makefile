NAME = parser
PG = bison
CC = gcc

all: $(NAME)

$(NAME): $(NAME).tab.c
	$(CC) -o $@ $<

$(NAME).tab.c: $(NAME).y
	$(PG) $<

clean:
	rm -f $(NAME) $(NAME).tab.c
