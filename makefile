FLAGS=-ansi -Wall -Wextra -Werror -pedantic-errors -c
LIBS=-lm

all: cluster.o spmat.o
	gcc cluster.o -o cluster ${LIBS}

cluster.o: cluster.c spmat.h
	gcc ${FLAGS} cluster.c

spmat.o: spmat.c
	gcc ${FLAGS} spmat.c

clean:
	rm -rf *.o cluster