COMPILER?=clang++
SRCS=$(wildcard src/*.cpp)
HDRS=$(wildcard src/*.hpp)
OBJS=$(patsubst %.c,%.o,$(SRC))

comp:
	clang++ -std=c++14 -stdlib=libc++ -O2 -o np -I/usr/local/Cellar/tclap/1.2.1/include -L/usr/local/Cellar/tclap/1.2.1/lib $(SRCS)

run:
	./np

clean:
	rm np

