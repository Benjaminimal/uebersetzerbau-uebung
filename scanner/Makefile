NAME = scanner

all: $(NAME)

$(NAME): $(NAME).c
	gcc -o $@ $< -lfl

$(NAME).c: $(NAME).lex
	flex -o $@ $<

clean:
	rm $(NAME).c $(NAME)
