SRC = main.c nc.c ui.c ps.c
OBJ = ${SRC:.c=.o}

LDFLAGS = -lncurses

vtop: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}
