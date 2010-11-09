#include "MatrixParser.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string>

using namespace std;

MatrixParser::MatrixParser(void) {
    _edgesCount = 0;
    _matrixSize = 0;
};

MatrixParser::MatrixParser(int count, char** arguments) {
    if (count != 2) {
        cerr << "Usage: " << arguments[0] << " FILE_NAME\n";
        exit(1);
    } else {
        cerr << "***********************\n";
        for (int i = 0; i < count; ++i) {
            cout << i << ": " << arguments[i] << "\n";
        }
        cerr << "***********************\n";
    }
    _filename = arguments[1];
    MatrixParser();
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

    int line = 0;
    while (getline(inFile, str)) {
        for (int pos = 0; pos < _matrixSize; ++pos) {
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
    bool* temp;

    _matrix = (bool**) malloc(_matrixSize * sizeof (bool*));
    temp = (bool*) malloc(_matrixSize * _matrixSize * sizeof (bool));
    for (int i = 0; i < _matrixSize; i++) {
        _matrix[i] = temp + (i * _matrixSize);
    }
}

int MatrixParser::getMatrixSize() {
    return _matrixSize;
}

int MatrixParser::getEdgesCount() {
    return _edgesCount;
}

bool MatrixParser::isOne(char value) {
    return value == '1';
}

MatrixParser::~MatrixParser() {
};

