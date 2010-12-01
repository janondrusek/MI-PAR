#include "GraphStructure.h"

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
    _matrix = new bool*[_matrixSize];
    for (unsigned int i = 0; i < _matrixSize; ++i) {
        _matrix[i] = new bool[_matrixSize];
        for (unsigned int j = 0; j < _matrixSize; ++j) {
            _matrix[i][j] = _matrix[i][j];
        }
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

vector<int> GraphStructure::getNeighbours(bool * line) {
    vector<int> neighbours;
    for (int i = 0; i < _matrixSize; ++i) {
        if (line[i]) {
            neighbours.push_back(i);
        }
    }
    return neighbours;
}

int GraphStructure::isAllVisited(int * visited) {
    for (int i = 0; i < _matrixSize; i++) {
        if (visited[i] == 0) {
            return i;
        }
    }
    return -1;
}

bool GraphStructure::isBipartial() {
    int current;
    int vertex;
    queue< int > q;
    int* partition = new int[_matrixSize];
    int* visited = new int[_matrixSize];

    for (int i = 0; i < _matrixSize; ++i) {
        partition[i] = 0;
        visited[i] = 0;
    }

    int firstUnvisited = 0;
    while (isAllVisited(visited) >= 0) {
        firstUnvisited = isAllVisited(visited);
        q.push(firstUnvisited);

        partition[firstUnvisited] = 1; // first not visited
        visited[firstUnvisited] = 1; // first not visited
        while (!q.empty()) {
            current = q.front();
            q.pop();
            vector<int> neighbours = getNeighbours(_matrix[current]);

            for (unsigned int i = 0; i < neighbours.size(); i++) {
                vertex = neighbours[i];
                if (partition[current] == partition[vertex]) return false;
                if (visited[vertex] == 0) {
                    visited[vertex] = 1;
                    partition[vertex] = 3 - partition[current]; // alter 1 and 2
                    q.push(vertex);
                }
            }
        }
    }
    return true;
}

bool ** GraphStructure::getMatrixCopy() {
    bool **matrix = new bool*[_matrixSize];
    for (int i = 0; i < _matrixSize; ++i) {
        matrix[i] = new bool[_matrixSize];
        for (int j = 0; j < _matrixSize; ++j) {
            matrix[i][j] = _matrix[i][j];
        }
    }
    return matrix;
}

bool ** GraphStructure::removeEdge(int edgePosition) {
    bool ** matrix = getMatrixCopy();
    int edgesCount = 0;
    for (int i = 0; i < _matrixSize; ++i) {
        for (int j = i + 1; j < _matrixSize; ++j) {
            if (matrix[i][j]) {
                ++edgesCount;
                if (edgePosition == edgesCount) {
                    matrix[i][j] = 0;
                    matrix[j][i] = 0;
                    return matrix;
                }
            }
        }
    }
    cerr << "!!! You never should've got here !!!\n";
    exit(1);
}

GraphStructure::~GraphStructure() {
    for (int i = 0; i < _matrixSize; i++) {
        delete [] _matrix[i];
    }
    delete [] _matrix;
}
