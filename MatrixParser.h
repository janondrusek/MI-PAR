#ifndef MATRIXPARSER_H
#define	MATRIXPARSER_H

class MatrixParser {
public:
    MatrixParser(void);
    MatrixParser(int count, char** arguments);

    bool** getMatrix();
    int getMatrixSize();
    int getEdgesCount();
    virtual ~MatrixParser();
private:
    bool ** _matrix;
    int _edgesCount;
    int _matrixSize;
    char* _filename;

    void readMatrix();
    void allocateMatrix();
    bool isOne(char value);
};

#endif	

