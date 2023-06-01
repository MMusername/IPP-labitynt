CC = gcc
FLAGS = -Wall -Wextra -Wno-implicit-fallthrough -std=c17 -O2

all: labyrinth

labyrinth: main.o get_data.o path_finding.o queue.o
	${CC} -o labyrinth main.o get_data.o path_finding.o queue.o

get_data.o: get_data.c get_data.h
	${CC} ${CFLAGS} -c get_data.c

queue.o: queue.c queue.h
	${CC} ${CFLAGS} -c queue.c

path_finding.o: path_finding.c path_finding.h get_data.h queue.h cube_struct.h
	${CC} ${CFLAGS} -c path_finding.c

main.o: main.c path_finding.h
	${CC} ${CFLAGS} -c main.c

clean:
	rm *.o
	rm labyrinth

