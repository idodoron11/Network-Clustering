FLAGS=-ansi -Wall -Wextra -Werror -pedantic-errors -c
LIBS=-lm

all: cluster.o defs.o division.o ErrorHandler.o graph.o LinkedList.o spmat.o VerticesGroup.o
	gcc cluster.o defs.o division.o ErrorHandler.o graph.o LinkedList.o spmat.o VerticesGroup.o -o cluster ${LIBS}

cluster.o: cluster.c spmat.h graph.h LinkedList.h division.h ErrorHandler.h
	gcc ${FLAGS} cluster.c

defs.o: defs.c
	gcc ${FLAGS} defs.c

division.o: division.c defs.h ErrorHandler.h
	gcc ${FLAGS} division.c

ErrorHandler.o: ErrorHandler.c
	gcc ${FLAGS} ErrorHandler.c

graph.o: graph.c ErrorHandler.h
	gcc ${FLAGS} graph.c

LinkedList.o: LinkedList.c ErrorHandler.h
	gcc ${FLAGS} LinkedList.c

spmat.o: spmat.c ErrorHandler.h
	gcc ${FLAGS} spmat.c

VerticesGroup.o: VerticesGroup.c defs.h ErrorHandler.h
	gcc ${FLAGS} VerticesGroup.c

clean:
	rm -rf *.o cluster