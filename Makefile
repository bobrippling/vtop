SRC = main.c mem.c nc.c ps.c ui.c proc.c argv.c io.c
OBJ = ${SRC:.c=.o}
DEPS = ${OBJ:%.o=.%.d}

CFLAGS = -std=c99 ${CFLAGS_CONFIGURE}
LDFLAGS = -lncurses ${LDFLAGS_CONFIGURE}

Q = @

all: tags vtop

vtop: ${OBJ}
	@echo link $@
	$Q${CC} -o $@ ${OBJ} ${LDFLAGS}

%.o: %.c
	@echo compile $<
	$Q${CC} -c -o $@ $< ${CFLAGS}

tags: ${SRC}
	@echo ctags
	$Q-ctags ${SRC} *.h

clean:
	rm -f vtop ${OBJ} ${DEPS}

.%.d: %.c
	@echo depend $<
	$Q${CC} -MM ${CFLAGS} $< > $@

-include ${OBJ:%.o=.%.d}
-include config.mk

.PHONY: all clean
