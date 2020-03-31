NAME = asmb

all: $(NAME).o

$(NAME).o: $(NAME).s
	gcc -c -o $@ $<

clean:
	rm -f *.o
