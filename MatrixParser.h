#ifndef MATRIXPARSER_H
#define	MATRIXPARSER_H

#include "GraphStructure.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string>

class MatrixParser {
public:
    MatrixParser(void);
    MatrixParser(int count, char** arguments);

    GraphStructure * getGraphStructure();
    bool** getMatrix();
    unsigned int getMatrixSize();
    unsigned int getEdgesCount();
    virtual ~MatrixParser();
private:
    bool ** _matrix;
    unsigned int _edgesCount;
    unsigned int _matrixSize;
    char* _filename;

    void readMatrix();
    void allocateMatrix();
    bool isOne(char value);
};

#endif	

