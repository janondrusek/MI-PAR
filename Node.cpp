#include "Node.h"

using namespace std;

Node::Node(void) {
}

Node::Node(int rank, int processes) {
    _processes = processes;
    _rank = rank;
    _winnerEdgesCount = -1;
    _currentRank = rank;
    initEmptyWinner();
}

Node::Node(stack<GraphStructure *> matrices, int processes) {
    _matrices = matrices;
    _processes = processes;
    _rank = 0;
    _winnerEdgesCount = -1;
    _currentRank = 0;
    initEmptyWinner();
}

void Node::initEmptyWinner() {
    bool ** matrix;
    _winner = new GraphStructure(matrix, 0, 0);
}

void Node::findWinner() {
    MPI_Status status;
    int flag;
    bool winnerSet = false;
    unsigned int iteration = 0;
    int message[LENGTH];

    while (!_matrices.empty()) {
        iteration++;
        if (iteration % 100 == 0) {
            //receive work request from another worker
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
            if (flag) {
                /* receiving message by blocking receive */
                MPI_Recv(message, LENGTH, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                int tag = status.MPI_TAG;
                if (tag == WALKER_B || tag == WALKER_W) {
                    if (tag == WALKER_W && _rank != 0) {
                        _matrices.top()->toMPIDataType(message);
                        _matrices.pop();
                    }
                    MPI_Send(message, LENGTH, MPI_INT, (_rank + 1) % _processes, WALKER_B, MPI_COMM_WORLD);
                } else if (tag == WORK_REQUEST && _matrices.size() > 20) {
                    _matrices.top()->toMPIDataType(message);
                    MPI_Send(message, LENGTH, MPI_INT, status.MPI_SOURCE, WORK_RESPONSE, MPI_COMM_WORLD);
                    _matrices.pop();
                } else if (tag == WORK_REQUEST) {
                    MPI_Send(message, LENGTH, MPI_INT, status.MPI_SOURCE, WORK_RESPONSE_EMPTY, MPI_COMM_WORLD);
                }
            }
            iteration = 0;
        }
        GraphStructure * gs;

        gs = _matrices.top();
        _matrices.pop();
        if (_winnerEdgesCount >= gs->getEdgesCount()) {
            delete gs;
            continue;
        }

        if (!gs->isBipartial()) {
            if (gs->getEdgesCount() > _winnerEdgesCount + 1) {
                for (int i = 0; i < gs->getEdgesCount(); ++i) {
                    _matrices.push(new GraphStructure(gs->removeEdge(i + 1),
                            gs->getEdgesCount() - 1, gs->getMatrixSize()));
                }
            }

            delete gs;
        } else {

            if (winnerSet) {
                GraphStructure * old = _winner;
                _winner = gs;
                delete old;
            } else {
                _winner = gs;
            }
            winnerSet = true;
            _winnerEdgesCount = gs->getEdgesCount();
            printf("worker %d temporary winner with edges: %d\n", _rank, _winnerEdgesCount);
        }
    }
}

int Node::getNextNode() {
    ++_currentRank;
    if (_currentRank % _processes == _rank) {
        ++_currentRank;
    }
    return _currentRank % _processes;
}

GraphStructure * Node::run() {
    printf("Starting worker: %d\n", _rank);
    MPI_Status status;
    int message[LENGTH];

    if (_rank == 0) {
        work();
    }

    while (true) {
        MPI_Recv(message, LENGTH, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        int tag = status.MPI_TAG;
        GraphStructure * response = new GraphStructure(message);
        if (tag == FINALIZE) {
            _winner->toMPIDataType(message);
            MPI_Send(message, LENGTH, MPI_INT, 0, FINALIZE_RESPONSE, MPI_COMM_WORLD);
            delete response;
            break;
        } else if (tag == WORK_REQUEST) {
            delete response;

            MPI_Send(message, LENGTH, MPI_INT, status.MPI_SOURCE, WORK_RESPONSE, MPI_COMM_WORLD);
        } else if (tag == WORK || tag == WORK_RESPONSE || (tag == WALKER_B && _rank == 0)) {
            appendResponse(response);
            work();
        } else if (tag == WORK_RESPONSE_EMPTY) {
            delete response;

            MPI_Send(message, LENGTH, MPI_INT, getNextNode(), WORK_REQUEST, MPI_COMM_WORLD);
        } else if (tag == WALKER_B || tag == WALKER_W) {
            if (_rank == 0) {
                if (tag == WALKER_B) {
                    appendResponse(response);
                    work();
                } else {
                    delete response;
                    break;
                }
            } else {
                delete response;

                MPI_Send(message, LENGTH, MPI_INT, (_rank + 1) % _processes, tag, MPI_COMM_WORLD);
            }
        } else {
            printf("this should never happen !!!");
        }
    }
    printf("worker %d returning winner with edges: %d\n", _rank, _winnerEdgesCount);
    return _winner;
}

void Node::appendResponse(GraphStructure * response) {
    _matrices.push(response);
}

void Node::work() {
    findWinner();

    int message[LENGTH];
    if (_rank == 0) {
        MPI_Send(message, LENGTH, MPI_INT, 1, WALKER_W, MPI_COMM_WORLD);
    } else {
        MPI_Send(message, LENGTH, MPI_INT, getNextNode(), WORK_REQUEST, MPI_COMM_WORLD);
    }
}

Node::~Node() {
}






