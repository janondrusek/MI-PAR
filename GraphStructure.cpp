#include "GraphStructure.h"

GraphStructure::GraphStructure() {
}

GraphStructure::GraphStructure(bool **matrix, int edgesCount) {
    _matrix = matrix;
    _edgesCount = edgesCount;
}

bool** GraphStructure::getMatrix() {
    return _matrix;
}

int GraphStructure::getEdgesCount() {
    return _edgesCount;
}

GraphStructure::~GraphStructure() {
    delete [] * _matrix;
    delete [] _matrix;
}

