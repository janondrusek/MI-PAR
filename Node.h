#ifndef NODE_H
#define	NODE_H

#include "GraphStructure.h"
#include "mpi.h"
#include <stack>
#include <stdio.h>
#include <iostream>

#define START 1
#define WORKING 2
#define WAITING 3
#define FINALIZE 4
#define FINALIZE_RESPONSE 5
#define WALKER_W 6
#define WALKER_B 7

#define WORK 10
#define WINNER_UPDATE 11
#define WORK_REQUEST 12
#define WORK_RESPONSE 13
#define WORK_RESPONSE_EMPTY 14

class Node {
public:
    Node(void);
    Node(int rank, int processes);
    Node(std::stack <GraphStructure *> matrices, int processes);
    GraphStructure * run();

    virtual ~Node();
private:
    GraphStructure * _winner;
    int _rank;
    int _processes;
    int _winnerEdgesCount;
    int _currentRank;
    std::stack <GraphStructure *> _matrices;

    void appendResponse(GraphStructure * response);
    void initEmptyWinner();
    int getNextNode();
    void findWinner();
    void work();
};

#endif	

