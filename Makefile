cfile=cube.c objects.c object_common.c own_signal.c
finial=cube
CFLAGS=-std=gnu99  -Wall -lncurses -pthread

all:$(finial)

$(finial):$(cfile:.c=.o)
	gcc $(CFLAGS) $^ -o $@

%.o:%.c 
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm *.o

.PHONY: clean all
