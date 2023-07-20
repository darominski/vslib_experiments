CXX := g++
WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-declarations \
            -Wredundant-decls -Winline -Wno-long-long \
            -Wconversion
CXXFLAGS := -std=c++20 -O0 $(WARNINGS)

all: main remote

main: main.o
	$(CXX) $(CXXFLAGS) -o main main.o

remote: remote.o
	$(CXX) $(CXXFLAGS) -o remote remote.o

assembly:
	$(CXX) $(CXXFLAGS) -S -o main.s main.cpp
	$(CXX) $(CXXFLAGS) -S -o remote.s remote.cpp

clean:
	rm -f *.o remote main
