#ifndef GRAPHSTRUCTURE_H
#define	GRAPHSTRUCTURE_H

class GraphStructure {
public:
    GraphStructure();
    GraphStructure(bool** matrix, int edgesCount);
    bool** getMatrix();
    int getEdgesCount();
    virtual ~GraphStructure();
private:
    bool ** _matrix;
    int _edgesCount;
};

#endif	

