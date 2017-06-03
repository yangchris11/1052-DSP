.PHONY: all clean

CC = g++
CFLAGS = -O3
LDFLAGS+=-lm     # link to math library

TARGET=test_hmm

all : train test

train : train.o
	${CC} train.o ${CFLAGS} -o train
train.o : train.cpp
	${CC} train.cpp ${CFLAGS} -c

test : test.o
	${CC} test.o ${CFLAGS} -o test
test.o : test.cpp
	${CC} test.cpp ${CFLAGS} -c

clean :
	rm -rf *.o
	rm -f train test 

#all: $(TARGET)
# type make/make all to compile test_hmm

#clean:
#	$(RM) $(TARGET)   # type make clean to remove the compiled file
