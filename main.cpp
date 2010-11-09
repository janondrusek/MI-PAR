/* 
 * File:   main.cpp
 * Author: magnusekm
 * Author: ondruja1
 *
 * Created on October 11, 2010, 10:33 PM
 */

#include "mpi.h"
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <stack>
#include "MatrixParser.h"
#include "GraphStructure.h"
#include <queue>

#define START 1
#define WORKING 2
#define WAITING 3
#define FINISHED 4

using namespace std;

void printMatrix(bool** matrix, int num) {
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            cout << matrix[i][j];
        }
        cout << "\n";
    }
}

vector<int> getNeighbours(bool * line, int num) {
    vector<int> neighbours;
    for (int i = 0; i < num; ++i) {
        if (line[i]) {
            neighbours.push_back(i);
        }
    }
    return neighbours;
}

int isAllVisited(int * visited, int num) {
    for (int i = 0; i < num; i++) {
        if (visited[i] == 0) {
            return i;
        }
    }
    return -1;
}

bool isBipartial(bool ** matrix, int num) {
    printf("in isBipartial\n");
    int current;
    int vertex;
    queue< int > q;
    int partition[num];
    int visited[num];

    for (int i = 0; i < num; ++i) {
        partition[i] = 0;
        visited[i] = 0;
    }

    int firstUnvisited = 0;
    while (isAllVisited(visited, num) >= 0) {
        firstUnvisited = isAllVisited(visited, num);
        q.push(firstUnvisited);

        partition[firstUnvisited] = 1; // first not visited
        visited[firstUnvisited] = 1; // first not visited
        while (!q.empty()) {
            current = q.front();
            q.pop();
            vector<int> neighbours = getNeighbours(matrix[current], num);

            for (int i = 0; i < neighbours.size(); i++) {
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

bool ** getMatrixCopy(bool ** original, int num) {
    bool ** matrix = (bool**) malloc(num * sizeof (bool*));
    bool * temp = (bool*) malloc(num * num * sizeof (bool));
    for (int i = 0; i < num; i++) {
        matrix[i] = temp + (i * num);
        for (int j = 0; j < num; ++j) {
            matrix[i][j] = original[i][j];
        }
    }
    return matrix;
}

bool **removeEdge(bool**original, int edgePosition, int num) {
    bool ** matrix = getMatrixCopy(original, num);
    int edgesCount = 0;
    for (int i = 0; i < num; ++i) {
        for (int j = i + 1; j < num; ++j) {
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
    cout << "!!! You never should've got here !!!";
}

GraphStructure* findWinner(MatrixParser *mp) {
    bool winnerSet = false;
    GraphStructure *winner;
    stack <GraphStructure *> matrices;
    matrices.push(new GraphStructure(mp->getMatrix(), mp->getEdgesCount(), mp->getMatrixSize()));

    while (!matrices.empty()) {
        GraphStructure * gs;
        gs = matrices.top();
        matrices.pop();
        if (winnerSet) {
            if (winner->getEdgesCount() >= gs->getEdgesCount() - 1) {
                delete gs;
                continue;
            }
        }

        if (!isBipartial(gs->getMatrix(), mp->getMatrixSize())) {
            for (int i = 0; i < gs->getEdgesCount(); ++i) {
                matrices.push(new GraphStructure(
                        removeEdge(gs->getMatrix(), i + 1, mp->getMatrixSize()),
                        gs->getEdgesCount() - 1, mp->getMatrixSize()));
            }
            delete gs;
        } else {
            if (!winnerSet) {
                winnerSet = true;
                winner = gs;

            } else {
                winner = gs;
            }

            printf("temporary best result: bipartial winner with edges count: %d\n",
                    winner->getEdgesCount());
        }
    }
    return winner;
}

void finalize(GraphStructure *winner) {
    printf("\nSearch finished. Final result is:\n");
    printf("Bipartial submatrix found with %d edges:\n", winner->getEdgesCount());

    printMatrix(winner->getMatrix(), winner->getMatrixSize());
}

void runBalancer(MatrixParser *mp, int processes) {
    if (isBipartial(mp->getMatrix(), mp->getMatrixSize())) {
        finalize(new GraphStructure(mp->getMatrix(), mp->getEdgesCount(),
                mp->getMatrixSize()));
    } else {
        stack <GraphStructure *> matrices;
        for (int i = 0; i < mp->getEdgesCount(); ++i) {
            matrices.push(new GraphStructure(
                    removeEdge(mp->getMatrix(), i + 1, mp->getMatrixSize()),
                    mp->getEdgesCount() - 1, mp->getMatrixSize()));
        }
        int lastWorker = 1;
        int* workerStatuses = new int[processes];
        for (int i = 1; i < processes; ++i) {
            workerStatuses[i] = START;
            MPI_Send(matrices.top()->toMPIDataType(), 0, MPI_PACKED, i, 1, MPI_COMM_WORLD);
            matrices.pop();
        }



    }
}

void runWorker(int myRank) {
    printf("Starting worker: %d\n", myRank);
    MPI_Status status;
    int workerStatus = START;
    stack <GraphStructure *> matrices;
    char buffer[LENGTH];

    MPI_Recv(buffer, LENGTH, MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    matrices.push(new GraphStructure(buffer));

    printf("worker %d received %d matrices\n", myRank, matrices.size());
}

int main(int argc, char** argv) {
    int myRank;
    int processes;

    /* start up MPI */
    MPI_Init(&argc, &argv);

    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &processes);

    cout << "my rank: " << myRank << "\n";

    if (myRank != 0) {
        runWorker(myRank);
    } else {
        MatrixParser *mp = new MatrixParser(argc, argv);
        runBalancer(mp, processes);
    }

    /* shut down MPI */
    MPI_Finalize();

    return 0;
}

