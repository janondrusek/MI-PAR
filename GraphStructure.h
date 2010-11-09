#ifndef GRAPHSTRUCTURE_H
#define	GRAPHSTRUCTURE_H
#define LENGTH 1000

class GraphStructure {
public:
    GraphStructure(void);
    GraphStructure(char* message);
    GraphStructure(bool** matrix, int edgesCount, int matrixSize);
    bool** getMatrix();
    int getEdgesCount();
    int getMatrixSize();
    char* toMPIDataType();

    virtual ~GraphStructure();
private:
    bool ** _matrix;
    char _buffer[LENGTH];
    int _edgesCount;
    int _matrixSize;
    int _position;
};

#endif	

