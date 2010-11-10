#ifndef GRAPHSTRUCTURE_H
#define	GRAPHSTRUCTURE_H
#define LENGTH 1000

class GraphStructure {
public:
    GraphStructure(int* message);
    GraphStructure(bool** matrix, int edgesCount, int matrixSize);
    bool** getMatrix();
    int getEdgesCount();
    int getMatrixSize();
    void toMPIDataType(int *message);

    virtual ~GraphStructure();
private:
    bool ** _matrix;
    int _edgesCount;
    int _matrixSize;

    void allocateMatrix();
};

#endif	

