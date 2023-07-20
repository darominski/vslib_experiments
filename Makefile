CXX := g++
WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-declarations \
            -Wredundant-decls -Winline -Wno-long-long \
            -Wconversion
INCLUDES := ./lib
CXXFLAGS := -std=c++20 -O0 $(WARNINGS) -I$(INCLUDES)

all: main remote

main: main.o parameterRegistry.o
	$(CXX) $(CXXFLAGS) -o main main.o parameterRegistry.o

remote: remote.o
	$(CXX) $(CXXFLAGS) -o remote remote.o

assembly:
	$(CXX) $(CXXFLAGS) -S -o main.s main.cpp
	$(CXX) $(CXXFLAGS) -S -o remote.s remote.cpp

clean:
	rm -f *.o remote main
