CXX=g++
CXXFLAGS=-std=c++20 -O2 -Wall

all: main.o remote.o
	$(CXX) $(CXXFLAGS) -o main main.o
	$(CXX) $(CXXFLAGS) -o remote remote.o

receiver: main.o
	$(CXX) $(CXXFLAGS) -o main main.o

remote: remote.o
	$(CXX) $(CXXFLAGS) -o remote remote.o

clean:
	rm -f *.o remote main
