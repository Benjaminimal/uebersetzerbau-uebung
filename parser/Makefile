NAME = parser
SCANNER = scanner

PG_FLAGS = -dv
PG = bison $(PG_FLAGS)

LG = flex

CC = gcc

all: $(NAME)

$(NAME): $(SCANNER).c $(NAME).tab.c
	$(CC) -o $@ $^

$(NAME).tab.c: $(NAME).y
	$(PG) $<

$(SCANNER).c: $(SCANNER).l
	$(LG) -o $@ $<

clean:
	rm -f $(NAME) $(NAME).tab.c $(NAME).tab.h $(NAME).output $(SCANNER).c
