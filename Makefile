CXX=g++

all: main.o remote.o
	$(CXX) -o main main.o
	$(CXX) -o remote remote.o

receiver: main.o
	$(CXX) -o main main.o

remote: remote.o
	$(CXX) -o remote remote.o

clean:
	rm -f *.o remote main
