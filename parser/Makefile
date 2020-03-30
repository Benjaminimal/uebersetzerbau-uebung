NAME = parser
SCANNER = scanner

PG_FLAGS = -dv
PG = bison $(PG_FLAGS)

LG = flex

CC_FLAGS = -std=c99 -pedantic -Wall -g
LD_FLAGS = -lfl
CC = gcc $(CC_FLAGS)

all: $(NAME)

$(NAME): $(SCANNER).c $(NAME).tab.c
	$(CC) -o $@ $^ $(LD_FLAGS)

$(NAME).tab.c: $(NAME).y
	$(PG) $<

$(SCANNER).c: $(SCANNER).l
	$(LG) -o $@ $<

clean:
	rm -f $(NAME) $(NAME).tab.c $(NAME).tab.h $(NAME).output $(SCANNER).c
