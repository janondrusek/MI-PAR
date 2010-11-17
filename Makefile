CXX     = mpiCC
LD      = mpiCC
CFLAGS  = -Wall -pedantic
DELFILE = rm -f

all: mi-par

mi-par: GraphStructure.o MatrixParser.o Node.o main.o 
	$(LD) -o mi-par main.o MatrixParser.o Node.o GraphStructure.o

main.o: main.cpp GraphStructure.h MatrixParser.h Node.h
	$(CXX) $(CFLAGS) -c -o main.o main.cpp

GraphStructure.o: GraphStructure.cpp GraphStructure.h
	$(CXX) $(CFLAGS) -c -o GraphStructure.o GraphStructure.cpp

Node.o: Node.cpp Node.h 
	$(CXX) $(CFLAGS) -c -o Node.o Node.cpp

MatrixParser.o: MatrixParser.cpp MatrixParser.h
	$(CXX) $(CFLAGS) -c -o MatrixParser.o MatrixParser.cpp

clean:
	$(DELFILE) main.o MatrixParser.o GraphStructure.o Node.o mi-par *.core
