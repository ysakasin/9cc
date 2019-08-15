CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

.PHONY: test clean run

run:
	scp tmp.s vps:~/
	ssh vps gcc -o tmp tmp.s
	ssh vps ./tmp

test: 9cc
	./9cc -test
	./test.sh

clean:
	rm -f 9cc *.o *~
