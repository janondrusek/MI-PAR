#ifndef GRAPHSTRUCTURE_H
#define	GRAPHSTRUCTURE_H

#include "mpi.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <queue>
#include <vector>

#define LENGTH 1000

class GraphStructure {
public:
    GraphStructure(int* message);
    GraphStructure(bool** matrix, int edgesCount, int matrixSize);
    bool** getMatrix();
    int getEdgesCount();
    int getMatrixSize();
    void toMPIDataType(int *message);
    bool isBipartial();
    bool ** removeEdge(int edgePosition);

    virtual ~GraphStructure();
private:
    bool ** _matrix;
    int _edgesCount;
    int _matrixSize;

    void allocateMatrix();
    int isAllVisited(int * visited);
    std::vector<int> getNeighbours(bool * line);
    bool** getMatrixCopy();
};

#endif	

