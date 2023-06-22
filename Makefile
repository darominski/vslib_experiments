CXX=g++

all: receiver.o remote.o
	$(CXX) -o receiver receiver.o
	$(CXX) -o remote remote.o

receiver: receiver.o
	$(CXX) -o receiver receiver.o

remote: remote.o
	$(CXX) -o remote remote.o

clean:
	rm -f *.o remote receiver
