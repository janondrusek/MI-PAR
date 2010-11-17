#include "mpi.h"
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <stack>
#include "MatrixParser.h"
#include "Node.h"
#include "GraphStructure.h"

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

void finalize(GraphStructure *winner, int processes) {
    MPI_Status status;
    GraphStructure * finalizeMessage = new GraphStructure(winner->getMatrix(), -1, 0);
    for (int i = 1; i < processes; ++i) {
        printf("Balancer sending finalizeMessage to worker %d\n", i);
        int message[LENGTH];
        printf("Building finalize message");
        finalizeMessage->toMPIDataType(message);
        printf("Built finalize message");
        MPI_Send(message, LENGTH, MPI_INT, i, FINALIZE, MPI_COMM_WORLD);
        MPI_Recv(message, LENGTH, MPI_INT, i, FINALIZE_RESPONSE, MPI_COMM_WORLD, &status);
        GraphStructure * workerWinner = new GraphStructure(message);
        if (workerWinner->getEdgesCount() > winner->getEdgesCount()) {
            winner = workerWinner;
        }
    }
    printf("\nSearch finished. Final result is:\n");
    printf("Bipartial submatrix found with %d edges:\n", winner->getEdgesCount());

    printMatrix(winner->getMatrix(), winner->getMatrixSize());
}

void runBalancer(GraphStructure *gs, int processes) {
    GraphStructure * winner;

    if (gs->isBipartial()) {
        cout << "pica\n";
        finalize(gs, processes);
    } else {
        stack <GraphStructure *> matrices;
        printf("runBalancer fill matrix, edges: %d\n", gs->getEdgesCount());
        for (int i = 0; i < gs->getEdgesCount(); ++i) {
            matrices.push(new GraphStructure(
                    gs->removeEdge(i + 1),
                    gs->getEdgesCount() - 1, gs->getMatrixSize()));
        }
        for (int i = 1; i < processes; ++i) {
            if (!matrices.empty()) {
                int message[LENGTH];
                matrices.top()->toMPIDataType(message);
                MPI_Send(message, LENGTH, MPI_INT, i, WORK, MPI_COMM_WORLD);
                matrices.pop();
            }
        }
        Node * worker = new Node(matrices, processes);
        winner = worker->run();

        finalize(winner, processes);
    }
}

int main(int argc, char** argv) {
    int rank;
    int processes;

    /* start up MPI */
    MPI_Init(&argc, &argv);

    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &processes);

    cout << "my rank: " << rank << "\n";

    if (rank != 0) {
        Node * worker = new Node(rank, processes);
        worker->run();
    } else {
        MatrixParser *mp = new MatrixParser(argc, argv);
        printf("mp edges count: %d size: %d\n", mp->getEdgesCount(), mp->getMatrixSize());
        runBalancer(mp->getGraphStructure(), processes);
    }

    /* shut down MPI */
    MPI_Finalize();

    return 0;
}

