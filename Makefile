CXX     = mpiCC
LD      = mpiCC
CFLAGS  = -Wall -pedantic
DELFILE = rm -f

all: mi-par

mi-par: MatrixParser.o GraphStructure.o main.o 
	$(LD) -o mi-par main.o MatrixParser.o GraphStructure.o

main.o: main.cpp MatrixParser.h GraphStructure.h
	$(CXX) $(CFLAGS) -c -o main.o main.cpp

GraphStructure.o: GraphStructure.cpp GraphStructure.h
	$(CXX) $(CFLAGS) -c -o GraphStructure.o GraphStructure.cpp

MatrixParser.o: MatrixParser.cpp MatrixParser.h
	$(CXX) $(CFLAGS) -c -o MatrixParser.o MatrixParser.cpp

clean:
	$(DELFILE) main.o MatrixParser.o GraphStructure.o mi-par *.core
