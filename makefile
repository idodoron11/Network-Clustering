FLAGS=-ansi -Wall -Wextra -Werror -pedantic-errors -c
LIBS=-lm

all: cluster.o
	gcc cluster.o -o cluster ${LIBS}

cluster.o: cluster.c
	gcc ${FLAGS} cluster.c

clean:
	rm -rf *.o cluster