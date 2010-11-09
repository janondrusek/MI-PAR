#include "GraphStructure.h"
#include "mpi.h"

GraphStructure::GraphStructure(void) {
    _position = 0;
};

GraphStructure::GraphStructure(char * message) {
    _position = 0;
    MPI_Unpack(_buffer, LENGTH, &_position, &_edgesCount, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(_buffer, LENGTH, &_position, &_matrixSize, 1, MPI_INT, MPI_COMM_WORLD);
    for (int i = 0; i < _matrixSize; ++i) {
        for (int j = 0; j < _matrixSize; ++j) {
            MPI_Unpack(_buffer, LENGTH, &_position, &_matrix[i][j], 1, MPI_CHAR, MPI_COMM_WORLD);
        }
    }
}

GraphStructure::GraphStructure(bool **matrix, int edgesCount, int matrixSize) {
    _position = 0;
    _matrix = matrix;
    _edgesCount = edgesCount;
    _matrixSize = matrixSize;
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

char * GraphStructure::toMPIDataType() {
    MPI_Pack(&_edgesCount, 1, MPI_INT, _buffer, LENGTH, &_position, MPI_COMM_WORLD);
    MPI_Pack(&_matrixSize, 1, MPI_INT, _buffer, LENGTH, &_position, MPI_COMM_WORLD);
    for (int i = 0; i < _matrixSize; ++i) {
        for (int j = 0; j < _matrixSize; ++j) {
            MPI_Pack(&_matrix[i][j], 1, MPI_CHAR, _buffer, LENGTH, &_position, MPI_COMM_WORLD);
        }
    }
}

GraphStructure::~GraphStructure() {
    delete [] * _matrix;
    delete [] _matrix;
}
