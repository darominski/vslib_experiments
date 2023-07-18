CXX := g++
CXXFLAGS := -std=c++20 -O2
WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes

all: main remote

main: main.o
	$(CXX) $(CXXFLAGS) ${WARNINGS} -o main main.o

remote: remote.o
	$(CXX) $(CXXFLAGS) ${WARNINGS} -o remote remote.o

clean:
	rm -f *.o remote main
