FLAGS=-ansi -Wall -Wextra -Werror -pedantic-errors -c
LIBS=-lm

all: cluster.o spmat.o graph.o
	gcc cluster.o spmat.o graph.o -o cluster ${LIBS}

cluster.o: cluster.c spmat.h
	gcc ${FLAGS} cluster.c

spmat.o: spmat.c
	gcc ${FLAGS} spmat.c

graph.o: graph.c
	gcc ${FLAGS} graph.c

clean:
	rm -rf *.o cluster