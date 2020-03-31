NAME = scanner

all: $(NAME)

$(NAME): $(NAME).c
	gcc -std=c99 -pedantic -Wall -g -o $@ $< -lfl

$(NAME).c: $(NAME).l
	flex -o $@ $<

clean:
	rm -f $(NAME).c $(NAME)
