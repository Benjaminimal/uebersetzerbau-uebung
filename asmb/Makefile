NAME = asmb
TEST = test

all: $(NAME).o

check: $(TEST)
	./$(TEST)

$(TEST): $(TEST).s $(NAME).s
	gcc -o $@ $^

$(TEST).s: $(TEST).c
	gcc -O -S $<

$(NAME).o: $(NAME).s
	gcc -c -o $@ $<

clean:
	rm -f *.o $(TEST) $(TEST).s
