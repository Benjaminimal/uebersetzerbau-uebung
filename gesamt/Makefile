NAME = gesamt
GENERATOR = codegen
PARSER = parser
SCANNER = scanner
OXOUT = oxout

LIBS = sym_tab.c ast.c x86.c

AGG = ox

PG = bison $(PG_FLAGS)
PG_FLAGS = -dv

LG = flex

CC = gcc $(CC_FLAGS) $(U_FLAGS)
# CC_FLAGS = -std=c99 -pedantic -Wall -g
CC_FLAGS = -g -DUSE_IBURG
LD_FLAGS = -lfl

all: $(NAME)

$(NAME): $(PARSER).c $(SCANNER).c $(GENERATOR).c $(LIBS)
	$(CC) -o $@ $^ $(LD_FLAGS)

$(PARSER).c: $(OXOUT).y
	$(PG) -o $@ $<

$(SCANNER).c: $(OXOUT).l
	$(LG) -o $@ $<

$(OXOUT).y $(OXOUT).l: $(PARSER).y $(SCANNER).l
	$(AGG) $^

$(GENERATOR).c: $(GENERATOR).bfe
	./my_bfe < $< | iburg -I > $@

clean:
	rm -f $(NAME) $(GENERATOR).c $(OXOUT).y $(OXOUT).l $(OXOUT).h $(PARSER) $(PARSER).c $(PARSER).h $(PARSER).output $(SCANNER).c test.out
