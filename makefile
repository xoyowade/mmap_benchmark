PROG=mmap
CC=g++
CFLAGS=-O2 -g -Wall -std=c++11
LDFLAGS=-lbenchmark -pthread
SRCS=${wildcard *.cc}
OBJS=${SRCS:.cc=.o}

all: ${PROG} #${PROG}.s

clean:
	rm -f ${PROG} ${PROG}.s *.o

%.o: %.cc
	${CC} -c -o $@ $< ${CFLAGS} 

${PROG}: ${OBJS}
	${CC} $^ -o $@ ${LDFLAGS}

${PROG}.s: ${PROG}
	objdump -D -j .text -S -l ${PROG} | c++filt > $@

