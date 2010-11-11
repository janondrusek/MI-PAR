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

#define WORK 1
#define WINNER_UPDATE 2
#define WORK_REQUEST 3
#define WORK_RESPONSE 3

using namespace std;

void printMatrix(bool** matrix, int num) {
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            cout << matrix[i][j];
        }
        cout << "\n";
    }
}

void printMessage(int * message) {
    for (int i = 0; i < LENGTH; i++) {
        cout << message[i] << "_";
    }
    cout << "\n";
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

bool isBipartial(bool ** matrix, unsigned int num) {
    int current;
    int vertex;
    queue< int > q;
    int* partition = new int[num];
    int* visited = new int[num];

    for (unsigned int i = 0; i < num; ++i) {
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

            for (unsigned int i = 0; i < neighbours.size(); i++) {
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
    cerr << "!!! You never should've got here !!!\n";
    exit(1);
}

GraphStructure* findWinner(GraphStructure * matrix) {
    bool winnerSet = false;
    MPI_Status status;
    int flag;
    GraphStructure *winner;
    stack <GraphStructure *> matrices;
    matrices.push(matrix);
    unsigned int iteration = 1;
    while (!matrices.empty()) {
        if (iteration % 100 == 0) {
            //send worker winner if I have winner
            if (winnerSet) {
                int message[LENGTH];
                winner->toMPIDataType(message);
                MPI_Send(message, LENGTH, MPI_INT, 0, WINNER_UPDATE, MPI_COMM_WORLD);
            }


            //receive winner from balancer
            MPI_Iprobe(0, WINNER_UPDATE, MPI_COMM_WORLD, &flag, &status);
            if (flag) {
                int message[LENGTH];
                /* receiving message by blocking receive */
                MPI_Recv(message, LENGTH, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                int tag = status.MPI_TAG;
                if (tag == WINNER_UPDATE) {
                    winner = new GraphStructure(message);
                } else if (tag == WORK_REQUEST && matrices.size() > 20) {
                    matrices.top()->toMPIDataType(message);
                    MPI_Send(message, LENGTH, MPI_INT, 0, WORK_RESPONSE, MPI_COMM_WORLD);
                    matrices.pop();
                }
            }
            iteration = 1;
        }
        GraphStructure * gs;
        gs = matrices.top();
        matrices.pop();
        if (winnerSet) {
            if (winner->getEdgesCount() >= gs->getEdgesCount() - 1) {
                delete gs;
                continue;
            }
        }

        if (!isBipartial(gs->getMatrix(), gs->getMatrixSize())) {
            for (int i = 0; i < gs->getEdgesCount(); ++i) {
                matrices.push(new GraphStructure(
                        removeEdge(gs->getMatrix(), i + 1, gs->getMatrixSize()),
                        gs->getEdgesCount() - 1, gs->getMatrixSize()));
            }
            delete gs;
        } else {
            if (!winnerSet) {
                winnerSet = true;
                winner = gs;
                printf("Found winner with edges: %d\n", winner->getEdgesCount());
            } else {
                winner = gs;
            }
        }
        iteration++;
    }
    return winner;
}

void finalize(GraphStructure *winner, int processes) {
    GraphStructure * finalizeMessage = new GraphStructure(winner->getMatrix(), -1, 0);
    for (int i = 1; i < processes; ++i) {
        printf("Balancer sending finalizeMessage to worker %d\n", i);
        int message[LENGTH];
        finalizeMessage->toMPIDataType(message);
        MPI_Send(message, LENGTH, MPI_INT, i, WORK, MPI_COMM_WORLD);
    }

    printf("\nSearch finished. Final result is:\n");
    printf("Bipartial submatrix found with %d edges:\n", winner->getEdgesCount());

    printMatrix(winner->getMatrix(), winner->getMatrixSize());
}

int getFirstWaitingWorker(int * workerStatuses, int processes) {
    for (int i = 1; i < processes; ++i) {
        if (workerStatuses[i] == WAITING) {
            return i;
        }
    }
    return 0;
}

bool isAnyWorkerWorking(int * workerStatuses, int processes) {
    for (int i = 1; i < processes; ++i) {
        if (workerStatuses[i] == WORKING) {
            return true;
        }
    }
    return false;
}

void runBalancer(MatrixParser *mp, int processes) {
    bool winnerSet = false;
    GraphStructure * winner;
    MPI_Status status;

    if (isBipartial(mp->getMatrix(), mp->getMatrixSize())) {
        finalize(new GraphStructure(mp->getMatrix(), mp->getEdgesCount(),
                mp->getMatrixSize()), processes);
    } else {
        stack <GraphStructure *> matrices;
        printf("runBalancer fill matrix, edges: %d\n", mp->getEdgesCount());
        for (unsigned int i = 0; i < mp->getEdgesCount(); ++i) {
            matrices.push(new GraphStructure(
                    removeEdge(mp->getMatrix(), i + 1, mp->getMatrixSize()),
                    mp->getEdgesCount() - 1, mp->getMatrixSize()));
        }
        int* workerStatuses = new int[processes];
        for (int i = 1; i < processes; ++i) {
            if (!matrices.empty()) {
                int message[LENGTH];
                matrices.top()->toMPIDataType(message);
                MPI_Send(message, LENGTH, MPI_INT, i, WORK, MPI_COMM_WORLD);
                matrices.pop();
                workerStatuses[i] = WORKING;
            } else {
                workerStatuses[i] = WAITING;
            }
        }

        while (isAnyWorkerWorking(workerStatuses, processes)) {
            //            int firstWaiting = getFirstWaitingWorker(workerStatuses, processes);
            //            if (firstWaiting > 0 && !matrices.empty()) {
            //                int message[LENGTH];
            //                matrices.top()->toMPIDataType(message);
            //                MPI_Send(message, LENGTH, MPI_INT, firstWaiting, WORK, MPI_COMM_WORLD);
            //                matrices.pop();
            //                workerStatuses[firstWaiting] = WORKING;
            //                continue;
            //            }

            GraphStructure * matrix;
            int message[LENGTH];
            MPI_Recv(message, LENGTH, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            int workerRank = status.MPI_SOURCE;
            int workerTag = status.MPI_TAG;
            matrix = new GraphStructure(message);

            // Prijem nedodelane prace od workera na zaklade pozadavku
            if (workerTag == WORK_RESPONSE) {
                int firstWaiting = getFirstWaitingWorker(workerStatuses, processes);
                if (firstWaiting > 0) {
                    matrix->toMPIDataType(message);
                    MPI_Send(message, LENGTH, MPI_INT, firstWaiting, WORK, MPI_COMM_WORLD);
                    workerStatuses[firstWaiting] = WORKING;
                } else {
                    matrices.push(matrix);
                }
                continue;
            }
            // Preskocime aktualizaci winnera a update winnera pro workery

            if (!winnerSet || matrix->getEdgesCount() > winner->getEdgesCount()) {
                winner = matrix;
                printf("Temporary Bipartial with edges count: %d\n", winner->getEdgesCount());
                winnerSet = true;
            } else {
                delete matrix;
            }

            // Worker zaslal vysledek, oznacime jako cekajiciho na praci
            if (workerTag == FINISHED) {
                workerStatuses[workerRank] = WAITING;
                if (!matrices.empty()) {
                    int message[LENGTH];
                    matrices.top()->toMPIDataType(message);
                    MPI_Send(message, LENGTH, MPI_INT, workerRank, WORK, MPI_COMM_WORLD);
                    matrices.pop();
                    workerStatuses[workerRank] = WORKING;
                } else {
                    // Vsem pracujicim Workrum zadost o cast zasobniku
                    for (int i = 1; i < processes; ++i) {
                        if (workerStatuses[i] == WORKING) {
                            MPI_Send(message, LENGTH, MPI_INT, i, WORK_REQUEST, MPI_COMM_WORLD);
                        }
                    }
                }

            }
            winner->toMPIDataType(message);
            for (int i = 1; i < processes; ++i) {
                MPI_Send(message, LENGTH, MPI_INT, i, WINNER_UPDATE, MPI_COMM_WORLD);
            }
        }
        finalize(winner, processes);
    }
}

void runWorker(int myRank) {
    printf("Starting worker: %d\n", myRank);
    MPI_Status status;
    int message[LENGTH];
    GraphStructure * winner;

    while (true) {
        MPI_Recv(message, LENGTH, MPI_INT, 0, WORK, MPI_COMM_WORLD, &status);
        GraphStructure * balancerMessage = new GraphStructure(message);
        if (balancerMessage->getEdgesCount() == -1) {
            printf("Worker %d received finalizeMessage\n", myRank);
            return;
        } else {
            winner = findWinner(balancerMessage);
            int message[LENGTH];
            winner->toMPIDataType(message);
            MPI_Send(message, LENGTH, MPI_INT, 0, FINISHED, MPI_COMM_WORLD);
        }
    }
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
        printf("mp edges count: %d size: %d\n", mp->getEdgesCount(), mp->getMatrixSize());
        runBalancer(mp, processes);
    }

    /* shut down MPI */
    MPI_Finalize();

    return 0;
}

