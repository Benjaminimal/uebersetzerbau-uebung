NAME = scanner

all: $(NAME)

$(NAME): $(NAME).c
	gcc -o $@ $< -lfl

$(NAME).c: $(NAME).l
	flex -o $@ $<

clean:
	rm -f $(NAME).c $(NAME)
