#include "MatrixParser.h"

using namespace std;

MatrixParser::MatrixParser(void) {
    _edgesCount = 0;
    _matrixSize = 0;
}

MatrixParser::MatrixParser(int count, char** arguments) {
    if (count != 2) {
        cerr << "Usage: " << arguments[0] << " FILE_NAME\n";
        exit(1);
    } else {
        cout << "***********************\n";
        for (int i = 0; i < count; ++i) {
            cout << i << ": " << arguments[i] << "\n";
        }
        cout << "***********************\n";
    }
    _filename = arguments[1];
    _edgesCount = 0;
    _matrixSize = 0;

    readMatrix();
}

bool** MatrixParser::getMatrix() {
    return _matrix;
}

void MatrixParser::readMatrix() {
    string str;
    ifstream inFile;

    inFile.open(_filename);
    if (!inFile) {
        cerr << "Unable to open file: " << _filename << "\n";
        exit(1); // call system to stop
    }
    getline(inFile, str);
    istringstream is(str);
    is >> _matrixSize;
    allocateMatrix();

    unsigned int line = 0;
    while (getline(inFile, str)) {
        for (unsigned int pos = 0; pos < _matrixSize; ++pos) {
            char value = str.at(pos);
            bool one = isOne(value);
            _matrix[line][pos] = one;
            if ((pos > line) && one) {
                ++_edgesCount;
            }
        }
        line++;
    }
    inFile.close();
}

void MatrixParser::allocateMatrix() {
    _matrix = new bool*[_matrixSize];
    for (int i = 0; i < _matrixSize; ++i) {
        _matrix[i] = new bool[_matrixSize];
        for (int j = 0; j < _matrixSize; ++j) {
            _matrix[i][j] = _matrix[i][j];
        }
    }
}

unsigned int MatrixParser::getMatrixSize() {
    return _matrixSize;
}

unsigned int MatrixParser::getEdgesCount() {
    return _edgesCount;
}

GraphStructure * MatrixParser::getGraphStructure() {
    GraphStructure * gs = new GraphStructure(getMatrix(), getEdgesCount(), getMatrixSize());
    return gs;
}

bool MatrixParser::isOne(char value) {
    return value == '1';
}

MatrixParser::~MatrixParser() {
}

