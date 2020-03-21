NAME = scanner

all: $(NAME)

$(NAME): $(NAME).c
	gcc -o $@ $< -lfl

$(NAME).c: $(NAME).lex
	flex -o $@ $<

clean:
	rm -f $(NAME).c $(NAME)
