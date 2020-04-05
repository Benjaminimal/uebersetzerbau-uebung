NAME = ag
PARSER = parser
SCANNER = scanner
OXOUT = oxout

AGG = ox

PG = bison $(PG_FLAGS)
PG_FLAGS = -dv

LG = flex

CC = gcc $(CC_FLAGS)
# CC_FLAGS = -std=c99 -pedantic -Wall -g
CC_FLAGS = -pedantic -Wall -g
LD_FLAGS = -lfl

all: $(NAME)

$(NAME): $(PARSER).c $(SCANNER).c id_list.c
	$(CC) -o $@ $^ $(LD_FLAGS)

$(PARSER).c: $(OXOUT).y
	$(PG) -o $@ $<

$(SCANNER).c: $(OXOUT).l
	$(LG) -o $@ $<

$(OXOUT).y $(OXOUT).l: $(PARSER).y $(SCANNER).l
	$(AGG) $^

clean:
	rm -f $(NAME) $(OXOUT).y $(OXOUT).l $(PARSER) $(PARSER).c $(PARSER).h $(PARSER).output $(SCANNER).c
