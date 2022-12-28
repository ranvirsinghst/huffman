CC = clang
CFLAGS = -O3 -Wall -Werror -Wextra -Wpedantic $(shell pkg-config --cflags gmp)

all: encode decode

encode: code.c encode.c huffman.c io.c node.c pq.c stack.c
	$(CC) -g -o $@ $^

decode: code.c decode.c huffman.c io.c node.c pq.c stack.c
	$(CC) -g -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -g -c $<

clean:
	rm -f *.o

spotless: clean
	rm -f encode decode *.o

format:
	clang-format -i -style=file *.c *.h
