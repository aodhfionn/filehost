CC=gcc
BIN=server.o
ARGS=-std=c17

build:
	$(CC) src/*.c -o $(BIN) $(ARGS)

test: build
	./$(BIN)