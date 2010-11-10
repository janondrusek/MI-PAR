#include "GraphStructure.h"
#include "mpi.h"
#include <stdio.h>
#include <iostream>

using namespace std;

GraphStructure::GraphStructure(int * message) {
    _edgesCount = message[0];
    _matrixSize = message[1];
    allocateMatrix();

    for (int i = 0; i < _matrixSize; ++i) {
        for (int j = 0; j < _matrixSize; ++j) {
            _matrix[i][j] = message[2 + (i * _matrixSize) + j];
        }
    }
}

GraphStructure::GraphStructure(bool **matrix, int edgesCount, int matrixSize) {
    _matrix = matrix;
    _edgesCount = edgesCount;
    _matrixSize = matrixSize;
}

void GraphStructure::allocateMatrix() {
    bool* temp;

    _matrix = (bool**) malloc(_matrixSize * sizeof (bool*));
    temp = (bool*) malloc(_matrixSize * _matrixSize * sizeof (bool));
    for (int i = 0; i < _matrixSize; i++) {
        _matrix[i] = temp + (i * _matrixSize);
    }
}

bool** GraphStructure::getMatrix() {
    return _matrix;
}

int GraphStructure::getEdgesCount() {
    return _edgesCount;
}

int GraphStructure::getMatrixSize() {
    return _matrixSize;
}

void GraphStructure::toMPIDataType(int *message) {
    message[0] = _edgesCount;
    message[1] = _matrixSize;
    for (int i = 0; i < _matrixSize; ++i) {
        for (int j = 0; j < _matrixSize; ++j) {
            message[2 + (i * _matrixSize) + j] = _matrix[i][j];
        }
    }
}

GraphStructure::~GraphStructure() {
    delete [] * _matrix;
    delete [] _matrix;
}
