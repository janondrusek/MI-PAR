#ifndef MATRIXPARSER_H
#define	MATRIXPARSER_H

class MatrixParser {
public:
    MatrixParser(void);
    MatrixParser(int count, char** arguments);

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

