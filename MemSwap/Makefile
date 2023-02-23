CC = gcc
OUTFILE = MEMSWAP.out

### Reglas ###

SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
OBJ = $(SRC:%.c=./obj/%.o)


build: makefolder $(OBJ)
	$(CC) -Wall main.c -o $(OUTFILE) $(OBJ) $(LINK)

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind.log ./$(OUTFILE)

LINK = -lcommons -lpthread -lm
CFLAGS = -c

debug: CFLAGS := $(CFLAGS) -g

./obj/%.o: ./src/%.c
	$(CC) $(CFLAGS) $< -o $@ $(LINK)

makefolder:
	if [ ! -d ./obj ]; then mkdir obj; fi

# Clean

clean:
	rm -rf obj
	rm -rf *.out
	rm -rf *.log